#include "POP3_command.h"
using namespace std;

namespace suport {
	int spacePosition(std::string msg, const int start, const int end) {
		for (int i = start; i < end; i++) {
			if (msg[i] == ' ') return i;
		}
		return -1;
	}

	int endlPosition(std::string msg) {
		for (int i = 0; i < msg.length(); i++) {
			if (msg[i] == '\n') 
			{
				return i;
			}
		}
	}
}

std::string POP3_command::handleSeverMsg(char severMsg[]) {
	size_t pos = 0;
	std::string serverResponse(severMsg);
	if ((pos = serverResponse.find("\r\n")) != std::string::npos) {
		serverResponse.erase(0, pos + 2);
		serverResponse.erase(serverResponse.find("\r\n.") + 3);
	}
	return serverResponse;
}

bool POP3_command::isCheckState(std::string severMsg) {
	if (severMsg.find("+OK") == string::npos) return false;
	return true;
}

bool POP3_command::isCheckStateLogin(char severMsg[]) {
	string msg = string(severMsg);
	if (msg.find("+OK") == string::npos) return false;
	return true;
}

bool POP3_command::sendToSeverMsg(CSocket& ClientSocket, typeSend type, std::string additionalMsg) {
	std::string msg = "";
	switch (type) {
	case USER:
		msg += "USER ";
		break;
	case PASS:
		msg += "PASS ";
		break;
	default:
		return false;
	}
	msg = msg + additionalMsg + "\r\n";
	ClientSocket.Send(msg.c_str(), msg.length(), 0);

	char SeverMsg[1000];
	ClientSocket.Receive(SeverMsg, 1000, 0);
	if (!(this->isCheckStateLogin(SeverMsg))) {
		return false;
	}
	return true;
}

bool POP3_command::sendToSever(CSocket& ClientSocket, typeSend type) {
	string msg = "";
	switch (type) {
	case LIST:
		msg += "LIST\r\n";
		break;
	case QUIT:
		msg += "QUIT\r\n";
		break;
	}
	ClientSocket.Send(msg.c_str(), msg.length(), 0);
	char SeverMsg[1000];
	ClientSocket.Receive(SeverMsg, 1000, 0);
	if (!(this->isCheckStateLogin(SeverMsg))) {
		std::cout << SeverMsg << "\n";
		return false;
	}
	return true;
}


int POP3_command::statSyntax(CSocket& ClientSocket, typeSend type = STAT) {
	if (type == STAT) {
	std::string msg = "STAT\r\n";
	ClientSocket.Send(msg.c_str(), msg.length(), 0);
	char SeverMsg[1000];
	ClientSocket.Receive(SeverMsg, 1000, 0);
	std::string numOfEmail = this->handleStatSyntax(SeverMsg);
	return std::stoi(numOfEmail);
	}
	return -1;
}

std::string POP3_command::handleStatSyntax(char msg[]) {
	string severMsg(msg);
	int iS1 = severMsg.find("+OK");
	iS1 += 4;
	size_t iS2 = suport::spacePosition(severMsg, iS1, severMsg.length());
	std::string ans = severMsg.substr(iS1, iS2 - iS1);
	return ans;
}

bool POP3_command::handleDowMail(CSocket& ClientSocket, typeSend type) {
	std::string content = "";
	switch (type) {
	case USER:
		content += this->username;
		break;
	case PASS:
		content += this->passwork;
		break;
	default:
		return false;
	}
	bool isSend = this->sendToSeverMsg(ClientSocket, type, content);
	if (!isSend) {
		std::cout << "Co loi xay ra.\n";
		return false;
	}
	return true;
}

std::string POP3_command::RETREmail(CSocket& ClientSocket, typeSend type, int index) {
	if (type == RETR) {
		std::string msgClient = "RETR ";
		msgClient = msgClient + std::to_string(index) + "\r\n";
		ClientSocket.Send(msgClient.c_str(), msgClient.length(), 0);
		char SeverMsg[4097];
		std::string msg = "";
		while (true) {
			ClientSocket.Receive(SeverMsg, 4096, 0);
			SeverMsg[4096] = '\0';
			std::string tmp(SeverMsg);
			int pos;
			if ((pos = tmp.find("\r\n.\r\n")) != string::npos) {
				tmp.erase(pos + 5);
				msg += tmp;
				break;
			}
			msg += tmp;
		}
		int pos;
		if ((pos = msg.find("\r\n")) != std::string::npos) {
			msg.erase(0, pos + 2);
		}
		return msg;
	}
	return "";
}

vector<Email> POP3_command::RETREmails(CSocket& ClientSocket, typeSend type, const int numOfEmails) {
	vector<Email> token;
	if (type == RETR) {
		for (int i = this->totalEmail + 1; i <= numOfEmails; i++) {
			string e = this->RETREmail(ClientSocket, RETR, i);
			Email email(e);
			token.push_back(email);
		}
	}
	return token;
}

vector<Email> POP3_command::dowMail(CSocket& ClientSocket) {
	vector<Email> token;
	ClientSocket.Create();
	if (ClientSocket.Connect(_T(LOOPBACK_IP), this->port) != 0) {
		char SeverMsg[100];
		ClientSocket.Receive(SeverMsg, 100, 0);

		this->handleDowMail(ClientSocket, USER);
		this->handleDowMail(ClientSocket, PASS);

		int nums = this->statSyntax(ClientSocket, STAT);
		this->sendToSever(ClientSocket, LIST);
		if (this->totalEmail == nums || nums == 0) {
			this->sendToSever(ClientSocket, QUIT);
			ClientSocket.Close();
			return token;
		}
		token = this->RETREmails(ClientSocket, RETR, nums);
		this->totalEmail = nums;
		this->sendToSever(ClientSocket, QUIT);
	}
	else cout << "Cannot connect to server !!!" << endl;
	ClientSocket.Close();
	return token;
}


POP3_command::POP3_command(std::string user, std::string pass,std::string fullname, int total, int port) {
	this->username = user;
	this->passwork = pass;
	this->fullname = fullname;
	this->totalEmail = total;
	this->port = port;
}


bool POP3_command::checkLogin(CSocket& ClientSocket) {
	ClientSocket.Create();
	bool isUser = false , isPass = false, isQuit = false;
	if (ClientSocket.Connect(_T(LOOPBACK_IP), this->port) != 0) {

		char SeverMsg[1000];
		ClientSocket.Receive(SeverMsg, 1000, 0);
		std::cout << this->username << "\n";

		isUser = this->sendToSeverMsg(ClientSocket, USER, this->username);
		isPass = this->sendToSeverMsg(ClientSocket, PASS, this->passwork);
		isQuit = this->sendToSever(ClientSocket, QUIT);
	}
	else cout << "Cannot connect to server !!!" << endl;
	ClientSocket.Close();
	return (isUser && isPass) && isQuit;
}