# Options Pricing Engine (C++/Python)

European option pricing engine based on the Black-Scholes model.
Written in modern C++17 with unit tests (Catch2).

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
EuropeanOption call{100, 1.0, OptionType::Call};
MarketData market{100, 0.2, 0.05};
double price = pricing::price(call, market).price;
​```

## Model assumptions & limitations
- Constant volatility and interest rate
- No dividends
- European exercise only (no early exercise)