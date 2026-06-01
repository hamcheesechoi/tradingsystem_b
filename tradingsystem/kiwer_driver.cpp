#include "driver.cpp"
#include "kiwer_api.cpp"

class KiwerStockerBrocker : public StockerBrocker {
public:
	void buy(std::string stockCode, int price, int count) {
		broker.buy(stockCode, price, count);
	};
	void sell(std::string stockCode, int price, int count) {
		broker.sell(stockCode, price, count);
	}
	int currentPrice(std::string stockCode, int minute) {
		// @todo minute
		return broker.currentPrice(stockCode);
	}
	void login(std::string ID, std::string pass) {
		broker.login(ID, pass);
	}

private:
	KiwerAPI broker;
};