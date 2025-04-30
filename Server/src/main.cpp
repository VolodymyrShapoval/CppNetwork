#include <iostream>
#include "Network.h"

using namespace CNet;

int main()
{
	if (Network::Initialize())
	{
		std::cout << "Winsock API initialized successfully." << std::endl;
	}
	Network::Shutdown();
	system("pause");
	return 0;
}