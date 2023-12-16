#pragma once
#include <iostream>
#include "fileStream.h"
#include <string>
#include "POP3_command.h"
#include "MailSending.h"
#include "json/json.h"


class User {
private:
	std::string username;
	std::string passwork;
	std::string mailsever;
	std::string fullname;
	std::string path;
	int autoload;
	int SMTPPort;
	int POP3Port;
	int totalEmail;
	bool isLogin = true;
	void writeConfig();
	POP3_command* pop3;
	std::vector<Email> emails;
	std::vector<Email> emailsCurent;
	std::vector<int> indexEmailsCurent;
	void getConfig();
public:
	int statusLogout = -1;
	int statusExit = -1;
	User();
	User(std::string user,std::string pass,std::string fullname, int autoload, int smtp, int pop3,std::string mailsever, int total);
	~User();
	bool login(CSocket& ClientSocket);
	void communicate(CSocket& ClientSocket);
	void logout();
	int inMailListFolder(std::string folder);
	void getListFolder();
	void saveDB();
	void getDB();
	void pushEmails(CSocket& ClientSocket);
	int getAutoload();
	int getTotal() {
		return this->emails.size();
	}
	void setPath(std::string path);
	std::string getPath();
};