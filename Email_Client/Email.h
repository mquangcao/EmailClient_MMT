#pragma once
#include <string>
#include <iostream>
#include <vector>
#include <sstream>
#include <fstream>
#include <Poco/StreamCopier.h>
#include <Poco/Base64Encoder.h>
#include <Poco/Base64Decoder.h>
#include <Poco/FileStream.h>
#include <json/json.h>
#include <fstream>

using std::string;
using std::vector;

enum status {
	UNREAD, READED
};

class Email {
private:
	
	status state;
	string subject;
	string content;
	string mailFrom;
	string userFrom;
	string folder;
	string boundary;
	vector<string> cc_list;
	vector<std::string> file_attached;
	vector<std::string> fileName_attached;
	string getIn(string& email,string begin, string end);
	string getMailFrom(string& email);
	string getUserFrom(string& email);
	string getBoundary(string& email);
	string getSubject(string& email);
	string getContent(string& email);
	string emailFilter();
	string getFileName(string & email);
	string getFileEncode(string& email);
	vector<string> split(const std::string& s, const std::string& delimiter);
	vector<string> getCc(string& email);
	void getFile(string& email);
	int countAtSymbols(const std::string& str);
	void decodeFile(const std::string& encodedString, const std::string& outputFile);
public:
	Email(string& email);
	Email(int state, string subject, string content, string mailFrom, string userFrom, string folder, string boundary, vector<string> cc_list, vector<std::string> file_attached, vector<std::string> fileName_attached);
	string getFolder();
	string formatEmail();
	string formatTitileList();
	void getFile();
	Json::Value handleJsonEmail(int index);
};