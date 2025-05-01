#include "IPEndpoint.h"
#include <WS2tcpip.h>

namespace CNet
{
	IPEndpoint::IPEndpoint(const char* ip, unsigned short port)
	{
		m_port = port;

		in_addr addr; // Location to store the IPV4 address
		int result = inet_pton(AF_INET, ip, &addr);

		if (result == 1)
		{
			if (addr.S_un.S_addr != INADDR_NONE)
			{
				m_ipString = ip;
				m_hostName = ip;

				m_ipBytes.resize(sizeof(ULONG));
				memcpy(&m_ipBytes[0], &addr.S_un.S_addr, sizeof(ULONG));

				m_ipVersion = IPVersion::IPV4;
				return;
			}
		}

	}

	IPVersion IPEndpoint::getIPVersion() const
	{
		return m_ipVersion;
	}

	std::vector<uint8_t> IPEndpoint::getIPBytes() const
	{
		return m_ipBytes;
	}

	std::string IPEndpoint::getHostName() const
	{
		return m_hostName;
	}

	std::string IPEndpoint::getIPString() const
	{
		return m_ipString;
	}

	unsigned short IPEndpoint::getPort() const
	{
		return m_port;
	}
}