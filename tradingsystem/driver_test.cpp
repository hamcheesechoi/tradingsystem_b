#include "gmock/gmock.h"
#include "driver.cpp"
#include "nemo_driver.cpp"
#include "kiwer_driver.cpp"

#include <stdexcept>

using std::string;
using namespace testing;

class TestDriver : public StockerBrocker {
public:
	void buy(std::string stockCode, int price, int count) override {
		stock_broker->buy(stockCode, price, count);

	};
	void sell(std::string stockCode, int price, int count) override {
		stock_broker->sell(stockCode, price, count);

	};
	int currentPrice(std::string stockCode, int minute) override {
		return stock_broker->currentPrice(stockCode, minute);
	};
	void login(std::string ID, std::string pass) override {
		stock_broker->login(ID, pass);
	};

	/**<TEST ???꾩슂 ???⑥닔 異붽??⑸땲??>**/
	TestDriver(string ID, string pass) : _ID(ID), _pass(pass) {}

	void setStockCode(string stockCode) {
		_stockCode = stockCode;
	}

	int getStockCount(string stockCode) {
		return this->_count;
	}

	void setBroker(string broker_name) {
		if (broker_name == "nemo")	stock_broker = new NemoStockerBrocker();
		else if(broker_name == "kiwer")	stock_broker = new KiwerStockerBrocker();
	}

private:
	string _ID; // ?댄썑 vector濡?愿由??꾩슂
	string _pass; // ?댄썑 vector濡?愿由??꾩슂
	string _stockCode; // ?댄썑 vector濡?愿由??꾩슂

	int _count; // ?섎웾
	StockerBrocker* stock_broker;
};


class DriverTestFixture : public Test {
public:
	TestDriver td{ "ID", "PW" };

	DriverTestFixture() {
		td.setStockCode("STOCK_CODE");
	}

	void SetUp() {
		td.setBroker("nemo");
	}
};


/************<Fail Case>*************/
// 1. id 오류
TEST_F(DriverTestFixture, IDError) {
	string id = "ERROR_ID";
	string password = "ERROR_PW";
	
	EXPECT_THROW(td.login(id, password), IdExcpetion) << "ID Error";
}


// 2. password 오류
TEST_F(DriverTestFixture, PWError) {
	string id = "ID";
	string password = "ERROR_PW";
	
	EXPECT_THROW(td.login(id, password), PwExcpetion) << "PW Error";
}

// 3. stock code 오류
TEST_F(DriverTestFixture, StockError) {
	string stockCode = "ERROR_STOCK";

	int price = 0;
	int count = 0;
	int munute = 0;
	EXPECT_THROW(td.buy(stockCode, price, count), StockExcpetion) << "Stock Error";
	EXPECT_THROW(td.sell(stockCode, price, count), StockExcpetion) << "Stock Error";
	EXPECT_THROW(td.currentPrice(stockCode, munute), StockExcpetion) << "Stock Error";
}

// 4. stock 잔여량 에러
TEST_F(DriverTestFixture, RemainStockError) {

	string stockCode = "STOCK_CODE";
	td.buy(stockCode, 0, 3);

	EXPECT_THROW(td.sell(stockCode, 0, 4), NotAvailableSellExcpetion);
}


/************<Success Case>*************/
// 1. sell
TEST_F(DriverTestFixture, sellSuccess) {
	string stockCode = "STOCK_CODE";
	td.buy(stockCode, 0, 3);

	td.sell(stockCode, 0, 2);

	EXPECT_EQ(td.getStockCount(stockCode), 1);
}

// 2. current Price (app test 개발 후 테스트 필요)
/*
TEST_F(DriverTestFixture, sellSuccess) {
	string stockCode = "STOCK_CODE";

	EXPECT_EQ(td.currentPrice(stockCode, 3), 200);
}
*/