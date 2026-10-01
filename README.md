# Options Pricing Engine (C++/Python)

European option pricing engine based on the Black-Scholes model.
Written in modern C++17, with unit tests (Catch2) and Python bindings (pybind11).

Computes the price of European calls and puts, together with the full set of
first-order Greeks (delta, gamma, vega, theta, rho), all from closed-form formulas.

## Requirements
- CMake ≥ 3.15
- A C++17 compiler
- Catch2 and pybind11 (fetched automatically by CMake)
- Python 3 (for the bindings and the demo notebook)

## Build & run tests
```bash
cmake -S . -B build
cmake --build build
ctest --test-dir build
```

## Usage (C++)
```cpp
EuropeanOption call{100, 1.0, OptionType::Call};  // strike, maturity (in years), type
MarketData market{100, 0.2, 0.05};                // spot, volatility, rate

PricingResult res = pricing::price(call, market);
double price = res.price;
double delta = res.delta;   // also: gamma, vega, theta, rho
```

## Usage (Python)
```python
import pricing_engine as pe

call = pe.EuropeanOption(100, 1.0, pe.OptionType.Call)
market = pe.MarketData(100, 0.2, 0.05)
res = pe.price(call, market)
print(res.price, res.delta)   # price and Greeks
```
See `notebooks/demo.ipynb` for a worked example plotting price and delta vs spot.

## Project structure
```
include/pricing/   public headers (API): instrument, market, black_scholes
src/               implementation
tests/             unit tests (Catch2)
apps/              demo executable
python/            pybind11 bindings
notebooks/         Python demo (price & delta vs spot)
```

## Design notes

**Separation of concerns.** Inputs are split into three structs mirroring the
three parts of a pricing problem: the *product* (`EuropeanOption`: strike,
maturity, type), the *market state* (`MarketData`: spot, volatility, rate),
and the *model* (the Black-Scholes formula itself). This keeps the product
description independent from the market data, so a second engine (e.g. Monte
Carlo) can consume the exact same inputs without changing the interface.

**Single entry point returning price and Greeks together.** `price()` returns
a `PricingResult` struct bundling the price and all five Greeks, rather than
five separate calls. The Greeks share intermediate quantities (d1, d2) with
the price, so computing them together avoids recomputation and keeps the API
minimal.

**Closed-form Greeks.** All Greeks are computed analytically (not by finite
differences), which is exact and fast. Finite differences are used only in the
tests, as an independent cross-check of the analytical formulas.

**Edge-case guards.** The degenerate inputs T = 0 and sigma = 0 make the
Black-Scholes formula divide by zero (d1 involves division by sigma*sqrt(T)).
These are short-circuited at the top of `price()` to return the intrinsic
(or discounted intrinsic) payoff directly, so the engine never returns NaN.

**Theta sign convention.** Theta is expressed as the derivative with respect
to calendar time (time decay), so it is negative for standard options. The
finite-difference test negates the derivative w.r.t. maturity T accordingly.

## Testing approach
The engine is validated against several independent checks:
- **Reference values** : two independent parameter sets, cross-checked by hand
- **Put-call parity** : C − P = S − K·e^(−rT), across multiple inputs
- **No-arbitrage bounds** : price ≥ 0, call ≤ S, put ≤ K·e^(−rT), lower bound
- **Monotonicity** : price increases/decreases with spot and volatility as expected
- **Greeks vs finite differences** — all five Greeks, for both call and put
- **Greeks theoretical bounds** : delta ranges, gamma ≥ 0, vega ≥ 0
- **Limit cases** : T = 0 and sigma = 0 return intrinsic payoff with no NaN

## Model assumptions & limitations
- Constant volatility and interest rate (no volatility smile, no rate curve)
- No dividends
- European exercise only (no early exercise)
- Log-normal price dynamics (no jumps, continuous trading assumed)

