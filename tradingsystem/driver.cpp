#pragma once
#define interface struct

interface StockerBrocker{
	virtual void buy() = 0;
	virtual void sell() = 0;
	virtual void currentPrice() = 0;
	virtual void login() = 0;
};