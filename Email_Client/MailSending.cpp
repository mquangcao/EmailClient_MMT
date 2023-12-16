#include"MailSending.h"
using namespace Poco;
using namespace std;

void MailSending(CSocket &ClientSocket,string &userName, string& user) {
	char SeverMsg[1000];
	string ehlo = "EHLO [127.0.0.1]\r\n";
	string content1 = "Content-Type: text/plain; charset=UTF-8; format=flowed\r\n";
	string content2 = "Content-Transfer-Encoding: 7bit\r\n";
	string tmp;
	char data[] = "DATA\r\n";
	char subject[200];              //Nhap chu de   
	char receiveEmail[100];         //To email
	char InputCC[100];              //Cc email
	char InputBCC[100];             //Bcc email
	char rcptto[200];                
	const char* newline = "\r\n";
	char contentEmail[1000];
	
	memset(SeverMsg, 0, 1000);
	ClientSocket.Receive(SeverMsg, 1000, 0);
	//send Hello
	ClientSocket.Send(ehlo.c_str(), ehlo.length(), 0);
	memset(SeverMsg, 0, 1000);

	ClientSocket.Receive(SeverMsg, 1000, 0);
	//send email From
	tmp = "MAIL FROM:<" + userName + ">\r\n";
	ClientSocket.Send(tmp.c_str(), tmp.length(), 0);

	memset(SeverMsg, 0, 1000);
	ClientSocket.Receive(SeverMsg, 1000, 0);
	cin.ignore();
	//Input mail receive 
	cout << "Nhap vao nguoi nhan: ";
	cin.getline(receiveEmail, sizeof(receiveEmail));
	sprintf(rcptto, "RCPT TO:<%s>\r\n", receiveEmail);
	ClientSocket.Send(rcptto, strlen(rcptto), 0);

	memset(SeverMsg, 0, 1000);
	ClientSocket.Receive(SeverMsg, 1000, 0);

	//Input CC
	cout << "Cc: ";
	cin.getline(InputCC, sizeof(InputCC));
	if (strlen(InputCC)!=0) {

		sprintf(rcptto, "RCPT TO:<%s>\r\n", InputCC);
		ClientSocket.Send(rcptto, strlen(rcptto), 0);

		memset(SeverMsg, 0, 1000);
		ClientSocket.Receive(SeverMsg, 1000, 0);
	}
	cout << "Bcc: ";
	cin.getline(InputBCC, sizeof(InputBCC));
	if (strlen(InputBCC) != 0) {
		sprintf(rcptto, "RCPT TO:<%s>\r\n", InputBCC);
		ClientSocket.Send(rcptto, strlen(rcptto), 0);
		memset(SeverMsg, 0, 1000);
		ClientSocket.Receive(SeverMsg, 1000, 0);
	}

	// Input data to server 
	ClientSocket.Send(data, strlen(data), 0);

	memset(SeverMsg, 0, 1000);
	ClientSocket.Receive(SeverMsg, 1000, 0);
	//Send 4 commend 

	sprintf(rcptto, "To: %s\r\n", receiveEmail);
	ClientSocket.Send(rcptto, strlen(rcptto), 0);
	if (strlen(InputCC)!=0) {
		sprintf(rcptto, "Cc: %s\r\n", InputCC);
		ClientSocket.Send(rcptto, strlen(rcptto), 0);
	}
	tmp = "From: " + user + " <" + userName+">\r\n";
	ClientSocket.Send(tmp.c_str(),tmp.length(), 0);

	//Input subject to server 
	cout << "Subject: ";
	cin.getline(subject, sizeof(subject));
	if (strlen(subject)!=0) {
		sprintf(rcptto, "Subject: %s\r\n", subject);
		ClientSocket.Send(rcptto, strlen(rcptto), 0);

	}
	ClientSocket.Send(content1.c_str(),content1.length(), 0);
	ClientSocket.Send(content2.c_str(),content2.length(), 0);
	ClientSocket.Send(newline, strlen(newline), 0);
	//Send message
	cout << "Email content: ";
	std::cin.getline(contentEmail, 1001);
	if (strlen(contentEmail) != 0) {
		sprintf(rcptto, "%s\r\n", contentEmail);
		ClientSocket.Send(rcptto, strlen(rcptto), 0);
	}
	ClientSocket.Send(newline, strlen(newline), 0);
		
}

