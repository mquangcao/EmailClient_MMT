#pragma once
#include "thread.h"
#include "fileStream.h"

CWinApp theApp;
using std::cout;

int _tmain(int argc, TCHAR* argv[], TCHAR* envp[])
{
	int nRetCode = 0;
	// initialize MFC and print and error on failure
	if (!AfxWinInit(::GetModuleHandle(NULL), NULL, ::GetCommandLine(), 0)){
		// TODO: change error code to suit your needs
		_tprintf(_T("Fatal Error: MFC initialization failed\n"));
		nRetCode = 1;
	} else {
		// initialize the library
		if (AfxSocketInit() == FALSE) {
			cout << "Khong the khoi tao Socket Libraray";
			return FALSE;
		}
		CSocket ClientSocket;
		CSocket ClientSocketDowMail;
		fs filestream;

		std::string path = "../Source";

		std::string fullPath;
		if (filestream.isCheckFolderExists(path) == true) {
			fullPath = path + "/";
		}
		else fullPath = "../";
		if (filestream.isCheckFolderExists(fullPath + "Emails") == false) {
			filestream.createFolder(fullPath + "Emails");
		}
		if (filestream.isCheckFolderExists(fullPath + "File") == false) {
			filestream.createFolder(fullPath + "File");
		}
		int index = 0;
		while (true) {
			std::string status;
			index++;
			if (index != 1) {
			std::cout << "=============================================================================================================\n";
			std::cout << "Neu khong muon dang nhap vui long nhap [exit] de thoat chuong trinh, neu muon tiep tuc nhap bat ki ki tu gi: ";
			std::getline(std::cin, status);
			if (status == "exit") return nRetCode;
			system("cls");
			}
			User* user = new User();
			user->setPath(fullPath);
			if (user->login(ClientSocket)) {
				std::thread dowMail(autoDowEmail, std::ref(ClientSocketDowMail), std::ref(user));
				user->communicate(ClientSocket);
				dowMail.join();
			}
			else {
				std:cout << "Khong the login, vui long kiem tra da bat sever chua.\n";
				system("pause");
			}
			int tt = user->statusExit;
			delete user;
			if (tt == 0) return nRetCode;
		}
	}

	return nRetCode;
}