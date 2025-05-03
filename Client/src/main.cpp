#include <iostream>
#include "IncludeFile.h"

using namespace CNet;

int main()
{
	if (Network::initialize())
	{
		std::cout << "Winsock API initialized successfully." << std::endl;
		Socket socket;
		if (socket.create() == PResult::P_SUCCESS)
		{
			std::cout << "Socket created successfully." << std::endl;
			if (socket.connect(IPEndpoint("127.0.0.1", 8080)) == PResult::P_SUCCESS)
			{
				std::cout << "Socket connected successfully." << std::endl;
				char buffer[1024];
				strcpy(buffer, "Hello, Server!\0");
				int result = PResult::P_SUCCESS;
				while (result == PResult::P_SUCCESS)
				{
					result = socket.sendAll(buffer, sizeof(buffer));
					std::cout << "Attempting to send data..." << std::endl;
					Sleep(500);
				}
			}
			else
			{
				std::cerr << "Failed to connect to the server." << std::endl;
			}
			socket.close();
		}
		else
		{
			std::cerr << "Failed to create socket." << std::endl;
		}
	}
	Network::shutdown();
	system("pause");
	return 0;
}