#pragma once
#include <iostream>
#include "Client.h"
#include <fstream>
#include "Email.h"
#include <json/json.h>

class fs{
public:
	static void rmdirAndChildFile(std::string directoryName);
	static bool isCheckFolderExists(std::string path);
	static void delFile(std::string fileName);
	static void createFolder(std::string);
	bool fileExists(const std::string& filename);
	static void writeFileMainConfig(std::string path,std::string username, std::string pass,std::string fullname, int autoload, int smtpPort, int pop3Port, std::string mailsever,int);
	Json::Value readConfig(std::string path);
	void writeEmailToFolder(string path, Json::Value& event);
	vector<Email> readEmailToFolder(string path);
};

