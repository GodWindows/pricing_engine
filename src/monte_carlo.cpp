#include "pricing/monte_carlo.hpp"
#include <algorithm>
#include <cmath>
#include <random>
#include <stdexcept>

namespace pricing {
    double price_mc(const EuropeanOption& option,
                    const MarketData& market,
                    int num_paths,
                    unsigned int seed)
    {
        if (num_paths <= 0)
        {
            throw std::invalid_argument("price_mc: num_paths must be positive");
        }

        std::mt19937 gen(seed);
        std::normal_distribution<double> norm(0.0, 1.0);

        // precomputing the constants
        const double drift = (market.r - (market.sigma*market.sigma)/2)*option.T;  //(r - sigma^2/2) * T
        const double diffusion = market.sigma * std::sqrt(option.T);

        // compute the payoff num_paths time 
        double payoffSum = 0.0;
        for (int i = 0; i < num_paths; ++i)
        {    
            double Z = norm(gen);
            double S_T = market.S * std::exp(drift + diffusion*Z);
            double payoff;
            if (option.type==OptionType::Call)
            {
                payoff = std::max(S_T - option.K, 0.0) ;
            }else
            {
                payoff = std::max(option.K - S_T, 0.0) ;
            }
            payoffSum += payoff;
        }
        double avgPayoff = payoffSum/num_paths;
        double price = avgPayoff * std::exp(-market.r*option.T); // e^(-rT) * average
        
        return price;
    }
}
