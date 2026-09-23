#include "pricing/black_scholes.hpp"
#include <cmath>
#include <algorithm>
constexpr double reciprocal_sqrt_2 = 0.70710678118654752440;
namespace{
    double normal_cdf(double x)
    {
        return 0.5 * (1.0 + std::erf(x * reciprocal_sqrt_2));
    }
}
/*
L'idée, pas le code complet — tu l'écris :

si option.T <= 0 : calcule le payoff intrinsèque selon type et retourne un PricingResult avec ce prix (Greeks à 0).
sinon si market.sigma <= 0 : calcule le forward actualisé K·e^(−rT), puis le payoff max(S − K·e^(−rT), 0) (call) ou l'inverse (put), et retourne.
sinon : ton calcul normal actuel.

Attention à un piège : max sur des double → utilise std::max, et pense à inclure <algorithm>.
*/
namespace pricing
{
    PricingResult price(const EuropeanOption &option, const MarketData &market)
    {
        double price;
        double discount = std::exp(-market.r * option.T);  // e^(−rT)

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
            //all the greeks should also be zero
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
        }else
        {
            double vol_sqrt_T = market.sigma * std::sqrt(option.T);
            double d1 = (std::log(market.S / option.K) + ((market.r + ((market.sigma * market.sigma) / 2))* option.T))/ vol_sqrt_T;
            double d2 = d1 - vol_sqrt_T;
            if(option.type == OptionType::Call){
                price = market.S * normal_cdf(d1) - option.K * discount * normal_cdf(d2);
            }
            else{
                price = option.K * discount * normal_cdf(-d2) - market.S * normal_cdf(-d1);
            }
        }
        
        
        
        PricingResult res = PricingResult{price, 0, 0, 0, 0, 0};
        return res;
    }

}