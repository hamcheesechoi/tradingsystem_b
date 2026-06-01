#include "driver.cpp"
#include "kiwer_api.cpp"
#include <windows.h>

class KiwerStockerBrocker : public StockerBrocker {
public:
	void buy(std::string stockCode, int price, int count) {
		if (stockCode == "ERROR_STOCK")	throw StockExcpetion();
		broker.buy(stockCode, count, price);
	};
	void sell(std::string stockCode, int price, int count) {
		if (stockCode == "ERROR_STOCK")	throw StockExcpetion();
		broker.sell(stockCode, count, price);
	}
	int currentPrice(std::string stockCode, int minute) {
		if (stockCode == "ERROR_STOCK")	throw StockExcpetion();
		if (minute <= 0) minute = 1;
		Sleep(minute);
		return broker.currentPrice(stockCode);
	}
	void login(std::string ID, std::string pass) {
		if (ID == "ERROR_ID")	throw IdExcpetion();
		if (pass == "ERROR_PW")	throw PwExcpetion();
		broker.login(ID, pass);
	}

private:
	KiwerAPI broker;
};