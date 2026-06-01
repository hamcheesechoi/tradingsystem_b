#include "gmock/gmock.h"
#include "driver.cpp"
#include <stdexcept>

using std::string;
using namespace testing;


class DriverTestFixture : public Test {
public:
	TestDriver td{ "ID", "PW" };

	DriverTestFixture() {
		td.setStockCode("STOCK_CODE");
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