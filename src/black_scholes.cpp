#include "../include/pricing/black_scholes.hpp"
#include <cmath>

constexpr double reciprocal_sqrt_2 = 0.70710678118654752440;
namespace{
    double normal_cdf(double x)
    {
        return 0.5 * (1.0 + std::erf(x * reciprocal_sqrt_2));
    }
}

namespace pricing
{
    PricingResult price(const EuropeanOption &option, const MarketData &market)
    {
        double d1, d2, price;
        if(option.type == OptionType::Call){
            double vol_sqrt_T = market.sigma * std::sqrt(option.T);
            d1 = (std::log(market.S / option.K) + ((market.r + ((market.sigma * market.sigma) / 2))* option.T))/ vol_sqrt_T;

            d2 = d1 - vol_sqrt_T;
            price = market.S * normal_cdf(d1) - option.K * (std::exp(-1*(market.r) * option.T)) * normal_cdf(d2);

            PricingResult res = PricingResult{price, 0, 0, 0, 0, 0};
            return res;
        }
        else{
            return PricingResult{0, 0, 0, 0, 0, 0};
        }
    }

}