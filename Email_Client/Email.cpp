#include "Email.h"

string Email::getFolder() {
	return this->folder;
}

Email::Email(int state, string subject, string content, string mailFrom, string userFrom, string folder, string boundary, vector<string> cc_list, vector<std::string> file_attached, vector<std::string> fileName_attached)
{
	this->boundary = boundary;
	this->state = state == 1 ? READED : UNREAD;
	this->subject = subject;
	this->content = content;
	this->mailFrom = mailFrom;
	this->userFrom = userFrom;
	this->folder = folder;
	this->cc_list = cc_list;
	this->file_attached = file_attached;
	this->fileName_attached = fileName_attached;
}

vector<string> Email::getCc(string& email) {
	vector<string> token;
	string cc = this->getIn(email, "\r\nCc:", "\r\n");
	if (cc == "") return token;
	token = this->split(cc, ", ");
	if (token.size() == 1 && token[0] == cc) {
		if (this->countAtSymbols(cc) > 1) {
			token = this->split(cc, " ");
		}
	}
	return token;
}

int Email::countAtSymbols(const std::string& str) {
	int atCount = 0;

	for (char ch : str) {
		if (ch == '@') {
			++atCount;
		}
	}

	return atCount;
}


string Email::formatEmail() {
	this->state = READED;
	string email = "";
	email += "===============================================\n";
	email += ("From: "  + this->userFrom  + " - " + this->mailFrom + "\n");
	if (this->cc_list.size() <= 0) {
		email += "Cc: ";
		for (int i = 0; i < this->cc_list.size(); ++i) {
			email += (this->cc_list[i]);
			if (i != this->cc_list.size() - 1) email += ", ";
		}
		email += "\r\n";
	}
	email += ("Subject : " + this->subject + "\r\n\r\n");
	email += ("Content : " + this->content + "\n");
	email += "===============================================\n\n";
	return email;
}

string Email::formatTitileList() {
	string email = "";
	if (this->state == UNREAD) email += "(Chua doc) ";
	 email += ("<" + this->userFrom + " - " + this->mailFrom + "> <" + this->subject + ">\n");
	return email;
}


string Email::getIn(string& email, string begin, string end) {
	size_t i = email.find(begin);
	if (i == string::npos) return "";
	size_t length = begin.length();
	size_t j = email.find(end, i + length);
	return email.substr(i + length, j - i - length);
}

string Email::getMailFrom(string& email) {
	size_t i = email.find("From: ");
	if (i == string::npos) return "";
	size_t j = email.find("<", i + 1);
	size_t k = email.find(">", j + 1);
	return email.substr(j + 1, k - j - 1);
}

string Email::getUserFrom(string& email) {
	size_t i = email.find("From: ");
	if (i == string::npos) return "";
	size_t j = email.find("<", i + 1);
	return email.substr(i + 6, j - i - 7);
}

string Email::getBoundary(string& email) {
	return this->getIn(email, "boundary=\"", "\"\r\n");
}

string Email::getSubject(string& email) {
	return this->getIn(email, "Subject: ", "\r\n");
}

string Email::getContent(string& email) {
	size_t i = email.find("Content-Type: text/plain;");
	if (i == string::npos) return "";
	size_t j = email.find("\r\n\r\n", i + 1);
	size_t k = email.find("\r\n", j + 4);
	return email.substr(j + 4, k - j - 4);
}

string Email::emailFilter() {
	if (this->mailFrom == "ahihi@testing.com" || this->mailFrom == "ahuu@testing.com")
		return "Project";
	if (this->subject.find("urgent") != string::npos || this->subject.find("ASAP") != string::npos)
		return "Important";
	if (this->content.find("report") != string::npos || this->content.find("meeting") != string::npos)
		return "Work";
	if (this->subject.find("virus") != string::npos || this->subject.find("hack") != string::npos ||
		this->subject.find("crack") != string::npos || this->content.find("virus") != string::npos ||
		this->content.find("hack") != string::npos || this->content.find("crack") != string::npos)
		return "Spam";
	return "Inbox";
}

