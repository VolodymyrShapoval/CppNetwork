#include "Socket.h"

namespace CNet
{
    Socket::Socket(IPVersion ipVersion, SocketHandle handle) : 
        m_ipVersion(ipVersion), m_handle(handle) { }

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
        return PResult::P_GENERIC_ERROR;
    }
}
