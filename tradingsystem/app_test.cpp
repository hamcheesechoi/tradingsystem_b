#include "gmock/gmock.h"
#include "app.cpp"
#include "driver.cpp"

using namespace testing;

class MockBrocker : public StockerBrocker {
public:
    MOCK_METHOD(void, login, (std::string ID, std::string password), (override));
    MOCK_METHOD(void, buy, (std::string stockCode, int price, int count), (override));
    MOCK_METHOD(void, sell, (std::string stockCode, int price, int count), (override));
    MOCK_METHOD(int, currentPrice, (std::string stockCode, int minute), (override));
};

TEST(App, TC1) {
    // buy nice timing
    // 200ms 주기로 3회 가격을 읽고, 가격이 올라가는 추세인지 파악
    // 가격이 올라가는 추세라면, 총 금액을 최대한 사용하여 최대 수량만큼 매수한다.
    // 마지막에 읽은 가격으로 매수한다.

    MockBrocker mock;
    auto_trading_system app(&mock);

    std::string stockCode = "005930";
    int budget = 1000000;

    // 3회 가격 조회: 5000 → 5100 → 5200 (상승 추세)
    EXPECT_CALL(mock, currentPrice(stockCode, 200))
        .WillOnce(Return(5000))
        .WillOnce(Return(5100))
        .WillOnce(Return(5200));

    // 마지막 가격(5200)으로 최대 수량 매수: 1000000 / 5200 = 192
    EXPECT_CALL(mock, buy(stockCode, 5200, 192))
        .Times(1);

    app.buy_nice_timing(stockCode, budget);
}

TEST(App, TC2) {
    // buy nice timing - 하락 추세일 때 매수하지 않음

    MockBrocker mock;
    auto_trading_system app(&mock);

    std::string stockCode = "005930";
    int budget = 1000000;

    // 3회 가격 조회: 5200 → 5100 → 5000 (하락 추세)
    EXPECT_CALL(mock, currentPrice(stockCode, 200))
        .WillOnce(Return(5200))
        .WillOnce(Return(5100))
        .WillOnce(Return(5000));

    // 하락 추세이므로 매수하지 않음
    EXPECT_CALL(mock, buy(_, _, _))
        .Times(0);

    app.buy_nice_timing(stockCode, budget);
}

TEST(App, TC3) {
    // sell nice timing
    // 200ms 주기로 3회 가격을 읽고, 가격이 내려가는 추세인지 파악
    // 가격이 내려가는 추세라면, 보유 수량 전량을 마지막에 읽은 가격으로 매도한다.

    MockBrocker mock;
    auto_trading_system app(&mock);

    std::string stockCode = "005930";
    int holdCount = 50;

    // 3회 가격 조회: 5200 → 5100 → 5000 (하락 추세)
    EXPECT_CALL(mock, currentPrice(stockCode, 200))
        .WillOnce(Return(5200))
        .WillOnce(Return(5100))
        .WillOnce(Return(5000));

    // 마지막 가격(5000)으로 전량 매도
    EXPECT_CALL(mock, sell(stockCode, 5000, holdCount))
        .Times(1);

    app.sell_nice_timing(stockCode, holdCount);
}

TEST(App, TC4) {
    // sell nice timing - 상승 추세일 때 매도하지 않음

    MockBrocker mock;
    auto_trading_system app(&mock);

    std::string stockCode = "005930";
    int holdCount = 50;

    // 3회 가격 조회: 5000 → 5100 → 5200 (상승 추세)
    EXPECT_CALL(mock, currentPrice(stockCode, 200))
        .WillOnce(Return(5000))
        .WillOnce(Return(5100))
        .WillOnce(Return(5200));

    // 상승 추세이므로 매도하지 않음
    EXPECT_CALL(mock, sell(_, _, _))
        .Times(0);

    app.sell_nice_timing(stockCode, holdCount);
}
