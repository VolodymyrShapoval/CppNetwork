#include "UDPSocket.h"
#include "assert.h"
#include <iostream>

namespace CNet
{
	UDPSocket::UDPSocket(IPVersion ipVersion, SocketHandle handle)
		: Socket(ipVersion, handle)
	{
		assert(ipVersion == IPVersion::IPV4);
	}

	PResult UDPSocket::create()
	{
		assert(m_ipVersion == IPVersion::IPV4);

		if (m_handle != INVALID_SOCKET)
		{
			return PResult::P_GENERIC_ERROR;
		}

		m_handle = socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP);
		if (m_handle == INVALID_SOCKET)
		{
			int error = WSAGetLastError();
			return PResult::P_GENERIC_ERROR;
		}

		return PResult::P_SUCCESS;
	}

	PResult UDPSocket::close()
	{
		if (m_handle == INVALID_SOCKET)
		{
			return PResult::P_GENERIC_ERROR;
		}

		int result = closesocket(m_handle);
		if (result != 0)
		{
			int error = WSAGetLastError();
			return PResult::P_GENERIC_ERROR;
		}

		m_handle = INVALID_SOCKET;
		return PResult::P_SUCCESS;
	}

	PResult UDPSocket::sendTo(const void* data, int numberOfBytes, IPEndpoint& destination, int& bytesSent)
	{
		bytesSent = ::sendto(m_handle, (const char*)data, numberOfBytes, 0,
			(sockaddr*)(&destination.getSockAddrIPv4()), sizeof(sockaddr_in));
		if (bytesSent == SOCKET_ERROR)
		{
			int error = WSAGetLastError();
			return PResult::P_GENERIC_ERROR;
		}

		return PResult::P_SUCCESS;
	}

	PResult UDPSocket::receiveFrom(void* destination, int numberOfBytes, IPEndpoint& source, int& bytesReceived)
	{
		sockaddr_in senderAddr;
		int senderAddrSize = static_cast<int>(sizeof(sockaddr_in));
		bytesReceived = ::recvfrom(m_handle, (char*)destination, numberOfBytes, 0,
			(sockaddr*)(&senderAddr), &senderAddrSize);
		if (bytesReceived == SOCKET_ERROR)
		{
			int error = WSAGetLastError();
			return PResult::P_GENERIC_ERROR;
		}

		source = IPEndpoint((sockaddr*)(&senderAddr));

		return PResult::P_SUCCESS;
	}
}