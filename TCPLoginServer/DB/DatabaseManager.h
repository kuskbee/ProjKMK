#pragma once

#include <string>
#include <jdbc/cppconn/driver.h>

#include "DBQueryStatus.h"

struct ConnDeleter {
	void operator()(sql::Connection* p) const {
		p->close();
		delete p;
	}
};

class DatabaseManager
{
public:
	bool Initialize(std::string& ConfigPath);
	
	sql::Connection* GetConnection();

	// Query Function
	SignUpStatus SignUp(sql::Connection* Conn,
				const std::string& UserId, 
				const std::string& Password, 
				const std::string& Nickname);

	LoginStatus Login(sql::Connection* Conn,
				const std::string& UserId,
				const std::string& Password,
				int& outPlayerId,
				std::string& outNickname
				);

private:
	sql::Driver* SqlDriver;
	
	std::string DBUser;
	std::string DBPass;
	std::string DBUrl;
	std::string Schema;
};