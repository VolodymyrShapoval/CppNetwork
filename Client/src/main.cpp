#include <iostream>
#include "IncludeFile.h"

using namespace CNet;

int main()
{
	if (Network::initialize())
	{
		std::cout << "Winsock API initialized successfully." << std::endl;
	}
	Network::shutdown();
	return 0;
}