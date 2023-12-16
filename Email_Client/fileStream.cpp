#include "fileStream.h"

bool fs::isCheckFolderExists(std::string path) {
	#include <sys/stat.h>
	struct stat sb;
	if (stat(path.c_str(), &sb) == 0) return true;
	return false;
}

void fs::rmdirAndChildFile(std::string directoryName) {
	std::string path = "rmdir /s /q " + directoryName;
	system(path.c_str());
 }

bool fs::fileExists(const std::string& filename) {
    std::ifstream file(filename);
    return file.good();
}

void fs::delFile(std::string fileName) {
	#include <cstdio>
	 if (std::remove(fileName.c_str()) != 0)
		 std::cout << "Error deleting file " << fileName << "\n";
	 else std::cout << "File successfully deleted" << fileName << "\n";
}

void fs::createFolder(std::string path) {
	#include <Windows.h>
	fs fileStream;
	if (!(fileStream.isCheckFolderExists(path))) {
		int length = MultiByteToWideChar(CP_UTF8, 0, path.c_str(), -1, nullptr, 0);
		std::wstring folderPath(length, L'\0');
		MultiByteToWideChar(CP_UTF8, 0, path.c_str(), -1, &folderPath[0], length);
		CreateDirectory(folderPath.c_str(), NULL);
	}
}

void fs::writeFileMainConfig(std::string path = "../config.json", std::string username = "", std::string pass = "",std::string fullname = "", int autoload = 10, int smtpPort = 2222, int pop3Port = 3333, std::string mailsever = "127.0.0.1", int total = 0) {
    #include<json/writer.h>
    Json::Value event;

    event["extra"]["email_status"]["totalEmail"] = total;
    event["general"]["mailsever"] = mailsever;
    event["general"]["POP3"] = pop3Port;
    event["general"]["SMTP"] = smtpPort;
    event["general"]["autoload"] = autoload;
    event["general"]["passwork"] = pass;
    event["general"]["username"] = username;
    event["general"]["fullname"] = fullname;

    std::ofstream file(path);

    if (!file.is_open()) {
        std::cout << "Cannot write config file." << std::endl;
        return;
    }

    Json::StyledWriter styledWriter;
    file << styledWriter.write(event);
    file.close();
}

Json::Value fs::readConfig(std::string path) {
    std::ifstream file(path);

    if (!file.is_open()) {
        std::cout << "Cannot read config file." << std::endl;
        return Json::Value();
    }

    Json::CharReaderBuilder readerBuilder;
    Json::Value jsonData;
    Json::parseFromStream(readerBuilder, file, &jsonData, nullptr);

    file.close();
    return jsonData;
}

void fs::writeEmailToFolder(string path, Json::Value& event) {
#include<json/writer.h>
	std::ofstream file(path);

	if (!file.is_open()) {
		std::cout << "Cannot write config file." << std::endl;
		return;
	}

	Json::StyledWriter styledWriter;
	file << styledWriter.write(event);
	file.close();
}

vector<Email> fs::readEmailToFolder(string filename) {
    vector<Email> tokens;
    std::ifstream file(filename);
    if (!file.is_open()) {
        std::cerr << "Cannot open file: " << filename << std::endl;
        return tokens;
    }

    Json::CharReaderBuilder readerBuilder;
    Json::Value root;
    JSONCPP_STRING errs;
    
    try {
        Json::parseFromStream(readerBuilder, file, &root, &errs);
    }
    catch (const std::exception& e) {
        std::cerr << "Error parsing JSON: " << e.what() << std::endl;
        file.close();
        return tokens;
    }

    file.close();
    for (const auto& entry : root) {
        string boundary = entry["boundary"].asString();
        vector<string> cc_list;
        const Json::Value ccList = entry["cc_list"];
        for (const auto& cc : ccList) {
            cc_list.push_back(cc.asString());
        }
        vector<string> fileName_attached;
        const Json::Value fileNameAttached = entry["fileName_attached"];
        for (const auto& fileName : fileNameAttached) {
            cc_list.push_back(fileName.asString());
        }
        vector<string> file_attached;
        const Json::Value fileAttached = entry["file_attached"];
        for (const auto& file : fileAttached) {
            file_attached.push_back(file.asString());
        }

        string content = entry["content"].asString();
        string folder = entry["folder"].asString();
        string mailFrom = entry["mailFrom"].asString();
        int state = entry["state"].asInt();
        string subject = entry["subject"].asString();
        string userFrom = entry["userFrom"].asString();
        
        Email event(state, subject, content, mailFrom, userFrom, folder, boundary, cc_list, file_attached, fileName_attached);
        tokens.push_back(event);
        
    }
    return tokens;
}