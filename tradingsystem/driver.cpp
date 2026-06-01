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

class TestDriver : public StockerBrocker {
public:
	void buy(std::string stockCode, int price, int count) override {
	
	};
	void sell(std::string stockCode, int price, int count) override {

	};
	int currentPrice(std::string stockCode, int minute) override {
		return 0;
	};
	void login(std::string ID, std::string pass) override {

	};

	/**<TEST ???„ìš” ???¨ìˆ˜ ì¶”ê??©ë‹ˆ??>**/
	TestDriver(string ID, string pass) : _ID(ID), _pass(pass) {}

	void setStockCode(string stockCode) {
		_stockCode = stockCode;
	}

	int getStockCount(string stockCode) {
		return this->_count;
	}
private:
	string _ID; // ?´í›„ vectorë¡?ê´€ë¦??„ìš”
	string _pass; // ?´í›„ vectorë¡?ê´€ë¦??„ìš”
	string _stockCode; // ?´í›„ vectorë¡?ê´€ë¦??„ìš”

	int _count; // ?˜ëŸ‰
};