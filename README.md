# Weather_tg_bot

This is a simple telegram bot for weather informing. 

## External sources

– https://github.com/reo7sp/tgbot-cpp.git // API for Telegram
– https://www.weatherapi.com // Weather data


## Project Structure

- `Dockerfile`: Contains the instructions to build the Docker image.
- `config.json`: Configuration file for the project. Add your API tokens.
- `main.cpp`: Main source file for the project.

## Getting Started

To get started, you need to:
- Add API tokens(tg bot and weatherAPI) to config.json. 
- Install Docker on your system.

Follow these steps to build and run the project:

```bash
docker build -t my_docker_project .
docker run my_docker_project
```

## LICENSE

This project is licensed under the MIT License. See the LICENSE file for details.