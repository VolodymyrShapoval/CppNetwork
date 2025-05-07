#include <iostream>
#include "IncludeFile.h"
#include "Constants.h"
#include <UDPSocket.h>
#include <thread>
#include <mutex>
#include <chrono>
#include <cmath>

using namespace CNet;

std::mutex consoleMutex;

void TCPClient();
PResult TCPSendWithBenchmarking(TCPSocket& socket, const uint64_t messageSize);
void TCPClientWithBenchmarking();
//void UDPClient();
void UDPClient(const std::string& remoteIP, uint16_t remotePort, uint16_t localPort);

int main()
{
	if (Network::initialize())
	{
		//// UDP Presenting
		//uint16_t remotePort, localPort;
		//std::cout << "Enter remote port: ";
		//std::cin >> remotePort;

		//std::cout << "Enter your local port: ";
		//std::cin >> localPort;
		//std::cin.ignore();

		//std::cout << std::string(20, '-') << std::endl;
		//UDPClient("127.0.0.1", remotePort, localPort);

		TCPClientWithBenchmarking();
	}
	Network::shutdown();
	system("pause");
	return 0;
}

// ------------- TCP with Benchmarking -------------------
PResult TCPSendWithBenchmarking(TCPSocket& socket, const uint64_t messageSize, uint64_t& bytesSent)
{
	std::string message(messageSize, 'a');
	if (message.empty())
	{
		return PResult::P_GENERIC_ERROR;
	}

	uint64_t bufferSize = htonll(static_cast<uint64_t>(message.size()));
	if (socket.sendAll(&bufferSize, sizeof(uint64_t)) != PResult::P_SUCCESS)
	{
		return PResult::P_GENERIC_ERROR;
	}
	bytesSent += sizeof(uint64_t);

	if (socket.sendAll(message.data(), message.size()) != PResult::P_SUCCESS)
	{
		return PResult::P_GENERIC_ERROR;
	}

	bytesSent += messageSize;
	return PResult::P_SUCCESS;
}

void TCPClientWithBenchmarking()
{
	std::cout << "Winsock API initialized successfully." << std::endl;
	TCPSocket socket;
	if (socket.create() == PResult::P_SUCCESS)
	{
		std::cout << "Socket created successfully." << std::endl;
		if (socket.connect(IPEndpoint("192.168.0.104", 8080)) == PResult::P_SUCCESS)
		{
			std::cout << "Socket connected successfully." << std::endl;
			uint64_t messageSize = 80 * KB;
			uint64_t bytesSent = 0;
			auto startTime = std::chrono::high_resolution_clock::now();
			for (int i = 0; i < 10024; ++i)
			{
				if (TCPSendWithBenchmarking(socket, messageSize, bytesSent) != PResult::P_SUCCESS)
				{
					std::cerr << "Failed to send message." << std::endl;
					return;
				}
			}
			auto endTime = std::chrono::high_resolution_clock::now();
			std::cout << "Message sent successfully." << std::endl;
			std::chrono::duration<double> elapsedTime = std::chrono::duration_cast<std::chrono::duration<double>>(endTime - startTime);
			double speedMBps = (static_cast<double>(bytesSent) / static_cast<double>(MB)) / elapsedTime.count();
			std::cout << "Benchmark: Speed: " << speedMBps << "MB/sec." << std::endl;
			socket.close();
		}
		else
		{
			std::cerr << "Failed to connect to the server." << std::endl;
		}
		socket.close();
	}
	else
	{
		std::cerr << "Failed to create socket." << std::endl;
	}
}

// ------------- TCP --------------------------------

void TCPStartReceiveLoop(TCPSocket& socket)
{
	std::string buffer;
	while (true)
	{
		uint32_t bufferSize = 0;
		int result = socket.receiveAll(&bufferSize, sizeof(uint32_t));
		if (result != PResult::P_SUCCESS)
			break;

		bufferSize = ntohl(bufferSize);
		if (bufferSize > g_MAX_PACKET_SIZE)
			break;

		buffer.resize(bufferSize);
		result = socket.receiveAll(&buffer[0], bufferSize);
		if (result != PResult::P_SUCCESS)
			break;

		std::lock_guard<std::mutex> lock(consoleMutex);
		std::cout << "\r\33[K";
		std::cout << "[Server] " << buffer << std::endl;
		std::cout << "Enter message: " << std::flush;
	}
}

