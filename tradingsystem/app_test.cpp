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

class MockStrategy : public ITimingStrategy {
public:
    MOCK_METHOD(bool, shouldBuy, (const PriceHistory&), (override));
    MOCK_METHOD(bool, shouldSell, (const PriceHistory&), (override));
};

// ===================== AppTest (기본값: RisingTrendStrategy) =====================

class AppTest : public Test {
protected:
    MockBrocker mock;
    auto_trading_system app{ &mock };

    std::string stockCode = "005930";
    int budget = 1000000;
    int holdCount = 50;

    void setPrices(int price1, int price2, int price3) {
        EXPECT_CALL(mock, currentPrice(stockCode, 200))
            .WillOnce(Return(price1))
            .WillOnce(Return(price2))
            .WillOnce(Return(price3));
    }
};

TEST_F(AppTest, BuyOnUpTrend) {
    // buy nice timing
    // 200ms 주기로 3회 가격을 읽고, 가격이 올라가는 추세인지 파악
    // 가격이 올라가는 추세라면, 총 금액을 최대한 사용하여 최대 수량만큼 매수한다.
    // 마지막에 읽은 가격으로 매수한다.

    // 3회 가격 조회: 5000 → 5100 → 5200 (상승 추세)
    setPrices(5000, 5100, 5200);

    // 마지막 가격(5200)으로 최대 수량 매수: 1000000 / 5200 = 192
    EXPECT_CALL(mock, buy(stockCode, 5200, 192))
        .Times(1);

    app.buy_nice_timing(stockCode, budget);
}

TEST_F(AppTest, SellOnDownTrend) {
    // sell nice timing
    // 200ms 주기로 3회 가격을 읽고, 가격이 내려가는 추세인지 파악
    // 가격이 내려가는 추세라면, 보유 수량 전량을 마지막에 읽은 가격으로 매도한다.

    // 3회 가격 조회: 5200 → 5100 → 5000 (하락 추세)
    setPrices(5200, 5100, 5000);

    // 마지막 가격(5000)으로 전량 매도
    EXPECT_CALL(mock, sell(stockCode, 5000, holdCount))
        .Times(1);

    app.sell_nice_timing(stockCode, holdCount);
}

// ===================== Runtime Strategy Switch =====================

TEST(AppTest_Strategy, RuntimeSwitchChangesDecisionLogic) {
    MockBrocker mock;
    MockStrategy strategyA, strategyB;
    auto_trading_system app(&mock);

    // strategyA: 매수 결정
    app.setStrategy(&strategyA);
    EXPECT_CALL(mock, currentPrice(_, _)).Times(3).WillRepeatedly(Return(5000));
    EXPECT_CALL(strategyA, shouldBuy(_)).WillOnce(Return(true));
    EXPECT_CALL(mock, buy(_, _, _)).Times(1);
    app.buy_nice_timing("005930", 1000000);

    // 런타임에 strategyB로 교체 후 매수 거부
    app.setStrategy(&strategyB);
    EXPECT_CALL(mock, currentPrice(_, _)).Times(3).WillRepeatedly(Return(5000));
    EXPECT_CALL(strategyB, shouldBuy(_)).WillOnce(Return(false));
    EXPECT_CALL(mock, buy(_, _, _)).Times(0);
    app.buy_nice_timing("005930", 1000000);
}

// ===================== RisingTrendStrategy Unit Test =====================

TEST(RisingTrendStrategyTest, ShouldBuy_WhenAllPricesRising) {
    RisingTrendStrategy s;
    EXPECT_TRUE(s.shouldBuy({ 5000, 5100, 5200 }));
}

TEST(RisingTrendStrategyTest, ShouldNotBuy_WhenPricesFalling) {
    RisingTrendStrategy s;
    EXPECT_FALSE(s.shouldBuy({ 5200, 5100, 5000 }));
}

TEST(RisingTrendStrategyTest, ShouldNotBuy_WhenPricesFlat) {
    RisingTrendStrategy s;
    EXPECT_FALSE(s.shouldBuy({ 5000, 5000, 5000 }));
}

TEST(RisingTrendStrategyTest, ShouldSell_WhenAllPricesFalling) {
    RisingTrendStrategy s;
    EXPECT_TRUE(s.shouldSell({ 5200, 5100, 5000 }));
}

TEST(RisingTrendStrategyTest, ShouldNotSell_WhenPricesRising) {
    RisingTrendStrategy s;
    EXPECT_FALSE(s.shouldSell({ 5000, 5100, 5200 }));
}

TEST(RisingTrendStrategyTest, ShouldNotSell_WhenPricesFlat) {
    RisingTrendStrategy s;
    EXPECT_FALSE(s.shouldSell({ 5000, 5000, 5000 }));
}

// ===================== MovingAverageStrategy Test (구현 없음 - TDD Red) =====================
// 전략: p3 > avg(p1,p2,p3) → 매수 / p3 < avg(p1,p2,p3) → 매도

TEST(MovingAverageStrategyTest, ShouldBuy_WhenLatestPriceAboveAverage) {
    MovingAverageStrategy s;
    // avg(4800,5000,5200) = 5000, p3=5200 > 5000 → 매수
    EXPECT_TRUE(s.shouldBuy({ 4800, 5000, 5200 }));
}

