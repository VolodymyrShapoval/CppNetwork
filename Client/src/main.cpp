#include <iostream>
#include "IncludeFile.h"

using namespace CNet;

int main()
{
	if (Network::Initialize())
	{
		std::cout << "Winsock API initialized successfully." << std::endl;
	}
	Network::Shutdown();
	return 0;
}