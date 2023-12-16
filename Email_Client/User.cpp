#include "User.h"

User::User() {
	fs filestream;
	std::ifstream file(this->getPath() + "config.json");
	if (file.good()) {
		Json::Value config = filestream.readConfig(this->getPath() + "config.json");
		this->username = config["general"]["username"].asString();
		this->passwork = config["general"]["passwork"].asString();
		this->fullname = config["general"]["fullname"].asString();
		this->autoload = config["general"]["autoload"].asInt();
		this->SMTPPort = config["general"]["SMTP"].asInt();
		this->POP3Port = config["general"]["POP3"].asInt();
		this->mailsever = config["general"]["mailsever"].asString();
		this->totalEmail = config["extra"]["totalEmail"].asInt();
	}
	else {
		this->username = "";
		this->passwork = "";
		this->fullname = "";
		this->autoload = 10;
		this->SMTPPort = 2222;
		this->POP3Port = 3333;
		this->mailsever = "127.0.0.1";
		this->totalEmail = 0;
	}
}

User::User(std::string user, std::string pass, std::string fullname,int autoload,int smtpPott,int pop3Port, std::string mailsever, int totalEmail) {
	this->username = user;
	this->passwork = pass;
	this->fullname = fullname;
	this->autoload = autoload;
	this->SMTPPort = smtpPott;
	this->POP3Port = pop3Port;
	this->mailsever = mailsever;
	this->totalEmail = totalEmail;
	this->writeConfig();
}



User::~User() {
	this->saveDB();
	if (this->isLogin == false) {
		this->username = "";
		this->passwork = "";
		this->fullname = "";
		this->autoload = 10;
		this->SMTPPort = 2222;
		this->POP3Port = 3333;
		this->mailsever = "127.0.0.1";
		this->totalEmail = 0;
	}
	this->writeConfig();
}

bool User::login(CSocket& ClientSocket) {
	if (this->username == "") {
		std::string tmp;
		std::cout << "  ----------- [LOGIN] ----------- \n";
		std::cout << " - Nhap vao ten :";
		std::getline(std::cin, tmp);
		this->fullname = tmp;
		
		std::cout << " - Nhap vao tai khoan : ";
		std::getline(std::cin, tmp);
		this->username = tmp;

		std::cout << " - Nhap vao mat khau : ";
		std::getline(std::cin, tmp);
		this->passwork = tmp;

		this->getDB();
		this->pop3 = new POP3_command(this->username, this->passwork, this->fullname, this->totalEmail, this->POP3Port);
		std::cout << " - Ban co muon cau hinh them, chon [Y/N] : ";
		char select = 'N';
		std::cin >> select;
		while (true) {
			if (select == 'N' || select == 'n') {
				if (this->pop3->checkLogin(ClientSocket)) {
					this->autoload = 10;
					this->SMTPPort = 2222;
					this->POP3Port = 3333;
					this->mailsever = "127.0.0.1";
				system("cls");
				std::cout << "  ------ [LOGIN SUCCESS] ------ \n";
				this->writeConfig();
				return true;
				}
				this->isLogin = false;
				std::cout << " - Tai khoan hoac mat khau khong hop le.\n";
				return false;
			}
			if (select == 'Y' || select == 'y') {
				std::cout << "  ------ [CONFIG] ------ \n";
				std::cout << " - Nhap vao emailsever (Khong The Thay Doi).\n";
				std::cout << " - Nhap vao SMTP Port : ";
				int port = 2222;
				std::cin >> port;
				while (port < 1000) {
					std::cout << " - Port phai lon hon 1000.\n - Vui long nhap lai: ";
					std::cin >> port;
				}
				this->SMTPPort = port;
				std::cout << " - Nhap vao POP3 Port : ";
				port = 3333;
				std::cin >> port;
				while (port < 1000) {
					std::cout << " - Port phai lon hon 1000.\n - Vui long nhap lai: ";
					std::cin >> port;
				}
				this->POP3Port = port;
				int time = 10;
				std::cout << " - Nhap vao thoi gian tu dong tai email (thoi gian tinh bang s) :";
				std::cin >> time;
				while (time < 0) {
					std::cout << " - Thoi gian phai lon hon 0.\n - Vui long nhap lai: ";
					std::cin >> time;
				}
				this->autoload = time;
				std::cout << " ---------- [SUCCES] -----------\n";
				if (this->pop3->checkLogin(ClientSocket)) {
					system("cls");
					std::cout << "  ------ [LOGIN SUCCESS] ------ \n";
					this->writeConfig();
					return true;
				}
				this->isLogin = false;
				std::cout << " - Tai khoan hoac mat khau khong hop le.\n";
				return false;
			}
			else {
				std::cout << "Vui long nhap lai, [Y/N]: ";
				std::cin >> select;
			}
		}
	}
	else {
		std::cout << " ------ -------------------------------- ------ \n";
		std::cout << " ----- [Welcome " << this->username << "] ----- \n";
		std::cout << " ------ -------------------------------- ------ \n\n";
		this->getDB();
		this->getConfig();
		this->pop3 = new POP3_command(this->username, this->passwork,this->fullname,this->totalEmail,this->POP3Port);
	}
	return true;
}

