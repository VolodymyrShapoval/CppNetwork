#pragma once
#include "SocketHandle.h"
#include "TCPSocketOption.h"
#include "PResult.h"
#include "IPVersion.h"
#include "IPEndpoint.h"

namespace CNet
{
	class Socket
	{
	public:
		Socket(IPVersion ipVersion, SocketHandle handle);
		virtual PResult create() = 0;
		virtual PResult close() = 0;
		virtual PResult bind(IPEndpoint endpoint);
		virtual SocketHandle getHandle();
		virtual IPVersion getIPVersion();

	protected:
		IPVersion m_ipVersion = IPVersion::IPV4;
		SocketHandle m_handle = INVALID_SOCKET;

		virtual PResult setSocketOption(SocketOption option, BOOL value);
	};
}