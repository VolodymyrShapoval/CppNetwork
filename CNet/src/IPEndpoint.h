#pragma once
#include "IPVersion.h"
#include <string>
#include <vector>
#include <WS2tcpip.h>

namespace CNet
{
	class IPEndpoint
	{
	public:
		IPEndpoint(const char* ip, unsigned short port);
		IPEndpoint(sockaddr* addr);
		IPVersion getIPVersion() const;
		std::vector<uint8_t> getIPBytes() const;
		std::string getHostName() const;
		std::string getIPString() const;
		unsigned short getPort() const;
		sockaddr_in getSockAddrIPv4() const;
		void print() const;

	private:
		IPVersion m_ipVersion = IPVersion::Unknown;
		std::string m_hostName = "";
		std::string m_ipString = "";
		std::vector<uint8_t> m_ipBytes;
		unsigned short m_port = 0;
	};
}