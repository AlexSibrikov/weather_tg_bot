#include <fstream>
#include <nlohmann/json.hpp>
#include <tgbot/tgbot.h>
#include <curl/curl.h>
#include <iostream>
#include <sstream>
#include <iomanip>

class Config {
public:
    // Метод для получения единственного экземпляра класса
    static Config& getInstance() {
        static Config instance;
        return instance;
    }

    // Метод для получения значения по ключу
    template <typename T>
    T get(const std::string& key) {
        if(config.contains(key)) {
            return config[key].get<T>();
        }
        else {
            throw std::runtime_error("Key not found: " + key);
        }
    }

private:
    nlohmann::json config;

    // Приватный конструктор
    Config() {
        std::ifstream file("config.json");
        if (file.is_open()) {
            file >> config;
            file.close();
        } else {
            std::cerr << "Не удалось открыть файл config.json" << std::endl;
        }
    }

    // Приватный деструктор
    ~Config() {}

    // Запрещаем копирование
    Config(const Config&) = delete;
    Config& operator=(const Config&) = delete;

};

size_t WriteCallback(void* contents, size_t size, size_t nmemb, void* userp)
{
    ((std::string*)userp)->append((char*)contents, size * nmemb);
    return size * nmemb;
}

std::string getWeather(double latitude, double longitude) {
    Config& config = Config::getInstance();

    std::string weather_api_key = config.get<std::string>("weather_api_key");

    std::string url = "http://api.weatherapi.com/v1/current.json?key=" + weather_api_key + "&q=" + std::to_string(latitude) + "," + std::to_string(longitude);

    CURL* curl;
    CURLcode res;
    std::string readBuffer;

    curl = curl_easy_init();

    if(curl) {
        curl_easy_setopt(curl, CURLOPT_URL, url.c_str());
        curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, WriteCallback);
        curl_easy_setopt(curl, CURLOPT_WRITEDATA, &readBuffer);
        res = curl_easy_perform(curl);
        curl_easy_cleanup(curl);
    }

    if (res != CURLE_OK) {
        return "Failed to get weather data.";
    }

    nlohmann::json jsonData;
    try {
        jsonData = nlohmann::json::parse(readBuffer);
        std::string location = jsonData["location"]["name"];

        double temp = jsonData["current"]["temp_c"].get<double>();

        std::stringstream stream;
        stream << std::fixed << std::setprecision(1) << temp;
        std::string temperature = stream.str();

        std::string condition = jsonData["current"]["condition"]["text"];

        return "Location: " + location + "\nTemperature: " + temperature + "°C\nCondition: " + condition;
    } catch (nlohmann::json::exception& e) {
        std::cerr << "JSON parse error: " << e.what() << std::endl;
        return "Failed to parse weather data.";
    }
}

int main() {

    Config& config = Config::getInstance();

    std::string tg_api_key = config.get<std::string>("tg_api_key_wthr");
    
    TgBot::Bot bot(tg_api_key);

    bot.getEvents().onCommand("start", [&bot](TgBot::Message::Ptr message) {
        TgBot::ReplyKeyboardMarkup::Ptr keyboardMarkup(new TgBot::ReplyKeyboardMarkup);
        TgBot::KeyboardButton::Ptr button(new TgBot::KeyboardButton);
        button->text = "Send Location 📍";
        button->requestLocation = true;
        
        std::vector<TgBot::KeyboardButton::Ptr> row;
        row.push_back(button);
        
        keyboardMarkup->keyboard.push_back(row);
        keyboardMarkup->oneTimeKeyboard = true;
        keyboardMarkup->resizeKeyboard = true;

        bot.getApi().sendMessage(message->chat->id, "Hello! I am your weather bot. Please share your location ⬇", nullptr, nullptr, keyboardMarkup, "", false);
    });

    bot.getEvents().onCommand("bye", [&bot](TgBot::Message::Ptr message) {
        bot.getApi().sendMessage(message->chat->id, "Bye, bye! Wish to see you again.");
    });

    bot.getEvents().onAnyMessage([&bot](TgBot::Message::Ptr message) {
        if (message->location) {
            double latitude = message->location->latitude;
            double longitude = message->location->longitude;

            std::string weatherInfo = getWeather(latitude, longitude);

            bot.getApi().sendMessage(message->chat->id, weatherInfo);
            return;
        }
        else if (StringTools::startsWith(message->text, "/start") || StringTools::startsWith(message->text, "/bye")) {
            return;
        }
    });

    try {
        std::cout << "Bot is running..." << std::endl;
        TgBot::TgLongPoll longPoll(bot);
        while (true) {
            longPoll.start();
        }
    } catch (std::exception& e) {
        std::cerr << "Error: " << e.what() << std::endl;
    }

    return 0;
}
