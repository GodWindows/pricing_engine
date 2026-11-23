#include <iostream>
#include "pricing/black_scholes.hpp"
#include "pricing/monte_carlo.hpp"

int main()
{
    EuropeanOption myOption{100, 1.0, OptionType::Put};
    MarketData myMarket{100, 0.2, 0.05};

    pricing::PricingResult myPrice = pricing::price(myOption, myMarket);
    double myPrice_mc = pricing::price_mc(myOption, myMarket, 100000, 42);

    std::cout << "Black-Scholes price: " << myPrice.price << '\n';
    std::cout << "Monte Carlo price:   " << myPrice_mc << '\n';

    return 0;
}