void User::getConfig() {
	fs filestream;
	if (filestream.fileExists("../config.json") == false) return;
	Json::Value event = filestream.readConfig("../config.json");
	this->username = event["general"]["username"].asString();
	this->totalEmail = event["extra"]["email_status"]["totalEmail"].asInt();
	this->POP3Port = event["general"]["POP3"].asInt();
	this->SMTPPort = event["general"]["SMTP"].asInt();
	this->autoload = event["general"]["autoload"].asInt();
	this->fullname = event["general"]["fullname"].asString();
	this->mailsever = event["general"]["mailsever"].asString();
	this->passwork = event["general"]["passwork"].asString();
}


void mergeJsonObjects(Json::Value& dest, const Json::Value& src) {
	if (src.isObject()) {
		for (const auto& key : src.getMemberNames()) {
			dest[key] = src[key];
		}
	}
	else if (src.isArray()) {
		for (const auto& element : src) {
			dest.append(element);
		}
	}
}

void User::saveDB() {
#include <json/writer.h>
	string path = this->getPath() + "Emails/" + this->username + ".json";
	Json::Value event;
	if (this->emails.size() == 0) return;
	event = this->emails[0].handleJsonEmail(0);
	for (int i = 1; i < this->totalEmail; i++) {
		mergeJsonObjects(event, this->emails[i].handleJsonEmail(i));
	}
	fs filestream;
	filestream.writeEmailToFolder(path, event);
}


void User::logout() {
	this->saveDB();
	this->username = "";
	this->passwork = "";
	this->fullname = "";
	this->autoload = 0;
	this->SMTPPort = 2222;
	this->POP3Port = 3333;
	this->mailsever = "127.0.0.1";
	this->totalEmail = 0;
	this->writeConfig();
	delete this->pop3;
	std::cout << "  ------ [LOGOUT SUCCESS] ------ \n";
}

void User::writeConfig() {
	fs fileStream;
	fileStream.writeFileMainConfig(this->getPath() + "config.json", this->username, this->passwork,this->fullname, this->autoload,this->SMTPPort, this->POP3Port, this->mailsever, this->totalEmail);
}

void User::communicate(CSocket& ClientSocket){
		bool isOn = true;
		do {

			std::cout << "MENU\n"
				<< "1. De gui email.\n"
				<< "2. De xem danh sach cac email da nhan.\n"
				<< "3. Logout.\n"
				<< "0. Exits.\n";
			int select = -1;
			std::cout << " - Nhap vao lua chon cua ban : ";
			std::cin >> select;
			switch (select) {
			case 1: 
				std::cout << "Ban da chon 1: \n";
				mailsendingwithAttachfile(ClientSocket, this->username,this->fullname,this->SMTPPort);
				break;
			case 2:
				std::cout << "Ban da chon 2.\n";
				this->pushEmails(ClientSocket);
				this->getListFolder();
				break;
			case 3: 			
				std::cout << "Ban da chon 3.\n";
				std::cout << "Logout.\n";
				this->logout();
				this->statusLogout = 3;
				isOn = false;
				break;
			case 0:
				std::cout << "Exits.\n";
				this->saveDB();
				this->statusExit = 0;
				isOn = false;
				break;
			default:
				std::cout << "Vui long nhap lai: ";
			}
		} while (isOn);
}

