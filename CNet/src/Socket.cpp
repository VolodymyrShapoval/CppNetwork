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
			return PResult::P_NOT_YET_IMPLEMENTED;
		}

		m_handle = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
        if (m_handle == INVALID_SOCKET)
        {
			int error = WSAGetLastError();
			return PResult::P_NOT_YET_IMPLEMENTED;
        }

        if (setSocketOption(SocketOption::TCP_NO_DELAY, TRUE) != PResult::P_SUCCESS)
        {
			return PResult::P_NOT_YET_IMPLEMENTED;
        }

        return PResult::P_SUCCESS;
    }

    PResult Socket::close()
    {
        if (m_handle == INVALID_SOCKET)
        {
			return PResult::P_NOT_YET_IMPLEMENTED;
        }

		int result = closesocket(m_handle);
        if (result != 0)
        {
			int error = WSAGetLastError();
			return PResult::P_NOT_YET_IMPLEMENTED;
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
			return PResult::P_NOT_YET_IMPLEMENTED;
        }
        return PResult::P_SUCCESS;
    }

    PResult Socket::listen(IPEndpoint endpoint, int backlog)
    {
        if (bind(endpoint) != PResult::P_SUCCESS)
        {
			return PResult::P_NOT_YET_IMPLEMENTED;
        }

		int result = ::listen(m_handle, backlog);

        if (result != 0)
        {
			int error = WSAGetLastError();
			return PResult::P_NOT_YET_IMPLEMENTED;
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
			return PResult::P_NOT_YET_IMPLEMENTED;
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
			return PResult::P_NOT_YET_IMPLEMENTED;
        }
        return PResult::P_SUCCESS;
    }

    PResult Socket::send(void* data, int numberOfBytes, int& bytesSent)
    {
		bytesSent = ::send(m_handle, (const char*)data, numberOfBytes, NULL);
        if (bytesSent == SOCKET_ERROR)
        {
			int error = WSAGetLastError();
			return PResult::P_NOT_YET_IMPLEMENTED;
        }

        return PResult::P_SUCCESS;
    }

    PResult Socket::receive(void* destination, int numberOfBytes, int& bytesReceived)
    {
		bytesReceived = ::recv(m_handle, (char*)destination, numberOfBytes, NULL);
        if (bytesReceived == 0)
        {
			return PResult::P_NOT_YET_IMPLEMENTED;
        }
        if (bytesReceived == SOCKET_ERROR)
        {
			int error = WSAGetLastError();
			return PResult::P_NOT_YET_IMPLEMENTED;
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
            return PResult::P_NOT_YET_IMPLEMENTED;
        }

        if (result != 0)
        {
			int error = WSAGetLastError();
			return PResult::P_NOT_YET_IMPLEMENTED;
        }

		return PResult::P_SUCCESS;
    }
}