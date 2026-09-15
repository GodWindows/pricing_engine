#include "black_scholes.hpp"
#include <cmath>

using namespace std;

PricingResult price(const EuropeanOption& option, const MarketData& market){
    double d1, d2, price;
    d1 = (log(market.S/option.K) + (market.r + (pow(market.sigma,2)/2)*option.T);

    PricingResult res =  PricingResult{price, 0,0,0,0,0};
}