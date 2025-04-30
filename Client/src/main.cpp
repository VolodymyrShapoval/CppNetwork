#include <iostream>
#include "IncludeFile.h"

using namespace CNet;

int main()
{
	if (Network::initialize())
	{
		std::cout << "Winsock API initialized successfully." << std::endl;
	}
	Socket socket;
	if (socket.create() == PResult::P_SUCCESS)
	{
		std::cout << "Socket created successfully." << std::endl;
		socket.close();
	}
	else
	{
		std::cerr << "Failed to create socket." << std::endl;
	}
	Network::shutdown();
	return 0;
}