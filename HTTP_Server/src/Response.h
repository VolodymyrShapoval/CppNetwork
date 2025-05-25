#pragma once
#include <string>
#include <fstream>

namespace CNet
{
	class Response
	{
	public:
		std::string status; // HTTP status code and message
		std::string body;   // Response body content
		std::string headers; // Response headers
		
		Response() : status("200 OK\r\n") {}
		
		Response& statusCode(const int& code) 
		{
			status = std::to_string(code) + " OK\r\n";
			return *this;
		}

		// Add a header to the response
		Response& header(const std::string& key, const std::string& value) 
		{
			headers += key + ": " + value + "\r\n";
			return *this;
		}

		Response& html(const std::string& path) 
		{
			header("Content-Type", "text/html");
			std::fstream file(path, std::ios::in | std::ios::binary);
			if (file.is_open()) 
			{
				body = std::string((std::istreambuf_iterator<char>(file)), std::istreambuf_iterator<char>());
				file.close();
			}
			else 
			{
				statusCode(404);
				body += "File Not Found";
			}
			return *this;
		}
	};
}