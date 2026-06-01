#pragma once
#include <iostream>
#include <cstdlib>
#include <ctime>
#include <string>

class KiwerAPI {
public:
	void login(std::string ID, std::string password) {
		std::cout << ID << " login success\n";
	}

	void buy(std::string stockCode, int count, int price) {
		std::cout << stockCode << " : Buy stock ( " << price << " * " << count << ")\n";
	}

	void sell(std::string stockCode, int count, int price) {
		std::cout << stockCode << " : Sell stock ( " << price << " * " << count << ")\n";
	}

	int currentPrice(std::string stockCode) {
		std::srand(std::time(NULL));
		int ret = std::rand() % 10 * 100 + 5000;
		return ret;
	}
};