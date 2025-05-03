#pragma once
#include "SocketHandle.h"
#include "PResult.h"
#include "IPVersion.h"
#include "SocketOption.h"
#include "IPEndpoint.h"

namespace CNet
{
	class Socket
	{
	public:
		Socket(IPVersion ipVersion = IPVersion::IPV4,
			SocketHandle handle = INVALID_SOCKET);
		PResult create();
		PResult close();
		PResult bind(IPEndpoint endpoint);
		PResult listen(IPEndpoint endpoint, int backlog = 5);
		PResult accept(Socket& clientSocket);
		PResult connect(IPEndpoint endpoint);
		PResult send(void* data, int numberOfBytes, int& bytesSent);
		PResult receive(void* destination, int numberOfBytes, int& bytesReceived);
		SocketHandle getHandle();
		IPVersion getIPVersion();

	private:
		IPVersion m_ipVersion = IPVersion::IPV4;
		SocketHandle m_handle = INVALID_SOCKET;
		PResult setSocketOption(SocketOption option, BOOL value);
	};
}