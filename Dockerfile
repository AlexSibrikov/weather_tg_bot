# Use an official Ubuntu as a parent image
FROM --platform=linux/amd64 ubuntu:20.04

# Set environment variables to avoid interactive prompts during package installation
ENV DEBIAN_FRONTEND=noninteractive

# Install dependencies
RUN apt-get update && \
    apt-get install -y g++ make cmake build-essential libssl-dev libcurl4-openssl-dev libjsoncpp-dev libboost-system-dev zlib1g-dev nlohmann-json3-dev git && \
    apt-get clean

# Install tgbot-cpp library (replace this section with the appropriate installation method)
# Assuming you clone and build tgbot-cpp from source, modify if necessary
RUN git clone https://github.com/reo7sp/tgbot-cpp /tgbot-cpp && \
    cd /tgbot-cpp && \
    cmake . && \
    make -j4 && \
    make install

# Create and set the working directory
WORKDIR /weather_bot

# Copy the current directory contents into the container at /weather_bot
COPY . .

# Build the project
RUN g++ main.cpp -o botw \
    -I/usr/local/include \
    -L/usr/local/lib \
    -lTgBot -lcurl -ljsoncpp -lboost_system -lssl -lcrypto -lpthread

# # Make port 80 available to the world outside this container
# EXPOSE 80

# Run the bot
CMD ["./botw"]
