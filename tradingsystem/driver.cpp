#include <string>

#define interface struct

interface StockerBrocker{
	virtual void buy(std::string stockCode, int price, int count) = 0;
	virtual void sell(std::string stockCode, int price, int count) = 0;
	virtual void currentPrice(std::string stockCode, int minute) = 0;
	virtual void login(std::string ID, std::string pass) = 0;
};