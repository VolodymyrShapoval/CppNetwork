#pragma once
#include "SocketHandle.h"
#include "PResult.h"
#include "IPVersion.h"

namespace CNet
{
	class Socket
	{
	private:
		IPVersion m_ipVersion = IPVersion::IPV4;
		SocketHandle m_handle = INVALID_SOCKET;

	public:
		Socket(IPVersion ipVersion = IPVersion::IPV4,
			SocketHandle handle = INVALID_SOCKET);
		PResult create();
		PResult close();
		SocketHandle getHandle();
		IPVersion getIPVersion();
	};
}