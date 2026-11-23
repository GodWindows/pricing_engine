#pragma once
#include "instrument.hpp"
#include "market.hpp"
namespace pricing
{
    // Throws std::invalid_argument if num_paths <= 0.
    double price_mc(const EuropeanOption& option, const MarketData& market, int num_paths, unsigned int seed = 42);
    
    
} // namespace pricing