void User::pushEmails(CSocket& ClientSocket) {
	vector<Email> token = this->pop3->dowMail(ClientSocket);
	for (int i = 0; i < token.size(); i++) {
		this->emails.push_back(token[i]);
	}
	this->totalEmail = this->emails.size();
}

int User::getAutoload() {
	return this->autoload;
}


void User::getListFolder() {
	bool run = true;
	system("cls");
	while (run) {
		std::string msg = "";
		int mailFolder = -1;
		std::cout << "FOLDER LIST: \n"
			<< "1. Inbox\n"
			<< "2. Important\n"
			<< "3. Work\n"
			<< "4. Spam\n"
			<< "5. Project\n"
			<< "0 . Exit folder list.\n";
		std::cout << "Nhap vao folder mail muon doc, ( nhap 0 de thoat ) : ";
		std::cin >> mailFolder;
		bool isOn = true;
		while (isOn) {
			switch (mailFolder) {
			case 1:
				msg += "Inbox";
				isOn = false;
				break;
			case 2:
				msg += "Important";
				isOn = false;
				break;
			case 3:
				msg += "Work";
				isOn = false;
				break;
			case 4:
				msg += "Spam";
				isOn = false;
				break;
			case 5:
				msg += "Project";
				isOn = false;
				break;
			case 0:
				isOn = false;
				return;
			default:
				std::cout << "Vui long nhap lai: ";
				std::cin >> mailFolder;
				break;
			}
		}
		int total = this->inMailListFolder(msg);
		if (total != 0) {
			std::cout << "Nhap vao  mail muon doc, nhap 0 de thoat: ";
			
			std::cin >> mailFolder;
			if (mailFolder == 0) return;
			while (mailFolder > total || mailFolder < 0) {
				std::cout << " - Vui long nhap vao mail co trong list: ";
				std::cin >> mailFolder;
			}
			std::cout << this->emailsCurent[mailFolder - 1].formatEmail();
			this->emails[this->indexEmailsCurent[mailFolder - 1]].formatEmail();
			this->emailsCurent[mailFolder - 1].getFile();
		}	
		if (this->emailsCurent.size() != 0) {
			this->emailsCurent.clear();
			this->indexEmailsCurent.clear();
		}
	}
}

int User::inMailListFolder(std::string folder) {
	int index = 0;
	bool isCheck = false;
	std::string msg = "===== Cac mail co trong " + folder + "======\r\n";
	for (int i = 0; i < this->totalEmail; i++) {
		if (this->emails[i].getFolder() == folder) {
			isCheck = true;
			msg += (std::to_string(++index) + ". " + this->emails[i].formatTitileList() + "\r\n");
			this->emailsCurent.push_back(this->emails[i]);
			this->indexEmailsCurent.push_back(i);
		}
	}
	
	if (isCheck) {
		system("cls");
		std::cout << msg;
	}
	else {
		std::cout << " \n========================================\n";
		std::cout << " - Khong co email nao trong folder nay.\n";
		std::cout << "========================================\n\n";
	}
	return index;
}

void User::getDB() {
	fs filestream;
	if (filestream.fileExists(this->getPath() + "Emails/" + this->username + ".json") == false) return;
	this->emails = filestream.readEmailToFolder(this->getPath() + "Emails/" + this->username + ".json");
	this->totalEmail = this->emails.size();
}


void User::setPath(std::string path) {
	this->path = path;
}
std::string User::getPath() {
	return this->path;
}