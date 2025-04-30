#include <iostream>
#include "Network.h"

using namespace CNet;

int main()
{
	if (Network::initialize())
	{
		std::cout << "Winsock API initialized successfully." << std::endl;
	}
	Network::shutdown();
	system("pause");
	return 0;
}