void TCPStartSendLoop(TCPSocket& socket)
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

		uint32_t bufferSize = htonl(static_cast<uint32_t>(message.size()));
		if (socket.sendAll(&bufferSize, sizeof(uint32_t)) != PResult::P_SUCCESS)
			break;

		if (socket.sendAll(message.data(), message.size()) != PResult::P_SUCCESS)
			break;
	}
}

void TCPClient()
{
	std::cout << "Winsock API initialized successfully." << std::endl;
	TCPSocket socket;
	if (socket.create() == PResult::P_SUCCESS)
	{
		std::cout << "Socket created successfully." << std::endl;
		if (socket.connect(IPEndpoint("127.0.0.1", 8080)) == PResult::P_SUCCESS)
		{
			std::cout << "Socket connected successfully." << std::endl;

			std::thread receiverThread(TCPStartReceiveLoop, std::ref(socket));
			TCPStartSendLoop(socket);
			socket.close();
			receiverThread.join();
		}
		else
		{
			std::cerr << "Failed to connect to the server." << std::endl;
		}
		socket.close();
	}
	else
	{
		std::cerr << "Failed to create socket." << std::endl;
	}
}

// ------------- UDP --------------------------------

//void UDPClient()
//{
//	std::cout << "Winsock API initialized successfully." << std::endl;
//	UDPSocket socket;
//	if (socket.create() == PResult::P_SUCCESS)
//	{
//		std::cout << "Socket created successfully." << std::endl;
//
//		IPEndpoint serverEndpoint("127.0.0.1", 8080);
//
//		std::string buffer = "";
//		while (true)
//		{
//			std::cout << "Enter message: ";
//			std::getline(std::cin, buffer);
//
//			if (buffer.empty())
//				continue;
//
//			int bytesSent = 0;
//			PResult result = socket.sendTo(buffer.data(), buffer.size(), serverEndpoint, bytesSent);
//			if (result != PResult::P_SUCCESS)
//			{
//				break;
//			}
//
//			std::cout << "Attempting to send data..." << std::endl;
//			Sleep(500);
//		}
//
//		socket.close();
//	}
//	else
//	{
//		std::cerr << "Failed to create socket." << std::endl;
//	}
//}

void StartReceiveLoop(UDPSocket& socket)
{
	std::string buffer(1024, '\0');
	while (true)
	{
		IPEndpoint sender;
		int bytesReceived = 0;
		PResult result = socket.receiveFrom(&buffer[0], buffer.size(), sender, bytesReceived);
		if (result != PResult::P_SUCCESS)
			break;

		std::lock_guard<std::mutex> lock(consoleMutex);
		std::cout << "\r\33[K";
		std::cout << "[From " << sender.getIPString() << ":" << sender.getPort() << "] "
			<< std::string(buffer.begin(), buffer.begin() + bytesReceived) << std::endl;
		std::cout << "Enter message: " << std::flush;
	}
}

void StartSendLoop(UDPSocket& socket, IPEndpoint& remoteEndpoint)
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

		int bytesSent = 0;
		PResult result = socket.sendTo(message.data(), message.size(), remoteEndpoint, bytesSent);
		if (result != PResult::P_SUCCESS)
		{
			std::lock_guard<std::mutex> lock(consoleMutex);
			std::cerr << "\nFailed to send message." << std::endl;
			break;
		}
	}
}

void UDPClient(const std::string& remoteIP, uint16_t remotePort, uint16_t localPort)
{
	std::cout << "Winsock API initialized successfully." << std::endl;
	UDPSocket socket;

	if (socket.create() != PResult::P_SUCCESS)
	{
		std::cerr << "Failed to create socket." << std::endl;
		return;
	}

	if (socket.bind(IPEndpoint("0.0.0.0", localPort)) != PResult::P_SUCCESS)
	{
		std::cerr << "Failed to bind socket to port " << localPort << std::endl;
		socket.close();
		return;
	}

	IPEndpoint remoteEndpoint(remoteIP.c_str(), remotePort);

	std::thread receiverThread(StartReceiveLoop, std::ref(socket));
	StartSendLoop(socket, remoteEndpoint);

	socket.close();
	receiverThread.detach();
}