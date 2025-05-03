#include <iostream>
#include "IncludeFile.h"

using namespace CNet;

int main()
{
	if (Network::initialize())
	{
		std::cout << "Winsock API initialized successfully." << std::endl;

		/*IPEndpoint test("www.google.com", 8080);
		if (test.getIPVersion() == IPVersion::IPV4)
		{
			std::cout << "IP Version: IPV4" << std::endl;
			std::cout << "Host Name: " << test.getHostName() << std::endl;
			std::cout << "IP: " << test.getIPString() << std::endl;
			std::cout << "Port: " << test.getPort() << std::endl;
			std::cout << "IP Bytes: ";
			for (const auto& byte : test.getIPBytes())
			{
				std::cout << static_cast<int>(byte) << " ";
			}
			std::cout << std::endl;
		}
		else
		{
			std::cerr << "This is not an IPV4 address" << std::endl;
		}*/

		Socket socket;
		if (socket.create() == PResult::P_SUCCESS)
		{
			std::cout << "Socket created successfully." << std::endl;
			if (socket.listen(IPEndpoint("0.0.0.0", 8080)) == PResult::P_SUCCESS)
			{
				std::cout << "Socket is successfully listening on port 4790." << std::endl;
				Socket newConnection;
				if (socket.accept(newConnection) == PResult::P_SUCCESS)
				{
					std::cout << "Socket accepted a connection." << std::endl;
					newConnection.close();
				}
				else
				{
					std::cerr << "Failed to accept a connection." << std::endl;
				}
			}
			else
			{
				std::cerr << "Failed to listen on port 4790." << std::endl;
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