#pragma once
#include <string>
#include <array>
#include "driver.cpp"
using std::string;

class Order {
    string stockCode_;
    int count_;
    int budget_;
    time_t time_;
    bool is_buy;
};

class auto_trading_system {
public:
    auto_trading_system(StockerBrocker* broker) : broker(broker) {}

    void selectStockBrocker(StockerBrocker* broker) {
        this->broker = broker;
    }

    void buy_nice_timing(string stockCode, int budget) {
        auto prices = readPrices(stockCode);
        if (!isUpTrend(prices)) return;
        broker->buy(stockCode, prices[2], budget / prices[2]);
    }

    void sell_nice_timing(string stockCode, int count) {
        auto prices = readPrices(stockCode);
        if (!isDownTrend(prices)) return;
        broker->sell(stockCode, prices[2], count);
    }

    void scheduleOrder(Order& order) {
    }

private:
    StockerBrocker* broker = nullptr;

    std::array<int, 3> readPrices(const string& stockCode) {
        return {
            broker->currentPrice(stockCode, 200),
            broker->currentPrice(stockCode, 200),
            broker->currentPrice(stockCode, 200)
        };
    }

    bool isUpTrend(const std::array<int, 3>& prices) {
        return prices[0] < prices[1] && prices[1] < prices[2];
    }

    bool isDownTrend(const std::array<int, 3>& prices) {
        return prices[0] > prices[1] && prices[1] > prices[2];
    }
};
