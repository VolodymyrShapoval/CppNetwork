#include <iostream>
#include "IncludeFile.h"
#include "Constants.h"

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
				
				std::string buffer = "";
				while (true)
				{
					std::cout << "Enter message: ";
					std::getline(std::cin, buffer);
					uint32_t bufferSize = buffer.size();
					bufferSize = htonl(bufferSize);
					int result = socket.sendAll(&bufferSize, sizeof(uint32_t));
					if (result != PResult::P_SUCCESS)
					{
						break;
					}

					result = socket.sendAll(buffer.data(), buffer.size());

					if (result != PResult::P_SUCCESS)
					{
						break;
					}
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