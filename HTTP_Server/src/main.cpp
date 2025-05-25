#include "HTTP_Server.h"
#include <exception>

using namespace CNet;

int main()
{
	try
	{
		HTTP_Server server;
		server.start(80);

	}
	catch (const std::exception& e)
	{
		std::cerr << "Exception: " << e.what() << std::endl;
		return 1;
	}

	return 0;
}