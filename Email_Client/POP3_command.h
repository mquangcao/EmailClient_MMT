#pragma once
#include <string>
#include "Client.h"
#include "fileStream.h"
#include "Email.h"
class POP3_command {
private:
	std::string username;
	std::string passwork;
	std::string fullname;
	int totalEmail;
	int port;
	enum typeSend {
		USER,
		PASS,
		STAT,
		LIST,
		RETR,
		QUIT
	};
	bool sendToSeverMsg(CSocket& , typeSend , std::string);
	bool isCheckState(std::string);
	bool isCheckStateLogin(char[]);
	bool sendToSever(CSocket& , typeSend );
	bool handleDowMail(CSocket& ClientSocket, typeSend type);
	std::string handleStatSyntax(char []);
	int statSyntax(CSocket& ClientSocket,typeSend);
	std::string RETREmail(CSocket& ClientSocket, typeSend,int);
	vector<Email> RETREmails(CSocket& ClientSocket, typeSend, const int);
	std::string handleSeverMsg(char*);
public:
	POP3_command(std::string user, std::string pass,std::string fullname, int total, int port);
	vector<Email> dowMail(CSocket& ClientSocket);
	bool checkLogin(CSocket& ClientSocket);
};