void EncodeFileToBase64(const std::string& filePath, std::string& encodedContent) {
	std::ifstream fileStream(filePath, std::ios::binary);

	if (!fileStream.is_open()) {
		std::cerr << "Error opening file: " << filePath << std::endl;
		return;
	}

	std::stringstream buffer;
	buffer << fileStream.rdbuf();

	fileStream.close();  // Close the file stream

	std::ostringstream encodedBuffer;
	Poco::Base64Encoder base64Encoder(encodedBuffer);
	Poco::StreamCopier::copyStream(buffer, base64Encoder);

	base64Encoder.close();  // Close the Base64 encoder after copying

	encodedContent = encodedBuffer.str();  // Capture the Base64-encoded content
}
std::string generateRandomAlphabeticString(int length) {
	const char alphabet[] = "ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz";
	const int alphabetLength = sizeof(alphabet) - 1; // Exclude null terminator

	std::string result;

	// Tạo một chuỗi chứa các chữ cái ngẫu nhiên
	std::random_device rd;
	std::mt19937 gen(rd());
	std::uniform_int_distribution<> dis(0, alphabetLength - 1);

	for (int i = 0; i < length; ++i) {
		char randomChar = alphabet[dis(gen)];
		result += randomChar;
	}

	return result;
}


string extractFileName(const std::string& fullPath) {
	size_t lastSlash = fullPath.find_last_of("\\/");
	if (lastSlash != std::string::npos) {
		return fullPath.substr(lastSlash + 1);
	}
	return fullPath;
}
std::uintmax_t getFileSize(const std::string& filePath) {
	std::ifstream fileStream(filePath, std::ios::binary | std::ios::ate);
	return static_cast<std::uintmax_t>(fileStream.tellg());
}
void send_mail_with_oneFile(CSocket& clientSocket,string&filePath) {
	const char* newline = "\r\n";
	string base64Content;
	EncodeFileToBase64(filePath, base64Content);
	//lay ten tep
	std::string fileName = extractFileName(filePath);
	// Nội dung đính kèm
	string attachmentContent =
		"Content-Type: text/plain; charset=UTF-8; name=\"" + fileName + "\"" + newline +
		"Content-Disposition: attachment; filename=\"" + fileName + "\"" + newline +
		"Content-Transfer-Encoding: base64" + newline + newline +
		base64Content + newline + newline;

	// Tạo nội dung của email
	string email = attachmentContent;

	// Gửi email qua socket
	clientSocket.Send(email.c_str(), email.length(), 0);
}
void mailsendingwithAttachfile(CSocket& ClientSocket, string& userName, string& user, int smtpPort) {
	ClientSocket.Create();
	if (ClientSocket.Connect(_T(LOOPBACK_IP), smtpPort) != 0) {
		MailSending(ClientSocket, userName, user);
		// Bat dau gui dinh kem file
		int select;
		int number;
		const char* end = ".\r\n";
		string filePath;
		cout << "Do you want to attach file? Yes (1) No(0) :";
		cin >> select;

		if (select == 1) {
			string boundary = generateRandomAlphabeticString(24);
			string tmp = "--" + boundary + "\r\n";
			string contentType = "Content - Type: multipart / mixed; boundary=\"" + boundary + "\"" + "\r\n";
			ClientSocket.Send(contentType.c_str(), contentType.length(), 0);
			cout << "How many file attach?: ";
			cin >> number;
			cin.ignore();
			for (int i = 0; i < number; i++) {
				cout << "Link file (size <=3MB) " << i + 1 << ": ";
				getline(cin, filePath);
				// Kiểm tra xem kích thước có dưới 3MB không
				std::uintmax_t fileSize = getFileSize(filePath);
				if (fileSize > 3 * 1024 * 1024) {
					std::cerr << "Size of file larger <3MB, fail to send email" << std::endl;
					return;
				}
				ClientSocket.Send(tmp.c_str(), tmp.length(), 0);
				send_mail_with_oneFile(ClientSocket, filePath);
			}
			string closingBoundary = "--" + boundary + "--" + "\r\n";
			ClientSocket.Send(closingBoundary.c_str(), closingBoundary.length(), 0);
		}
		ClientSocket.Send(end, strlen(end), 0);
		cout << "Gui mail thanh cong\n";
	}
	else { cout << "Khong the ket noi den Server !!!" << endl; }
	
}

	 
