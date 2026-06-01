#pragma once
#include <string>
#include <vector>
#include <numeric>
#include <algorithm>
#include "driver.cpp"
#include <iostream>
#include <array>
#include <queue>

using std::string, std::priority_queue;

class Order {
public:
    string stockCode_;
    int count_;
    int budget_;
    time_t time_;
    bool is_buy;

    bool operator<(const Order other) const {
        return time_ > other.time_;
    };
};

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
    // prices.back() > average(prices) ì´ë©´ ë§¤ìˆ˜
    bool shouldBuy(const PriceHistory& prices) override {
        return prices.back() > average(prices);
    }
    // prices.back() < average(prices) ì´ë©´ ë§¤ë„
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
    // prices.back() > max(prices[0..n-1]) ì´ë©´ ë§¤ìˆ˜
    bool shouldBuy(const PriceHistory& prices) override {
        int highest = *std::max_element(prices.begin(), prices.begin() + n);
        return prices.back() > highest;
    }
    // prices.back() < min(prices[0..n-1]) ì´ë©´ ë§¤ë„
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

    void scheduleOrder(Order order) {
        reserved_order.push(order);
    }

    void processOrders(time_t currentTime) {

        while (!reserved_order.empty() && reserved_order.top().time_ <= currentTime) {
            time_t reserved_time = reserved_order.top().time_;
            Order order = reserved_order.top();

            int price = readPrice(order.stockCode_, currentTime);

            if (order.is_buy) {
                broker->buy(order.stockCode_, price, order.budget_ / price);
                std::cout << "[ÀÚµ¿ ÁÖ¹® : " << order.stockCode_ << " ] " << order.budget_ / price << "°³ ¸Å¼ö ¿Ï·á µÇ¾ú½À´Ï´Ù." << std::endl;
            }
            else {
                broker->sell(order.stockCode_, price, order.count_);
                std::cout << "[ÀÚµ¿ ÁÖ¹® : " << order.stockCode_ << " ] " << order.budget_ / price << "°³ ¸Åµµ ¿Ï·á µÇ¾ú½À´Ï´Ù." << std::endl;
            }

            reserved_order.pop();
        }
    }

    int getScheduledOrderCount() {
        return reserved_order.size();
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

    priority_queue<Order> reserved_order;
    int readPrice(const string& stockCode, int minutes) {
        return broker->currentPrice(stockCode, minutes);
    }
};
