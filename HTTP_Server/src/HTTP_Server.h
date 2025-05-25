#pragma once

#include <string>
#include "IncludeFile.h"
#include <iostream>
#include <thread>
#include <regex>
#include <sstream>

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

        // Set the public directory path
        inline void publicDir(const std::string& dir) 
        {
            publicDirPath = dir;
        }

        // Start the server on the specified port
        PResult start(int port) 
        {
			TCPSocket listen_socket;
			if (listen_socket.create() != PResult::P_SUCCESS) {
				std::cerr << "Failed to create socket." << std::endl;
				int error = WSAGetLastError();
				return PResult::P_GENERIC_ERROR;
			}
            
			if (listen_socket.listen(IPEndpoint("192.168.0.107", port)) != PResult::P_SUCCESS) {
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
        /*void get(const std::string& path, std::function<void(Request&, Response&)> callback) {
            assignHandler("GET", path, callback);
        }*/

        // Add a new route with a POST method
        /*void post(const std::string& path, std::function<void(Request&, Response&)> callback) {
            assignHandler("POST", path, callback);
        }*/

    private:
        // Map to store the routes
        //std::map<std::string, std::map<std::string, std::pair<std::string, std::function<void(Request&, Response&)>>>> routes;
        // Path to the public directory
        std::string publicDirPath;

        // Add event handler to the routes map
       /* void assignHandler(const std::string& method, const std::string& path, std::function<void(Request&, Response&)> callback) {
            std::string newPath = std::regex_replace(path, std::regex("/:\\w+/?"), "/([^/]+)/?");
            routes[method][newPath] = std::pair<std::string, std::function<void(Request&, Response&)>>(path, callback);
        }*/

        // Handle the client request
        void handleClient(TCPSocket client_socket) 
        {
            std::string request = readRequest(client_socket);
            std::string method = request.substr(0, request.find(' '));
            std::string path = request.substr(request.find(' ') + 1, request.find(' ', request.find(' ') + 1) - request.find(' ') - 1);
            
			std::cout << "Received request: " << method << " " << path << std::endl;
			std::cout << "Request content:\n" << request << std::endl;

            /*Response response;
            Request req;*/

            /*if (path.find('?') != std::string::npos) 
            {
                std::string query_string = path.substr(path.find('?') + 1);
                path = path.substr(0, path.find('?'));
                std::istringstream query_iss(query_string);
                std::string query_pair;
                while (std::getline(query_iss, query_pair, '&')) {
                    std::string key = query_pair.substr(0, query_pair.find('='));
                    std::string value = query_pair.substr(query_pair.find('=') + 1);
                    req.query[key] = value;
                }
            }*/


            /*std::smatch match;
            for (auto& route : routes[method]) {
                std::string route_path = route.first;

                if (std::regex_match(path, match, std::regex(route_path))) {
                    std::regex token_regex(":\\w+");
                    std::string originalPath = routes[method][route_path].first;

                    std::vector<std::string> tokens = split_(originalPath, '/');
                    while (std::regex_search(originalPath, match, token_regex)) {
                        const std::string match_token = match.str();
                        int position = 0;
                        for (int i = 0; i < tokens.size(); i++) {
                            if (tokens[i] == match_token) {
                                position = i;
                                break;
                            }
                        }

                        std::vector<std::string> path_tokens = split_(path, '/');
                        req.params[match_token.substr(1)] = path_tokens[position];

                        originalPath = match.suffix();
                    }

                    routes[method][route_path].second(req, response);
                    serve_static_file(path, response);
                    send_response(client_socket, response);
                    SOCKET_CLOSE(client_socket);
                    return;
                }
            }*/

            //// Serve static files if not matched by any route
            //serve_static_file(path, response);
            //send_response(client_socket, response);
			client_socket.close(); // Close the client socket after handling the request
        }

        //void serve_static_file(const std::string& path, Response& response) {

        //    // Determine the content type based on the file extension
        //    std::string content_type;
        //    std::string file_extension = path.substr(path.find_last_of('.') + 1);
        //    if (file_extension == "html") {
        //        content_type = "text/html";
        //    }
        //    else if (file_extension == "css") {
        //        content_type = "text/css";
        //    }
        //    else if (file_extension == "js") {
        //        content_type = "application/javascript";
        //    }
        //    else if (file_extension == "json") {
        //        content_type = "application/json";
        //    }
        //    else if (file_extension == "jpg" || file_extension == "jpeg") {
        //        content_type = "image/jpeg";
        //    }
        //    else if (file_extension == "png") {
        //        content_type = "image/png";
        //    }
        //    else if (file_extension == "gif") {
        //        content_type = "image/gif";
        //    }
        //    else {
        //        // Default to octet-stream for unknown file types
        //        content_type = "application/octet-stream";
        //    }
        //    std::fstream file(publicDirPath + path, std::ios::in | std::ios::binary);
        //    if (file) {

        //        response << std::string((std::istreambuf_iterator<char>(file)), std::istreambuf_iterator<char>());
        //        file.close();
        //        // Set the response headers
        //        response.header("Content-Type", content_type);
        //    }
        //}

        // Read the request from the client
        std::string readRequest(TCPSocket client_socket) {
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

        /*void send_response(SOCKET client_socket, Response response) {
            response.header("Content-Length", std::to_string(response.body.size()));
            response.header("X-Powered-By", "Xebec-Server/0.1.0");
            response.header("Programming-Language", "C++");
            response.headers += "\r\n";
            std::string res = "HTTP/1.1 " + response.status + response.headers + response.body;
            send(client_socket, res.c_str(), res.size(), 0);
        }*/

    };
}