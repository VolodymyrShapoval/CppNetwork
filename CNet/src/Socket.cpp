#include "Socket.h"
#include "assert.h"

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