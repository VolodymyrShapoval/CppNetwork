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
#include <openssl/ssl.h>
#include <openssl/err.h>
#include <fstream>

namespace CNet
{
    class HTTPS_Server
    {
    public:
        HTTPS_Server(std::string& executable_path)
        {
            setPublicDirPath(executable_path);
            SSL_library_init();
            OpenSSL_add_all_algorithms();
            SSL_load_error_strings();

            const SSL_METHOD* method = TLS_server_method();
            ssl_ctx = SSL_CTX_new(method);

            if (!ssl_ctx) {
                ERR_print_errors_fp(stderr);
                throw std::runtime_error("Failed to create SSL context");
            }

			std::string certPath = getSertificatesDirPath(executable_path) + "cert.pem";
			std::string keyPath = getSertificatesDirPath(executable_path) + "key.pem";

            if (SSL_CTX_use_certificate_file(ssl_ctx, certPath.c_str(), SSL_FILETYPE_PEM) <= 0 ||
                SSL_CTX_use_PrivateKey_file(ssl_ctx, keyPath.c_str(), SSL_FILETYPE_PEM) <= 0)
            {
                ERR_print_errors_fp(stderr);
                throw std::runtime_error("Failed to load cert/key");
            }
        }

        ~HTTPS_Server()
        {
            SSL_CTX_free(ssl_ctx);
        }

        PResult start(int port)
        {
            TCPSocket listen_socket;
            if (listen_socket.create() != PResult::P_SUCCESS) {
                std::cerr << "Failed to create socket." << std::endl;
                return PResult::P_GENERIC_ERROR;
            }

            if (listen_socket.listen(IPEndpoint("192.168.0.105", port)) != PResult::P_SUCCESS) {
                std::cerr << "Failed to listen on port " << port << std::endl;
                listen_socket.close();
                return PResult::P_GENERIC_ERROR;
            }

            std::cout << "HTTPS Server is listening on port " << port << std::endl;

            while (true)
            {
                TCPSocket client_socket;
                if (listen_socket.accept(client_socket) != PResult::P_SUCCESS)
                    continue;

                std::thread t(&HTTPS_Server::handleClient, this, client_socket);
                t.detach();
            }

            return PResult::P_SUCCESS;
        }

        void get(const std::string& path, std::function<void(Request&, Response&)> callback) {
            assignHandler("GET", path, callback);
        }

        void post(const std::string& path, std::function<void(Request&, Response&)> callback) {
            assignHandler("POST", path, callback);
        }

    private:
        std::map<std::string, std::map<std::string, std::pair<std::string, std::function<void(Request&, Response&)>>>> routes;
        std::string publicDirPath;
        SSL_CTX* ssl_ctx = nullptr;

        void setPublicDirPath(const std::string& executable_path)
        {
            size_t lastSlashPos = executable_path.find_last_of("/\\");
            if (lastSlashPos != std::string::npos)
                publicDirPath = executable_path.substr(0, lastSlashPos + 1) + "public/";
            else
                publicDirPath = "public/";
        }

        std::string getSertificatesDirPath(const std::string& executable_path)
        {
            size_t lastSlashPos = executable_path.find_last_of("/\\");
            if (lastSlashPos != std::string::npos)
                return executable_path.substr(0, lastSlashPos + 1) + "certificates/";
            else
                return "certificates/";
        }

        void assignHandler(const std::string& method, const std::string& path, std::function<void(Request&, Response&)> callback) {
            std::string newPath = std::regex_replace(path, std::regex("/:\\w+/?"), "/([^/]+)/?");
            routes[method][newPath] = std::make_pair(path, callback);
        }

        void handleClient(TCPSocket client_socket)
        {
            SSL* ssl = SSL_new(ssl_ctx);
            SSL_set_fd(ssl, client_socket.getHandle());

            if (SSL_accept(ssl) <= 0) {
                ERR_print_errors_fp(stderr);
                SSL_free(ssl);
                client_socket.close();
                return;
            }

            std::string request = readRequest(ssl);
            std::string method = request.substr(0, request.find(' '));
            std::string path = request.substr(request.find(' ') + 1, request.find(' ', request.find(' ') + 1) - request.find(' ') - 1);

            std::cout << "Received request: " << method << " " << path << std::endl;

            Response response;
            Request req;

            std::smatch match;
            for (auto& route : routes[method])
            {
                if (std::regex_match(path, match, std::regex(route.first)))
                {
                    std::regex token_regex(":\\w+");
                    std::string originalPath = route.second.first;

                    std::vector<std::string> tokens = split_(originalPath, '/');
                    while (std::regex_search(originalPath, match, token_regex))
                    {
                        originalPath = match.suffix();
                    }

                    route.second.second(req, response);
                    serveStaticFile(path, response);
                    sendResponse(ssl, response);
                    SSL_shutdown(ssl);
                    SSL_free(ssl);
                    client_socket.close();
                    return;
                }
            }

            serveStaticFile(path, response);
            sendResponse(ssl, response);
            SSL_shutdown(ssl);
            SSL_free(ssl);
            client_socket.close();
        }

        std::string readRequest(SSL* ssl)
        {
            std::string request;
            char buffer[1024];
            int bytes_received;

            do {
                bytes_received = SSL_read(ssl, buffer, sizeof(buffer));
                if (bytes_received > 0)
                    request.append(buffer, bytes_received);
            } while (bytes_received == sizeof(buffer));

            return request;
        }

        void sendResponse(SSL* ssl, Response response)
        {
            response.header("Content-Length", std::to_string(response.body.size()));
            response.header("X-Powered-By", "Xebec-Server/0.1.0");
            response.header("Programming-Language", "C++");
            response.headers += "\r\n";

            std::string res = "HTTP/1.1 " + response.status + response.headers + response.body;
            SSL_write(ssl, res.c_str(), res.size());
        }

        void serveStaticFile(const std::string& path, Response& response)
        {
            std::string requestedPath = (path == "/") ? "/index.html" : path;
            std::string fullPath = publicDirPath + requestedPath;

            std::ifstream file(fullPath, std::ios::in | std::ios::binary);
            if (file)
            {
                std::ostringstream buffer;
                buffer << file.rdbuf();
                file.close();

                std::string content_type;
                std::string extension = requestedPath.substr(requestedPath.find_last_of('.') + 1);

                if (extension == "html") content_type = "text/html";
                else if (extension == "jpg" || extension == "jpeg") content_type = "image/jpeg";
                else content_type = "application/octet-stream";

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
    };
}
