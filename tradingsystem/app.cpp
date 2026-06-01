#pragma once
#include <string>
#include <windows.h>
#include "driver.cpp"
using std::string;

class auto_trading_system {
public:
	auto_trading_system(StockerBrocker* broker) {
		this->broker = broker;
	}
	void selectStockBrocker(StockerBrocker* broker) {
		this->broker = broker;
	}

	// 200ms 주기로 3회 가격을 읽어 상승 추세인지 판단하고,
	// 상승 추세라면 총 금액(budget)을 최대한 사용하여 최대 수량만큼,
	// 마지막에 읽은 가격으로 매수한다.
	void buy_nice_timing(string stockCode, int budget) {
		int price1 = broker->currentPrice(stockCode, 0);
		Sleep(200);
		int price2 = broker->currentPrice(stockCode, 0);
		Sleep(200);
		int price3 = broker->currentPrice(stockCode, 0);

		bool isUpTrend = (price1 < price2) && (price2 < price3);
		if (isUpTrend == false) return;

		int count = budget / price3;
		broker->buy(stockCode, price3, count);
	}

	// 200ms 주기로 3회 가격을 읽어 하락 추세인지 판단하고,
	// 하락 추세라면 사용자가 설정한 수량(count)만큼 모두,
	// 마지막에 읽은 가격으로 매도한다.
	void sell_nice_timing(string stockCode, int count) {
		int price1 = broker->currentPrice(stockCode, 0);
		Sleep(200);
		int price2 = broker->currentPrice(stockCode, 0);
		Sleep(200);
		int price3 = broker->currentPrice(stockCode, 0);

		bool isDownTrend = (price1 > price2) && (price2 > price3);
		if (isDownTrend == false) return;

		broker->sell(stockCode, price3, count);
	}

private:
	StockerBrocker* broker = nullptr;
};