TEST(MovingAverageStrategyTest, ShouldNotBuy_WhenLatestPriceBelowAverage) {
    MovingAverageStrategy s;
    // avg(5200,5000,4800) = 5000, p3=4800 < 5000 → 매수 안함
    EXPECT_FALSE(s.shouldBuy({ 5200, 5000, 4800 }));
}

TEST(MovingAverageStrategyTest, ShouldNotBuy_WhenLatestPriceEqualsAverage) {
    MovingAverageStrategy s;
    // avg(5000,5000,5000) = 5000, p3=5000 = 5000 → 매수 안함
    EXPECT_FALSE(s.shouldBuy({ 5000, 5000, 5000 }));
}

TEST(MovingAverageStrategyTest, ShouldSell_WhenLatestPriceBelowAverage) {
    MovingAverageStrategy s;
    // avg(5200,5000,4800) = 5000, p3=4800 < 5000 → 매도
    EXPECT_TRUE(s.shouldSell({ 5200, 5000, 4800 }));
}

TEST(MovingAverageStrategyTest, ShouldNotSell_WhenLatestPriceAboveAverage) {
    MovingAverageStrategy s;
    // avg(4800,5000,5200) = 5000, p3=5200 > 5000 → 매도 안함
    EXPECT_FALSE(s.shouldSell({ 4800, 5000, 5200 }));
}

// ===================== BreakoutStrategy Test (구현 없음 - TDD Red) =====================
// 전략: 직전 N개 고점 초과 시 매수 / 직전 N개 저점 하향 시 매도
// 인터페이스: BreakoutStrategy(int N), shouldBuy/shouldSell(const std::vector<int>& history)
//   - history 크기: N(직전) + 1(현재) = N+1
//   - 마지막 원소가 현재 가격, 앞 N개가 lookback 가격

TEST(BreakoutStrategyTest, ShouldBuy_WhenBreaksN2PeriodHigh) {
    BreakoutStrategy s(2);
    // lookback N=2: max(5000,5100)=5100, current=5200 > 5100 → 매수
    EXPECT_TRUE(s.shouldBuy({ 5000, 5100, 5200 }));
}

TEST(BreakoutStrategyTest, ShouldNotBuy_WhenBelowN2PeriodHigh) {
    BreakoutStrategy s(2);
    // lookback N=2: max(5300,5100)=5300, current=5200 < 5300 → 매수 안함
    EXPECT_FALSE(s.shouldBuy({ 5300, 5100, 5200 }));
}

TEST(BreakoutStrategyTest, ShouldNotBuy_WhenEqualsN2PeriodHigh) {
    BreakoutStrategy s(2);
    // lookback N=2: max(5000,5200)=5200, current=5200 = 5200 → 매수 안함 (초과해야)
    EXPECT_FALSE(s.shouldBuy({ 5000, 5200, 5200 }));
}

TEST(BreakoutStrategyTest, ShouldBuy_WhenBreaksN5PeriodHigh) {
    BreakoutStrategy s(5);
    // lookback N=5: max(4800,4900,5000,5100,5000)=5100, current=5200 > 5100 → 매수
    EXPECT_TRUE(s.shouldBuy({ 4800, 4900, 5000, 5100, 5000, 5200 }));
}

TEST(BreakoutStrategyTest, ShouldNotBuy_WhenBelowN5PeriodHigh) {
    BreakoutStrategy s(5);
    // lookback N=5: max(4800,4900,5300,5100,5000)=5300, current=5200 < 5300 → 매수 안함
    EXPECT_FALSE(s.shouldBuy({ 4800, 4900, 5300, 5100, 5000, 5200 }));
}

TEST(BreakoutStrategyTest, ShouldSell_WhenBreaksN2PeriodLow) {
    BreakoutStrategy s(2);
    // lookback N=2: min(5200,5100)=5100, current=5000 < 5100 → 매도
    EXPECT_TRUE(s.shouldSell({ 5200, 5100, 5000 }));
}

TEST(BreakoutStrategyTest, ShouldNotSell_WhenAboveN2PeriodLow) {
    BreakoutStrategy s(2);
    // lookback N=2: min(4900,5100)=4900, current=5000 > 4900 → 매도 안함
    EXPECT_FALSE(s.shouldSell({ 4900, 5100, 5000 }));
}

TEST(BreakoutStrategyTest, ShouldNotSell_WhenEqualsN2PeriodLow) {
    BreakoutStrategy s(2);
    // lookback N=2: min(5000,5200)=5000, current=5000 = 5000 → 매도 안함 (미만이어야)
    EXPECT_FALSE(s.shouldSell({ 5000, 5200, 5000 }));
}

TEST(BreakoutStrategyTest, ShouldSell_WhenBreaksN5PeriodLow) {
    BreakoutStrategy s(5);
    // lookback N=5: min(5200,5100,5000,5100,5200)=5000, current=4900 < 5000 → 매도
    EXPECT_TRUE(s.shouldSell({ 5200, 5100, 5000, 5100, 5200, 4900 }));
}
