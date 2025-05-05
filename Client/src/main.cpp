#include <iostream>
#include "IncludeFile.h"
#include "Constants.h"
#include <UDPSocket.h>
#include <thread>
#include <mutex>

using namespace CNet;

std::mutex consoleMutex;

void TCPClient();
void UDPClient();
void UDPClient(const std::string& remoteIP, uint16_t remotePort, uint16_t localPort);

int main()
{
	if (Network::initialize())
	{
		// UDP Presenting
		uint16_t remotePort, localPort;
		std::cout << "Enter remote port: ";
		std::cin >> remotePort;

		std::cout << "Enter your local port: ";
		std::cin >> localPort;
		std::cin.ignore(); // очищення '\n'

		std::cout << std::string(20, '-') << std::endl;
		UDPClient("127.0.0.1", remotePort, localPort);
	}
	Network::shutdown();
	system("pause");
	return 0;
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

			std::string buffer = "";
			while (true)
			{
				std::cout << "Enter message: ";
				std::getline(std::cin, buffer);
				uint32_t bufferSize = buffer.size();
				bufferSize = htonl(bufferSize);
				int result = socket.sendAll(&bufferSize, sizeof(uint32_t));
				if (result != PResult::P_SUCCESS)
				{
					break;
				}

				result = socket.sendAll(buffer.data(), buffer.size());

				if (result != PResult::P_SUCCESS)
				{
					break;
				}
				std::cout << "Attempting to send data..." << std::endl;
				Sleep(500);
			}
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

void UDPClient()
{
	std::cout << "Winsock API initialized successfully." << std::endl;
	UDPSocket socket;
	if (socket.create() == PResult::P_SUCCESS)
	{
		std::cout << "Socket created successfully." << std::endl;

		IPEndpoint serverEndpoint("127.0.0.1", 8080);

		std::string buffer = "";
		while (true)
		{
			std::cout << "Enter message: ";
			std::getline(std::cin, buffer);

			if (buffer.empty())
				continue;

			int bytesSent = 0;
			PResult result = socket.sendTo(buffer.data(), buffer.size(), serverEndpoint, bytesSent);
			if (result != PResult::P_SUCCESS)
			{
				break;
			}

			std::cout << "Attempting to send data..." << std::endl;
			Sleep(500);
		}

		socket.close();
	}
	else
	{
		std::cerr << "Failed to create socket." << std::endl;
	}
}

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
		std::cout << "\r\33[K"; // Очистити поточний рядок
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
	receiverThread.detach(); // або .join(), якщо хочеш чекати завершення
}