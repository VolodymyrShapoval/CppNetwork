#pragma once
#include "Network.h"
#include "TCPSocket.h"
#include <string>
#include <vector>
#include <sstream>

std::vector<std::string> split_(const std::string& path, char delimiter) {
    std::vector<std::string> tokens;
    std::string token;
    std::istringstream iss(path); // Convert the string to a stream
    while (std::getline(iss, token, delimiter)) {
        tokens.push_back(token); // Add the token to the vector
    }
    return tokens;
}