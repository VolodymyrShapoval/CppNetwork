#include <iostream>
#include "IncludeFile.h"
#include "Constants.h"
#include <UDPSocket.h>
#include <mutex>

using namespace CNet;
std::mutex consoleMutex;

void TCPServer();
void UDPServer();

int main()
{
	if (Network::initialize())
	{
		TCPServer();
	}
	Network::shutdown();
	system("pause");
	return 0;
}

// ------------- TCP --------------------------------

void StartReceiveLoop(TCPSocket& connection)
{
	std::string buffer;
	while (true)
	{
		uint32_t bufferSize = 0;
		int result = connection.receiveAll(&bufferSize, sizeof(uint32_t));
		if (result != PResult::P_SUCCESS)
			break;

		bufferSize = ntohl(bufferSize);
		if (bufferSize > g_MAX_PACKET_SIZE)
			break;

		buffer.resize(bufferSize);
		result = connection.receiveAll(&buffer[0], bufferSize);
		if (result != PResult::P_SUCCESS)
			break;

		std::lock_guard<std::mutex> lock(consoleMutex);
		std::cout << "\r\33[K"; // Очистити поточний рядок
		std::cout << "[Client] " << buffer << std::endl;
		std::cout << "Enter message: " << std::flush;
	}
}

void StartSendLoop(TCPSocket& connection)
{
	std::string message;
	while (true)
	{
		{
			std::lock_guard<std::mutex> lock(consoleMutex);
			std::cout << "Enter message: " << std::flush;
		}

		std::getline(std::cin, message);
		if (message.empty())
			continue;

		if (message == "exit")
			break;

		uint32_t size = htonl(static_cast<uint32_t>(message.size()));
		if (connection.sendAll(&size, sizeof(uint32_t)) != PResult::P_SUCCESS)
			break;

		if (connection.sendAll(message.data(), message.size()) != PResult::P_SUCCESS)
			break;
	}
}

void TCPServer()
{
	std::cout << "Winsock API initialized successfully." << std::endl;

	TCPSocket socket;
	if (socket.create() == PResult::P_SUCCESS)
	{
		std::cout << "Socket created successfully." << std::endl;
		if (socket.listen(IPEndpoint("0.0.0.0", 8080)) == PResult::P_SUCCESS)
		{
			std::cout << "Socket is successfully listening on port 8080." << std::endl;
			TCPSocket newConnection;
			if (socket.accept(newConnection) == PResult::P_SUCCESS)
			{
				std::cout << std::string(20, '-') << std::endl;
				std::cout << "Client connected." << std::endl;

				std::thread receiverThread(StartReceiveLoop, std::ref(newConnection));
				StartSendLoop(newConnection);

				newConnection.close();
				receiverThread.join();
			}
			else
			{
				std::cerr << "Failed to accept a connection." << std::endl;
			}
		}
		else
		{
			std::cerr << "Failed to listen on port 8080." << std::endl;
		}
		socket.close();
	}
	else
	{
		std::cerr << "Failed to create socket." << std::endl;
	}
}

// ------------- UDP --------------------------------

void UDPServer()
{
	std::cout << "Winsock API initialized successfully." << std::endl;

	UDPSocket socket;
	if (socket.create() == PResult::P_SUCCESS)
	{
		std::cout << "Socket created successfully." << std::endl;

		if(socket.bind(IPEndpoint("0.0.0.0", 8080)) == PResult::P_SUCCESS)
		{
			std::string buffer = "";
			uint32_t bufferSize = 10;
			while (true)
			{
				IPEndpoint incommingIPEndpoint;
				int bytesReceived = 0;
				buffer.resize(bufferSize);
				int result = socket.receiveFrom(&buffer[0], bufferSize, incommingIPEndpoint, bytesReceived);
				if (result != PResult::P_SUCCESS)
				{
					break;
				}
				std::cout << "[" << bufferSize << "] - " << buffer << std::endl;
			}
		}
		socket.close();
	}
	else
	{
		std::cerr << "Failed to create socket." << std::endl;
	}
}