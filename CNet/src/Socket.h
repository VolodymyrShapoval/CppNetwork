#pragma once
#include "SocketHandle.h"
#include "PResult.h"
#include "IPVersion.h"
#include "SocketOption.h"
#include "IPEndpoint.h"
#include "Constants.h"

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
		PResult send(const void* data, int numberOfBytes, int& bytesSent);
		PResult receive(void* destination, int numberOfBytes, int& bytesReceived);
		PResult sendAll(const void* data, int numberOfBytes);
		PResult receiveAll(void* destination, int numberOfBytes);
		SocketHandle getHandle();
		IPVersion getIPVersion();

	private:
		IPVersion m_ipVersion = IPVersion::IPV4;
		SocketHandle m_handle = INVALID_SOCKET;
		PResult setSocketOption(SocketOption option, BOOL value);
	};
}