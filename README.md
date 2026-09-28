# Options Pricing Engine (C++/Python)

European option pricing engine based on the Black-Scholes model.
Written in modern C++17, with unit tests (Catch2) and Python bindings (pybind11, in progress).

Computes the price of European calls and puts, together with the full set of
first-order Greeks (delta, gamma, vega, theta, rho), all from closed-form formulas.

## Requirements
- CMake ≥ 3.15
- A C++17 compiler
- Catch2 (fetched automatically by CMake)

## Build & run tests
​```bash
cmake -S . -B build
cmake --build build
ctest --test-dir build
​```

## Usage
​```cpp
EuropeanOption call{100, 1.0, OptionType::Call};  // strike, maturity ( in years), type
MarketData market{100, 0.2, 0.05};                // spot, volatility, rate

PricingResult res = pricing::price(call, market);
double price = res.price;
double delta = res.delta;   // also: gamma, vega, theta, rho
​```

## Project structure
​```
include/pricing/   public headers (API)
src/               implementation
tests/             unit tests (Catch2)
apps/              demo executable
​```

## Testing approach
- Reference values (canonical Black-Scholes cases)
- Call-put parity across multiple inputs
- Limit cases (T = 0, sigma = 0) with no NaN
- Greeks validated against finite differences

## The model's assumptions & limitations :
- Constant volatility and interest rate
- No dividends
- European exercise only (no early exercise)
- Theta is expressed as the derivative with respect to calendar time (time decay), so it is negative for standard options