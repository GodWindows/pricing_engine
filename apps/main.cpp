#include <iostream>
#include "pricing/black_scholes.hpp"

int main(int argc, char const *argv[])
{
    EuropeanOption myOption = EuropeanOption{100, 1.0, OptionType::Put} ;

    MarketData myMarket = MarketData{100, 0.2, 0.05};

    pricing::PricingResult myPrice = pricing::price(myOption, myMarket);

    std::cout<< "Le prix est  : "<< myPrice.price;

    return 0;
}
