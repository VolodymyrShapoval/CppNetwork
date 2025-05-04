#pragma once

namespace CNet
{
	enum SocketOption
	{
		TCP_NO_DELAY, // True to disable Nagle's algorithm
		UDP_SO_BROADCAST,
		UDP_SO_REUSEADDR
	};
}