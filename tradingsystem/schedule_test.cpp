#include "gmock/gmock.h"
#include "app.cpp"
#include "driver.cpp"
#include <sstream>

using namespace testing;

class MockBrocker2 : public StockerBrocker {
public:
    MOCK_METHOD(void, login, (std::string ID, std::string pw), (override));
    MOCK_METHOD(void, buy, (std::string stockCode, int price, int count), (override));
    MOCK_METHOD(void, sell, (std::string stockCode, int price, int count), (override));
    MOCK_METHOD(int, currentPrice, (std::string stockCode, int minute), (override));
};

class AppScheduleTest : public Test {
protected:
    MockBrocker2 mock;
    auto_trading_system app{ &mock };
    std::string stockCode = "005930";
    time_t baseTime = 1000000;

    Order& MakeBuyOrder(std::string s, int budget, time_t t) {
        Order* o = new Order();
        o->stockCode_ = s;
        o->budget_ = budget;
        o->time_ = t;
        o->is_buy = true;
        return *o;
    }
    Order& MakeSellOrder(std::string s, int count, time_t t) {
        Order* o = new Order();
        o->stockCode_ = s;
        o->count_ = count;
        o->time_ = t;
        o->is_buy = false;
        return *o;
    }
};

TEST_F(AppScheduleTest, ScheduledOrderStoredInQueue) {
    app.scheduleOrder(MakeBuyOrder(stockCode, 5000, baseTime));
    EXPECT_EQ(app.getScheduledOrderCount(), 1);
}

TEST_F(AppScheduleTest, OrderNotExecutedBeforeScheduledTime) {
    app.scheduleOrder(MakeBuyOrder(stockCode, 100000, baseTime + 100));
    EXPECT_CALL(mock, buy(_, _, _)).Times(0);
    EXPECT_CALL(mock, sell(_, _, _)).Times(0);
    time_t before = baseTime + 99;
    app.processOrders(before);
    EXPECT_EQ(app.getScheduledOrderCount(), 1);
}

TEST_F(AppScheduleTest, BuyOrderExecutedAtScheduledTime) {
    EXPECT_CALL(mock, currentPrice(stockCode, 0)).WillOnce(Return(5000));
    EXPECT_CALL(mock, buy(stockCode, 5000, 20)).Times(1);
    app.scheduleOrder(MakeBuyOrder(stockCode, 100000, baseTime));
    time_t at = baseTime;
    app.processOrders(at);
    EXPECT_EQ(app.getScheduledOrderCount(), 0);
}

TEST_F(AppScheduleTest, SellOrderExecutedAtScheduledTime) {
    EXPECT_CALL(mock, currentPrice(stockCode, 0)).WillOnce(Return(5000));
    EXPECT_CALL(mock, sell(stockCode, 5000, 10)).Times(1);
    app.scheduleOrder(MakeSellOrder(stockCode, 10, baseTime));
    time_t at = baseTime;
    app.processOrders(at);
    EXPECT_EQ(app.getScheduledOrderCount(), 0);
}

TEST_F(AppScheduleTest, OrderExecutedAfterScheduledTime) {
    EXPECT_CALL(mock, currentPrice(stockCode, 0)).WillOnce(Return(5000));
    EXPECT_CALL(mock, buy(stockCode, 5000, 20)).Times(1);
    app.scheduleOrder(MakeBuyOrder(stockCode, 100000, baseTime));
    time_t after = baseTime + 500;
    app.processOrders(after);
    EXPECT_EQ(app.getScheduledOrderCount(), 0);
}

TEST_F(AppScheduleTest, MultipleOrdersExecuteAtRespectiveTimes) {
    app.scheduleOrder(MakeBuyOrder(stockCode, 100000, baseTime));
    app.scheduleOrder(MakeSellOrder(stockCode, 10, baseTime + 100));
    EXPECT_EQ(app.getScheduledOrderCount(), 2);

    EXPECT_CALL(mock, currentPrice(stockCode, 0)).WillOnce(Return(5000));
    EXPECT_CALL(mock, buy(stockCode, 5000, 20)).Times(1);
    EXPECT_CALL(mock, sell(_, _, _)).Times(0);
    time_t t1 = baseTime;
    app.processOrders(t1);
    EXPECT_EQ(app.getScheduledOrderCount(), 1);

    EXPECT_CALL(mock, currentPrice(stockCode, 0)).WillOnce(Return(5200));
    EXPECT_CALL(mock, sell(stockCode, 5200, 10)).Times(1);
    time_t t2 = baseTime + 100;
    app.processOrders(t2);
    EXPECT_EQ(app.getScheduledOrderCount(), 0);
}

