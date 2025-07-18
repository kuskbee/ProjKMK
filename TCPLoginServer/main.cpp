#include <iostream>

#include "server.h"

#pragma comment(lib, "ws2_32")
#pragma comment(lib, "mysqlcppconn.lib")

int main()
{
	Server LoginServer;

	bool bSuccess = LoginServer.Initialize();
	if (false == bSuccess)
	{
		std::cerr << "Failed to Initialize LoginServer." << std::endl;
		return 1;
	}

	LoginServer.Run();
	LoginServer.Close();

	return 0;
}