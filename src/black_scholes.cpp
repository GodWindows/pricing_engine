#include "pricing/black_scholes.hpp"
#include <cmath>
#include <algorithm>
constexpr double reciprocal_sqrt_2 = 0.70710678118654752440;
constexpr double reciprocal_sqrt_2pi = 0.39894228040143267794;
namespace{
    double normal_cdf(double x)
    {
        return 0.5 * (1.0 + std::erf(x * reciprocal_sqrt_2));
    }

    double normal_pdf(double x) {
        return reciprocal_sqrt_2pi * std::exp(-0.5 * x * x);
    }
}

namespace pricing
{
    PricingResult price(const EuropeanOption &option, const MarketData &market)
    {
        double price, delta, gamma, vega, theta, rho;
        double discount = std::exp(-market.r * option.T);  // e^(−rT)
        theta = rho = 0;

        if (option.T<=0) // if the option's maturity is zero, the price must be equal to the the instant payoff
        {
            if (option.type==OptionType::Call)
            {
                price = std::max(market.S - option.K, 0.00);
            }
            else
            {
                price = std::max(option.K - market.S, 0.00);
            }  
            delta = gamma = vega = theta = rho = 0;
        }else if(market.sigma<=0) //if there is no volatility, we apply the interest rates to the strike.
        {
            if (option.type==OptionType::Call)
            {
                price = std::max(market.S - option.K*discount, 0.00);
            }
            else
            {
                price = std::max(option.K*discount - market.S, 0.00);
            } 
            delta = gamma = vega = 0;
        }else
        {
            double vol_sqrt_T = market.sigma * std::sqrt(option.T);
            double d1 = (std::log(market.S / option.K) + ((market.r + ((market.sigma * market.sigma) / 2))* option.T))/ vol_sqrt_T;
            double d2 = d1 - vol_sqrt_T;
            gamma = normal_pdf(d1) / (market.S * vol_sqrt_T);
            vega = market.S * normal_pdf(d1) * std::sqrt(option.T);
            double common_term_for_theta = -(market.S * normal_pdf(d1) * market.sigma) / (2 * std::sqrt(option.T));
            if(option.type == OptionType::Call){
                price = market.S * normal_cdf(d1) - option.K * discount * normal_cdf(d2);
                delta = normal_cdf(d1);
                rho = option.K * option.T * discount * normal_cdf(d2);
                theta = common_term_for_theta - market.r * option.K * discount * normal_cdf(d2);
            }
            else{
                price = option.K * discount * normal_cdf(-d2) - market.S * normal_cdf(-d1);
                delta = normal_cdf(d1) - 1.00;
                rho = -option.K * option.T * discount * normal_cdf(-d2);
                theta = common_term_for_theta + market.r * option.K * discount * normal_cdf(-d2);
            }
        }
        
        PricingResult res = PricingResult{price, delta, gamma, vega, theta, rho};
        return res;
    }

}