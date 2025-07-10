#include "DatabaseManager.h"

//MySQL
#include <jdbc/mysql_connection.h>
#include <jdbc/cppconn/exception.h>
#include <jdbc/cppconn/prepared_statement.h>

//Ini
#include "../Utils/IniParser.h"
#include <sstream>

bool DatabaseManager::Initialize(std::string& ConfigPath)
{
	// get database config
	std::string Section = "Database";
	auto DbConfig = util::IniParser::ParseSection(ConfigPath, Section);

	std::string Host = DbConfig["Host"];
	std::string Port = DbConfig["Port"];
	DBUser = DbConfig["User"];
	DBPass = DbConfig["Password"];
	Schema = DbConfig["Schema"];

	std::stringstream Url;
	Url << "tcp://" << Host << ":" << Port
		<< "?useUnicode=true&characterEncoding=UTF-8";
	DBUrl = Url.str();

	try
	{
		SqlDriver = get_driver_instance();
	}
	catch (sql::SQLException& e)
	{
		std::cerr << "Could not connect to database. Error: " << e.what() << std::endl;
		return false;
	}

	std::cout << "Database connection successful!" << std::endl;
	return true;
}

sql::Connection* DatabaseManager::GetConnection()
{
	// 스레드-로컬 저장소에 한 번만 생성되는 unique_ptr
	thread_local std::unique_ptr<sql::Connection> conn;
	if (!conn)
	{
		try
		{
			conn.reset(SqlDriver->connect(DBUrl, DBUser, DBPass));
			conn->setSchema(Schema);
		}
		catch (sql::SQLException& e)
		{
			std::cerr << "DB connect error in thread: " << e.what() << "\n";
			return nullptr;
		}
	}
	return conn.get();
}

SignUpStatus DatabaseManager::SignUp(sql::Connection* Conn, const std::string& UserId, const std::string& Password, const std::string& Nickname)
{
	SignUpStatus Status = SignUpStatus::OtherError;

	try
	{
		// 1. ID 중복 체크
		std::unique_ptr<sql::PreparedStatement> pstmt(Conn->prepareStatement(
			"SELECT id FROM users WHERE userid = ?"
		));
		pstmt->setString(1, UserId); // 클라이언트로부터 받은 ID
		std::unique_ptr <sql::ResultSet> res(pstmt->executeQuery());

		if (res->next())
		{
			// 이미 존재하는 ID. 
			std::cout << "UserId[" << UserId << "] already exists." << std::endl;

			Status = SignUpStatus::DuplicateUserId;
		}
		else
		{
			// 2. 닉네임 중복 체크
			pstmt.reset(Conn->prepareStatement(
				"SELECT id FROM users WHERE userid = ?"
			));
			pstmt->setString(1, Nickname); // 클라이언트로부터 받은 닉네임
			std::unique_ptr <sql::ResultSet> res(pstmt->executeQuery());

			if (res->next())
			{
				// 이미 존재하는 닉네임. 
				std::cout << "UserNick[" << Nickname << "] already exists." << std::endl;

				Status = SignUpStatus::DuplicateNickname;
			}
			else
			{
				// 3. ID, 닉네임 중복이 아니면 INSERT
				pstmt.reset(Conn->prepareStatement(
					"INSERT INTO users(userid, password, nickname) VALUES(?, ?, ?)"
				));
				pstmt->setString(1, UserId);
				pstmt->setString(2, Password);
				pstmt->setString(3, Nickname); // 닉네임도 받아야 함
				pstmt->executeUpdate();

				std::cout << "New user created." << std::endl;

				Status = SignUpStatus::Success;
			}
		}
	}
	catch (sql::SQLException& e)
	{
		std::cerr << "SQL Error during authentication: " << e.what() << std::endl;
	}

	return Status;
}

LoginStatus DatabaseManager::Login(sql::Connection* Conn, const std::string& UserId, const std::string& Password,
							int& outPlayerId, std::string& outNickname)
{
	LoginStatus Status = LoginStatus::OtherError;
	outPlayerId = 0;
	outNickname = "";

	try
	{
		std::unique_ptr<sql::PreparedStatement> pstmt(Conn->prepareStatement(
			"SELECT id, password, nickname FROM users WHERE userid = ?"
		));
		pstmt->setString(1, UserId);

		std::unique_ptr <sql::ResultSet> res(pstmt->executeQuery());

		if (res->next())
		{
			std::string storedPassword = res->getString("password");
			if (storedPassword == Password) // :EX: 해싱된 비밀번호 비교
			{ 
				outPlayerId = res->getInt("id");
				outNickname = res->getString("nickname");

				Status = LoginStatus::Success;
			}
			else
			{
				Status = LoginStatus::InvalidPassword;
			}
		}
		else
		{
			Status = LoginStatus::UserIdNotFound;
		}
	}
	catch (sql::SQLException& e)
	{
		std::cerr << "SQL Error during authentication: " << e.what() << std::endl;
	}

	return Status;
}
