#include "IPEndpoint.h"
#include <assert.h>
#include <iostream>

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

	IPEndpoint::IPEndpoint(sockaddr* addr)
	{
		assert(addr->sa_family == AF_INET);
		sockaddr_in* addrv4 = reinterpret_cast<sockaddr_in*>(addr);
		m_ipVersion = IPVersion::IPV4;
		m_port = ntohs(addrv4->sin_port);
		m_ipBytes.resize(sizeof(ULONG));
		memcpy(&m_ipBytes[0], &addrv4->sin_addr.S_un.S_addr, sizeof(ULONG));
		m_ipString.resize(16);
		inet_ntop(AF_INET, &addrv4->sin_addr, &m_ipString[0], m_ipString.size());
		m_hostName = m_ipString;		
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

	sockaddr_in IPEndpoint::getSockAddrIPv4() const
	{
		assert(m_ipVersion == IPVersion::IPV4);
		sockaddr_in addr = {};
		addr.sin_family = AF_INET;
		memcpy(&addr.sin_addr.S_un.S_addr, &m_ipBytes[0], sizeof(ULONG));
		addr.sin_port = htons(m_port);

		return addr;
	}

	void IPEndpoint::print() const
	{
		std::cout << "IP Version: ";
		switch (m_ipVersion)
		{
		case IPVersion::IPV4:
			std::cout << "IPV4" << std::endl;
			break;
		case IPVersion::IPV6:
			std::cout << "IPV6" << std::endl;
			break;
		default:
			std::cout << "Unknown" << std::endl;
			break;
		}
		std::cout << "Host Name: " << m_hostName << std::endl;
		std::cout << "IP: " << m_ipString << std::endl;
		std::cout << "Port: " << m_port << std::endl;
		std::cout << "IP Bytes: ";
		for (const auto& byte : m_ipBytes)
		{
			std::cout << static_cast<int>(byte) << " ";
		}
		std::cout << std::endl;
	}
}