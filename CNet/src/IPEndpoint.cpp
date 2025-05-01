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

		// Attempt to resolve the hostname to IPV4 address
		addrinfo hints = {};
		hints.ai_family = AF_INET; // IPv4 addresses only
		addrinfo* hostInfo = nullptr;
		result = getaddrinfo(ip, NULL, &hints, &hostInfo);
		if (result == 0)
		{
			sockaddr_in* host_addr = reinterpret_cast<sockaddr_in*>(hostInfo->ai_addr);

			//host_addr->sin_addr.S_un.S_addr = ntohl(host_addr->sin_addr.S_un.S_addr);
			m_ipString.resize(16);
			inet_ntop(AF_INET, &host_addr->sin_addr, &m_ipString[0], m_ipString.size());

			m_hostName = ip;

			ULONG ip_long = host_addr->sin_addr.S_un.S_addr;
			m_ipBytes.resize(sizeof(ULONG));
			memcpy(&m_ipBytes[0], &ip_long, sizeof(ULONG));

			m_ipVersion = IPVersion::IPV4;

			freeaddrinfo(hostInfo);
			return;
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