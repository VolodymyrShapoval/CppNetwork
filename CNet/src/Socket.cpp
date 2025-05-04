#include "Socket.h"
#include "assert.h"
#include <iostream>

namespace CNet
{
    Socket::Socket(IPVersion ipVersion, SocketHandle handle)
		: m_ipVersion(ipVersion), m_handle(handle)
    {
		assert(ipVersion == IPVersion::IPV4);
    }

    PResult Socket::create()
    {
		assert(m_ipVersion == IPVersion::IPV4);

		if (m_handle != INVALID_SOCKET)
		{
			return PResult::P_GENERIC_ERROR;
		}

		m_handle = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
        if (m_handle == INVALID_SOCKET)
        {
			int error = WSAGetLastError();
			return PResult::P_GENERIC_ERROR;
        }

        if (setSocketOption(SocketOption::TCP_NO_DELAY, TRUE) != PResult::P_SUCCESS)
        {
			return PResult::P_GENERIC_ERROR;
        }

        return PResult::P_SUCCESS;
    }

    PResult Socket::close()
    {
        if (m_handle == INVALID_SOCKET)
        {
			return PResult::P_GENERIC_ERROR;
        }

		int result = shutdown(m_handle, SD_BOTH);
		if (result != 0)
		{
			int error = WSAGetLastError();
			return PResult::P_GENERIC_ERROR;
		}

		result = closesocket(m_handle);
        if (result != 0)
        {
			int error = WSAGetLastError();
			return PResult::P_GENERIC_ERROR;
        }

		m_handle = INVALID_SOCKET;
        return PResult::P_SUCCESS;
    }

    PResult Socket::bind(IPEndpoint endpoint)
    {
		sockaddr_in addr = endpoint.getSockAddrIPv4();
		int result = ::bind(m_handle, (sockaddr*)(&addr), sizeof(sockaddr_in));
        if (result != 0)
        {
			int error = WSAGetLastError();
			return PResult::P_GENERIC_ERROR;
        }
        return PResult::P_SUCCESS;
    }

    PResult Socket::listen(IPEndpoint endpoint, int backlog)
    {
        if (bind(endpoint) != PResult::P_SUCCESS)
        {
			return PResult::P_GENERIC_ERROR;
        }

		int result = ::listen(m_handle, backlog);

        if (result != 0)
        {
			int error = WSAGetLastError();
			return PResult::P_GENERIC_ERROR;
        }

        return PResult::P_SUCCESS;
    }

    PResult Socket::accept(Socket& clientSocket)
    {
        sockaddr_in addr = {};
		int len = sizeof(sockaddr_in);
		SocketHandle acceptedConnectionHandle = ::accept(m_handle, (sockaddr*)(&addr), &len);
        if (acceptedConnectionHandle == INVALID_SOCKET)
        {
			int error = WSAGetLastError();
			return PResult::P_GENERIC_ERROR;
        }
		IPEndpoint newConnectionIPEndpoint((sockaddr*)(&addr));
		std::cout << "New connection accepted:" << std::endl;
		newConnectionIPEndpoint.print();
		clientSocket = Socket(IPVersion::IPV4, acceptedConnectionHandle);
        return PResult::P_SUCCESS;
    }

    PResult Socket::connect(IPEndpoint endpoint)
    {
		sockaddr_in addr = endpoint.getSockAddrIPv4();
		int result = ::connect(m_handle, (sockaddr*)(&addr), sizeof(sockaddr_in));
        if (result != 0)
        {
			int error = WSAGetLastError();
			return PResult::P_GENERIC_ERROR;
        }
        return PResult::P_SUCCESS;
    }

    PResult Socket::send(const void* data, int numberOfBytes, int& bytesSent)
    {
		bytesSent = ::send(m_handle, (const char*)data, numberOfBytes, NULL);
        if (bytesSent == SOCKET_ERROR)
        {
			int error = WSAGetLastError();
			return PResult::P_GENERIC_ERROR;
        }

        return PResult::P_SUCCESS;
    }

    PResult Socket::receive(void* destination, int numberOfBytes, int& bytesReceived)
    {
		bytesReceived = ::recv(m_handle, (char*)destination, numberOfBytes, NULL);
        if (bytesReceived == 0)
        {
			return PResult::P_GENERIC_ERROR;
        }
        if (bytesReceived == SOCKET_ERROR)
        {
			int error = WSAGetLastError();
			return PResult::P_GENERIC_ERROR;
        }

        return PResult::P_SUCCESS;
    }

    PResult Socket::sendAll(const void* data, int numberOfBytes)
    {
        int totalBytesSent = 0;

        while (totalBytesSent < numberOfBytes)
        {
			int bytesRemaining = numberOfBytes - totalBytesSent;
            int bytesSent = 0;
			char* bufferOffset = (char*)data + totalBytesSent;
			PResult result = send(bufferOffset, bytesRemaining, bytesSent);
            if (result != PResult::P_SUCCESS)
            {
				return PResult::P_GENERIC_ERROR;
            }
			totalBytesSent += bytesSent;
        }
        return PResult::P_SUCCESS;
    }

    PResult Socket::receiveAll(void* destination, int numberOfBytes)
    {
        int totalBytesReceived = 0;

        while (totalBytesReceived < numberOfBytes)
        {
            int bytesRemaining = numberOfBytes - totalBytesReceived;
            int bytesReceived = 0;
            char* bufferOffset = (char*)destination + totalBytesReceived;
            PResult result = receive(bufferOffset, bytesRemaining, bytesReceived);
            if (result != PResult::P_SUCCESS)
            {
                return PResult::P_GENERIC_ERROR;
            }
            totalBytesReceived += bytesReceived;
        }
        return PResult::P_SUCCESS;
    }

    SocketHandle Socket::getHandle()
    {
		return m_handle;
    }

    IPVersion Socket::getIPVersion()
    {
		return m_ipVersion;
    }

    PResult Socket::setSocketOption(SocketOption option, BOOL value)
    {
		int result = 0;
        switch (option)
        {
        case SocketOption::TCP_NO_DELAY:
			result = setsockopt(m_handle, IPPROTO_TCP, TCP_NODELAY, (const char*)&value, sizeof(value));
            break;
        default:
            return PResult::P_GENERIC_ERROR;
        }

        if (result != 0)
        {
			int error = WSAGetLastError();
			return PResult::P_GENERIC_ERROR;
        }

		return PResult::P_SUCCESS;
    }
}