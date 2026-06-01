#pragma once
#include <iostream>
#include <string>
#include <array>
#include <queue>
#include "driver.cpp"

using std::string, std::priority_queue, std::pair;

class Order {
public:
    string stockCode_;
    int count_;
    int budget_;
    time_t time_;
    bool is_buy;

    bool operator<(const Order* other) const {
        if (this->time_ < other->time_) return true;
        else return false;
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

        int getScheduledOrderCount() {
            return 0;
        }

        void scheduleOrder(Order* order) {
            reserved_order.push(order);
        }

        void processOrders(time_t currentTime) {

            while (reserved_order.top()->time_ <= currentTime) {
                time_t reserved_time = reserved_order.top()->time_;
                Order* order = reserved_order.top();

                int price = readPrice(order->stockCode_, 200);

                if (order->is_buy) {
                    broker->buy(order->stockCode_, price, order->budget_ / price);
                    std::cout << "[자동 주문 : " << order->stockCode_ << " ] " << order->budget_ / price << "개 매수 완료 되었습니다." << std::endl;
                }
                else {
                    broker->sell(order->stockCode_, price, order->count_);
                    std::cout << "[자동 주문 : " << order->stockCode_ << " ] " << order->budget_ / price << "개 매수 완료 되었습니다." << std::endl;
                }

                reserved_order.pop();
            }
        }

    private:
        StockerBrocker* broker = nullptr;
        priority_queue<Order*> reserved_order;

        std::array<int, 3> readPrices(const string& stockCode) {
            return {
                readPrice(stockCode, 200),
                readPrice(stockCode, 200),
                readPrice(stockCode, 200)
            };
        }

        int readPrice(const string& stockCode, int minutes) {
            return broker->currentPrice(stockCode, minutes);
        }

        bool isUpTrend(const std::array<int, 3>& prices) {
            return prices[0] < prices[1] && prices[1] < prices[2];
        }

        bool isDownTrend(const std::array<int, 3>& prices) {
            return prices[0] > prices[1] && prices[1] > prices[2];
        }
    };
