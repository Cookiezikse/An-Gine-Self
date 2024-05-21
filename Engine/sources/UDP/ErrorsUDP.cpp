#include "UDP/ErrorsUDP.h"
#include <iostream>

namespace Bousk
{
	namespace Network
	{
		namespace ErrorsUDP
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
}