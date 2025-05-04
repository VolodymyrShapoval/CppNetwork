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
		virtual PResult create();
		virtual PResult close();
		virtual PResult bind(IPEndpoint endpoint);
		virtual SocketHandle getHandle();
		virtual IPVersion getIPVersion();

	private:
		IPVersion m_ipVersion = IPVersion::IPV4;
		SocketHandle m_handle = INVALID_SOCKET;

		virtual PResult setSocketOption(SocketOption option, BOOL value);
	};
}