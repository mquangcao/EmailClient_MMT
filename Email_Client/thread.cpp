#include "thread.h"



void autoDowEmail(CSocket& ClientSocket,User *& user) {
	while (true) {
		if (user->statusExit == 0) return;
		if (user->statusLogout == 3) return;
		user->pushEmails(ClientSocket);
		int timeLoad = user->getAutoload();
		while (timeLoad--) {
			std::this_thread::sleep_for(std::chrono::seconds(1));
			if (user->statusExit == 0) return;
			if (user->statusLogout == 3) return;
		}
	}
}
