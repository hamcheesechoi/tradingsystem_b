#include "driver.cpp"
#include "nemo_api.cpp"

class NemoStockerBrocker : public StockerBrocker {
public:
	void buy(std::string stockCode, int price, int count) {
		broker.purchasingStock(stockCode, price, count);
	};
	void sell(std::string stockCode, int price, int count) {
		broker.sellingStock(stockCode, price, count);
	}
	int currentPrice(std::string stockCode, int minute) {
		return broker.getMarketPrice(stockCode, minute);
	}
	void login(std::string ID, std::string pass) {
		broker.certification(ID, pass);
	}

private:
	NemoAPI broker;
};