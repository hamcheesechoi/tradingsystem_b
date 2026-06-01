#pragma once
#include <string>
#include <exception>

struct StockerBrocker{
	virtual void buy(std::string stockCode, int price, int count) = 0;
	virtual void sell(std::string stockCode, int price, int count) = 0;
	virtual int currentPrice(std::string stockCode, int minute) = 0;
	virtual void login(std::string ID, std::string pass) = 0;
};

class IdExcpetion : public std::exception {

};

class PwExcpetion : public std::exception {

};

class StockExcpetion : public std::exception {

};

class NotAvailableSellExcpetion : public std::exception {

};

