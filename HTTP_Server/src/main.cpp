#include "HTTP_Server.h"
#include "HTTPS_Server.h"
#include <exception>

using namespace CNet;

void startHTTP_Server(std::string executable_path);
void startHTTPS_Server(std::string executable_path);

int main(int argc, char* argv[])
{
	try
	{
		if(Network::initialize())
		{
			startHTTPS_Server(argv[0]);
		}
		
	}
	catch (const std::exception& e)
	{
		std::cerr << "Exception: " << e.what() << std::endl;
		return 1;
	}

	return 0;
}

void startHTTP_Server(std::string executable_path)
{
	HTTP_Server server(executable_path);
	server.start(80);
}

void startHTTPS_Server(std::string executable_path)
{
	HTTPS_Server server(executable_path);
	server.start(443);
}