#pragma once
#include "driver.cpp"
#include "nemo_api.cpp"

class NemoStockerBrocker : public StockerBrocker {
public:
	void buy(std::string stockCode, int price, int count) {
		if (stockCode == "ERROR_STOCK")	throw StockExcpetion();
		broker.purchasingStock(stockCode, price, count);
	};
	void sell(std::string stockCode, int price, int count) {
		if (stockCode == "ERROR_STOCK")	throw StockExcpetion();
		broker.sellingStock(stockCode, price, count);
	}
	int currentPrice(std::string stockCode, int minute) {
		if (stockCode == "ERROR_STOCK")	throw StockExcpetion();
		return broker.getMarketPrice(stockCode, minute);
	}
	void login(std::string ID, std::string pass) {
		if (ID == "ERROR_ID")	throw IdExcpetion();
		if (pass == "ERROR_PW")	throw PwExcpetion();
		broker.certification(ID, pass);
	}

private:
	NemoAPI broker;
};