#include "TCP/Errors.h"
#include <iostream>

namespace Network
{
	namespace Errors
	{
		int Get()
		{
			//std::cout << "function get" << std::endl;
#ifdef _WIN32
			return WSAGetLastError();
#else
			return errno;
#endif
		}
	}
}