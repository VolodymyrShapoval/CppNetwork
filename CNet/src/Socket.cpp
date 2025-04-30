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

    SocketHandle Socket::getHandle()
    {
		return m_handle;
    }

    IPVersion Socket::getIPVersion()
    {
		return m_ipVersion;
    }
}