#include "instrument.hpp"
#include "market.hpp"

struct PricingResult{
    double price;
    double delta;
    double gamma;
    double vega;
    double theta;
    double rho;
};

PricingResult price(const EuropeanOption& option, const MarketData& market);