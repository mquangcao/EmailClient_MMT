#define _CRT_SECURE_NO_WARNINGS
#define warrning (disable:4996)
#include "stdafx.h"
#include "Client.h"
#include "afxsock.h"
#include <string>
#include<iostream>
#include <fstream>
#include "Poco/Base64Encoder.h"
#include"Poco/StreamCopier.h"
#include <sstream>
#include <iomanip>
#include"random"
#define _AFXDLL




using namespace std;
//void encodePDF(const std::string& inputFilePath, const std::string& outputFilePath);
void MailSending(CSocket& ClientSocket, string& userName, string& user);
void EncodeFileToBase64(const std::string& filePath, std::string& encodedContent);
std::uintmax_t getFileSize(const std::string& filePath);
std::string generateRandomAlphabeticString(int length);
string extractFileName(const std::string& fullPath);
void  send_mail_with_oneFile(CSocket& clientSocket,string&filePath);
void mailsendingwithAttachfile(CSocket& ClientSocket, string& userName, string& user, int smtpPort);


	