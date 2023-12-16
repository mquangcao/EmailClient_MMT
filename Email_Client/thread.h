#pragma once
#include "User.h"
#include "Client.h"
#include <thread>
#include <chrono>
#include <iostream>

void autoDowEmail(CSocket& ClientSocket, User*& user);