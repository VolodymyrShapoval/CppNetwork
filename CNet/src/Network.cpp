#include "Network.h"
#include <iostream>

namespace CNet
{
	bool Network::initialize()
	{
		WSADATA wsaData;
		int result = WSAStartup(MAKEWORD(2, 2), &wsaData);
		if (result != 0)
		{
			std::cerr << "Failed to start up the winsock API" << result << std::endl;
			return false;
		}

		if (LOBYTE(wsaData.wVersion) != 2 || HIBYTE(wsaData.wVersion) != 2)
		{
			std::cerr << "Could not find a usable version of winsock.dll" << std::endl;
			return false;
		}

		return true;
	}
	void Network::shutdown()
	{
		WSACleanup();
	}
}