#pragma once
#include "Socket.h"
#include "TCPSocketOption.h"
#include "Constants.h"

namespace CNet
{
	class UDPSocket : public Socket
	{
	public:
		UDPSocket(IPVersion ipVersion = IPVersion::IPV4,
			SocketHandle handle = INVALID_SOCKET);

		PResult create() override;
		PResult close() override;
		PResult sendTo(const void* data, int numberOfBytes, IPEndpoint& destination, int& bytesSent);
		PResult receiveFrom(void* destination, int numberOfBytes, IPEndpoint& source, int& bytesReceived);
	};
}