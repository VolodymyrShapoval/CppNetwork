#pragma once
#include "Socket.h"
#include "TCPSocketOption.h"
#include "Constants.h"

namespace CNet
{
	class TCPSocket : public Socket
	{
	public:
		TCPSocket(IPVersion ipVersion = IPVersion::IPV4,
			SocketHandle handle = INVALID_SOCKET);

		PResult create() override;
		PResult close() override;
		PResult listen(IPEndpoint endpoint, int backlog = 5);
		PResult accept(TCPSocket& clientSocket);
		PResult connect(IPEndpoint endpoint);
		PResult send(const void* data, int numberOfBytes, int& bytesSent);
		PResult receive(void* destination, int numberOfBytes, int& bytesReceived);
		PResult sendAll(const void* data, int numberOfBytes);
		PResult receiveAll(void* destination, int numberOfBytes);

	private:
		PResult setSocketOption(SocketOption option, BOOL value) override;
	};
}