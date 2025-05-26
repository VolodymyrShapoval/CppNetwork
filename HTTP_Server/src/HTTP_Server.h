#pragma once
#include <string>
#include "IncludeFile.h"
#include "Response.h"
#include "Request.h"
#include <iostream>
#include <thread>
#include <regex>
#include <sstream>
#include <functional>
#include <map>

namespace CNet
{
    std::vector<std::string> split_(const std::string& path, char delimiter) {
        std::vector<std::string> tokens;
        std::string token;
        std::istringstream iss(path); // Convert the string to a stream
        while (std::getline(iss, token, delimiter)) {
            tokens.push_back(token); // Add the token to the vector
        }
        return tokens;
    }

	class HTTP_Server
	{
    public:
        HTTP_Server()
        {
			Network::initialize(); // Initialize Winsock
        }

		~HTTP_Server() 
        {
			Network::shutdown(); // Clean up Winsock
		}

        // // Set the public directory path
        // inline void publicDir(const std::string& dir) 
        // {
        //     publicDirPath = dir;
        // }

        // Start the server on the specified port
        PResult start(int port) 
        {
			TCPSocket listen_socket;
			if (listen_socket.create() != PResult::P_SUCCESS) {
				std::cerr << "Failed to create socket." << std::endl;
				int error = WSAGetLastError();
				return PResult::P_GENERIC_ERROR;
			}
            
			if (listen_socket.listen(IPEndpoint("10.211.55.4", port)) != PResult::P_SUCCESS) {
				std::cerr << "Failed to listen on port " << port << std::endl;
				int error = WSAGetLastError();
                listen_socket.close();
				return PResult::P_GENERIC_ERROR;
			}

            std::cout << "Server is listening on port " << port << std::endl;

            while (true) 
            {
                TCPSocket client_socket;
                if (listen_socket.accept(client_socket) != PResult::P_SUCCESS)
                {
                    continue;
                }

                std::thread t(&HTTP_Server::handleClient, this, client_socket); // Create a new thread to handle the client
                t.detach();
            }
			std::cout << "Server stopped." << std::endl;
			return PResult::P_SUCCESS;
        }

        // Add a new route with a GET method
        void get(const std::string& path, std::function<void(Request&, Response&)> callback) {
            assignHandler("GET", path, callback);
        }

        // Add a new route with a POST method
        void post(const std::string& path, std::function<void(Request&, Response&)> callback) {
            assignHandler("POST", path, callback);
        }

    private:
        // Map to store the routes
        std::map<std::string, std::map<std::string, std::pair<std::string, std::function<void(Request&, Response&)>>>> routes;
        // Path to the public directory
        std::string publicDirPath = "public";

        // Add event handler to the routes map
        void assignHandler(const std::string& method, const std::string& path, std::function<void(Request&, Response&)> callback) {
            std::string newPath = std::regex_replace(path, std::regex("/:\\w+/?"), "/([^/]+)/?");
            routes[method][newPath] = std::pair<std::string, std::function<void(Request&, Response&)>>(path, callback);
        }

        // Handle the client request
        void handleClient(TCPSocket client_socket) 
        {
            std::string request = readRequest(client_socket);
            std::string method = request.substr(0, request.find(' '));
            std::string path = request.substr(request.find(' ') + 1, request.find(' ', request.find(' ') + 1) - request.find(' ') - 1);
            
			std::cout << "Received request: " << method << " " << path << std::endl;
			std::cout << "Request content:\n" << request << std::endl;

            Response response;
            Request req;

            std::smatch match;
            for (auto& route : routes[method]) 
            {
                std::string route_path = route.first;

                if (std::regex_match(path, match, std::regex(route_path))) 
                {
                    std::regex token_regex(":\\w+");
                    std::string originalPath = routes[method][route_path].first;

                    std::vector<std::string> tokens = split_(originalPath, '/');
                    while (std::regex_search(originalPath, match, token_regex)) 
                    {
                        const std::string match_token = match.str();
                        int position = 0;
                        for (int i = 0; i < tokens.size(); i++) 
                        {
                            if (tokens[i] == match_token) 
                            {
                                position = i;
                                break;
                            }
                        }
                        originalPath = match.suffix();
                    }

                    routes[method][route_path].second(req, response);
                    serveStaticFile(path, response);
                    sendResponse(client_socket, response);
                    client_socket.close();
                    return;
                }
            }

            // Serve static files if not matched by any route
            serveStaticFile(path, response);
            sendResponse(client_socket, response);
			client_socket.close(); // Close the client socket after handling the request
        }

        void serveStaticFile(const std::string& path, Response& response) 
        {
            std::string requestedPath = path;

            if (requestedPath == "/") {
                requestedPath = "/index.html";
            }

            std::string fullPath = publicDirPath + requestedPath;

            std::fstream file(fullPath, std::ios::in | std::ios::binary);
            if (file)
            {
                std::ostringstream buffer;
                buffer << file.rdbuf();
                file.close();

                std::string content_type;
                std::string file_extension = requestedPath.substr(requestedPath.find_last_of('.') + 1);
                if (file_extension == "html") 
                    content_type = "text/html";
                else if (file_extension == "jpg" || file_extension == "jpeg")
                    content_type = "image/jpeg";
                else
                    content_type = "application/octet-stream";

                response.body = buffer.str();
                response.header("Content-Type", content_type);
                response.status = "200 OK";
            }
            else
            {
                response.status = "404 Not Found";
                response.body = "<h1>404 Not Found</h1>";
                response.header("Content-Type", "text/html");
            }
        }

        // Read the request from the client
        std::string readRequest(TCPSocket client_socket) 
        {
            std::string request;
            char buffer[1024]; // Buffer to store the request
            int bytes_received; // Bytes received from the client
            do 
            {
                bytes_received = recv(client_socket.getHandle(), buffer, 1024, 0); // Receive the request
                if (bytes_received > 0) 
                {
                    request.append(buffer, bytes_received);
                }
            } while (bytes_received == 1024);
            return request;
        }

        void sendResponse(TCPSocket client_socket, Response response) 
        {
            response.header("Content-Length", std::to_string(response.body.size()));
            response.header("X-Powered-By", "Xebec-Server/0.1.0");
            response.header("Programming-Language", "C++");
            response.headers += "\r\n";
            std::string res = "HTTP/1.1 " + response.status + response.headers + response.body;
            client_socket.sendAll(res.c_str(), res.size());
        }
    };
}