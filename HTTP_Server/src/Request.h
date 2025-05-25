#pragma once
#include <string>
#include <map>


namespace CNet
{
	class Request
	{
    public:
        std::string method;
        std::string uri;
        std::string body;
		std::map<std::string, std::string> headers;

        Request() {}
	};
}