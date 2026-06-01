#pragma once
#include <string>
#include <vector>
#include <numeric>
#include <algorithm>
#include "driver.cpp"
using std::string;

using PriceHistory = std::vector<int>;

class ITimingStrategy {
public:
    virtual bool shouldBuy(const PriceHistory& prices) = 0;
    virtual bool shouldSell(const PriceHistory& prices) = 0;
    virtual int requiredHistory() const { return 3; }
    virtual ~ITimingStrategy() = default;
};

class RisingTrendStrategy : public ITimingStrategy {
public:
    bool shouldBuy(const PriceHistory& prices) override {
        return prices[0] < prices[1] && prices[1] < prices[2];
    }
    bool shouldSell(const PriceHistory& prices) override {
        return prices[0] > prices[1] && prices[1] > prices[2];
    }
};

class MovingAverageStrategy : public ITimingStrategy {
public:
    // prices.back() > average(prices) 이면 매수
    bool shouldBuy(const PriceHistory& prices) override {
        return prices.back() > average(prices);
    }
    // prices.back() < average(prices) 이면 매도
    bool shouldSell(const PriceHistory& prices) override {
        return prices.back() < average(prices);
    }
private:
    double average(const PriceHistory& prices) const {
        long long sum = std::accumulate(prices.begin(), prices.end(), 0LL);
        return static_cast<double>(sum) / prices.size();
    }
};

class BreakoutStrategy : public ITimingStrategy {
    int n;
public:
    BreakoutStrategy(int n) : n(n) {}
    int requiredHistory() const override { return n + 1; }
    // prices.back() > max(prices[0..n-1]) 이면 매수
    bool shouldBuy(const PriceHistory& prices) override {
        int highest = *std::max_element(prices.begin(), prices.begin() + n);
        return prices.back() > highest;
    }
    // prices.back() < min(prices[0..n-1]) 이면 매도
    bool shouldSell(const PriceHistory& prices) override {
        int lowest = *std::min_element(prices.begin(), prices.begin() + n);
        return prices.back() < lowest;
    }
};

class auto_trading_system {
public:
    auto_trading_system(StockerBrocker* broker)
        : broker(broker), strategy(&defaultStrategy) {}

    void selectStockBrocker(StockerBrocker* broker) {
        this->broker = broker;
    }

    void setStrategy(ITimingStrategy* s) {
        strategy = s;
    }

    void buy_nice_timing(string stockCode, int budget) {
        auto prices = readPrices(stockCode);
        if (!strategy->shouldBuy(prices)) return;
        broker->buy(stockCode, prices.back(), budget / prices.back());
    }

    void sell_nice_timing(string stockCode, int count) {
        auto prices = readPrices(stockCode);
        if (!strategy->shouldSell(prices)) return;
        broker->sell(stockCode, prices.back(), count);
    }

private:
    StockerBrocker* broker;
    RisingTrendStrategy defaultStrategy;
    ITimingStrategy* strategy;

    PriceHistory readPrices(const string& stockCode) {
        PriceHistory prices;
        for (int i = 0; i < strategy->requiredHistory(); ++i)
            prices.push_back(broker->currentPrice(stockCode, 200));
        return prices;
    }
};
