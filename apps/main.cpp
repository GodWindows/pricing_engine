#include <iostream>
#include "pricing/black_scholes.hpp"
#include "pricing/monte_carlo.hpp"

int main(int argc, char const *argv[])
{
    EuropeanOption myOption = EuropeanOption{100, 1.0, OptionType::Put} ;

    MarketData myMarket = MarketData{100, 0.2, 0.05};

    pricing::PricingResult myPrice = pricing::price(myOption, myMarket);

    double myPrice_mc = pricing::price_mc(myOption, myMarket, 10000, 42);

    std::cout<< "Le prix est  : "<< myPrice.price << std::endl;

    std::cout<< "Le prix est  : "<< myPrice_mc;

    return 0;
}