vector<string> Email::split(const std::string& s, const std::string& delimiter) {
	std::vector<std::string> tokens;
	size_t start = 0, end = 0;

	while ((end = s.find(delimiter, start)) != std::string::npos) {
		std::string token = s.substr(start, end - start);
		tokens.push_back(token);
		start = end + delimiter.length();
	}

	std::string token = s.substr(start);
	tokens.push_back(token);

	return tokens;
}

void Email::getFile(string& email) {
	if (this->boundary != "") {
		vector<string> token = this->split(email, "--" + this->boundary);
		for (int i = 0; i < token.size(); i++) {
			if (token[i].find("base64") != string::npos) {
			fileName_attached.push_back(this->getFileName(token[i]));
			file_attached.push_back(this->getFileEncode(token[i]));
			}
		}
	}
}


string Email::getFileName(string& email) {
	return this->getIn(email, "filename=\"", "\"\r\n");
}

string Email::getFileEncode(string& email) {
	return this->getIn(email, "base64\r\n\r\n", "\r\n\r\n");
}

Email::Email(string& email) {
	this->boundary = this->getBoundary(email);
	this->userFrom = this->getUserFrom(email);
	this->mailFrom = this->getMailFrom(email);
	this->state = UNREAD;
	this->subject = this->getSubject(email);
	this->content = this->getContent(email);
	this->getFile(email);
	this->cc_list = this->getCc(email);
	this->folder = this->emailFilter();
}

void Email::decodeFile(const std::string& encodedString, const std::string& outputFile) {
	// Tao doi tuong base64 de giai ma
	std::istringstream input(encodedString);
	Poco::Base64Decoder decoder(input);
	
	// Mo file dau ra de ghi du lieu ma
	Poco::FileOutputStream output(outputFile, std::ios::binary);
	if (!output.good()) {
		std::cerr << "Error opening output file: " << outputFile << std::endl;
		return;
	}
	Poco::StreamCopier::copyStream(decoder, output);
	output.close();
}

void Email::getFile() {
	if (this->boundary != "") {
		std::cout << "Email nay co dinh kem file, ban co muon tai khong ( neu khong nhap gi file se duoc luu o ../File) [Y/N] : ";
		char select;
		std::string outputPath = "";
		std::cin >> select;
		bool isOn = true;
		while (isOn) {
			switch (select) {
			case 'Y':
			case 'y':
				std::cout << "Nhap vao duong dan noi muon luu: ";
				std::cin.ignore();
				std::getline(std::cin, outputPath);
				if (outputPath.empty()) outputPath = "../File/";
				for (int i = 0; i < this->fileName_attached.size(); i++) {	
					this->decodeFile(this->file_attached[i], outputPath + this->fileName_attached[i]);
				}
				isOn = false;
				break;
			case 'n':
			case 'N':
				isOn = false;
				return;
			default:
				std::cout << "Vui long nhap lai: ";
				std::cin >> select;
			}
		}
	}
}



Json::Value Email::handleJsonEmail(int index) {
#include<json/writer.h>
	Json::Value event;
	event[std::to_string(index)]["state"] = this->state;
	event[std::to_string(index)]["subject"] = this->subject;
	event[std::to_string(index)]["content"] = this->content;
	event[std::to_string(index)]["mailFrom"] = this->mailFrom;
	event[std::to_string(index)]["userFrom"] = this->userFrom;
	event[std::to_string(index)]["folder"] = this->folder;
	event[std::to_string(index)]["boundary"] = this->boundary;
	Json::Value ccListJson(Json::arrayValue);
	for (const auto& cc : this->cc_list) {
		ccListJson.append(cc);
	}
	event[std::to_string(index)]["cc_list"] = ccListJson;

	Json::Value file_attachedJson(Json::arrayValue);
	for (const auto& cc : this->file_attached) {
		file_attachedJson.append(cc);
	}
	event[std::to_string(index)]["file_attached"] = file_attachedJson;

	Json::Value fileName_attachedJson(Json::arrayValue);
	for (const auto& cc : this->fileName_attached) {
		fileName_attachedJson.append(cc);
	}
	event[std::to_string(index)]["fileName_attached"] = fileName_attachedJson;

	return event;
}