#pragma once
#include <string>
#include <exception>

#define interface struct

using namespace std;

interface StockerBrocker{
	virtual void buy(std::string stockCode, int price, int count) = 0;
	virtual void sell(std::string stockCode, int price, int count) = 0;
	virtual int currentPrice(std::string stockCode, int minute) = 0;
	virtual void login(std::string ID, std::string pass) = 0;
};

class IdExcpetion : public exception {

};

class PwExcpetion : public exception {

};

class StockExcpetion : public exception {

};

class NotAvailableSellExcpetion : public exception {

};

