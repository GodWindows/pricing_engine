#include "instrument.hpp"
#include "market.hpp"
namespace pricing
{
    double price_mc(const EuropeanOption& option, const MarketData& market, int num_paths, unsigned int seed = 42);
    
} // namespace pricing
