# Options Pricing Engine (C++17 / Python)

**A fast, rigorously validated options pricing library: C++17 at the core, with first-class Python access.**

This engine gives you **two independent pricing engines** behind **one shared API**:

- **Analytical Black-Scholes engine.** Exact closed-form prices for European calls and puts, plus the **full set of first-order Greeks** (Δ delta, Γ gamma, ν vega, Θ theta, ρ rho), all computed analytically in a single call.
- **Monte Carlo engine.** Risk-neutral simulation under geometric Brownian motion, **cross-validated against the closed-form prices** and fully reproducible thanks to seeded RNG.
- **Python bindings (pybind11).** The same C++ performance, available from Python in one `import`.
- **19 test cases, all passing.** They cover reference values, put-call parity, no-arbitrage bounds, monotonicity, finite-difference Greek checks, edge cases and Monte Carlo convergence.

It's built the way production quant libraries are built: a clean product/market/model separation, a minimal API, numerical robustness on degenerate inputs, and a test suite grounded in financial theory, not just "does it run."

---

## Highlights

| Feature | What you get |
|---|---|
| **Closed-form pricing** | Exact Black-Scholes prices: no approximation, no iteration |
| **Analytical Greeks** | All five first-order Greeks, exact and computed in one pass alongside the price |
| **Monte Carlo engine** | Risk-neutral GBM simulation, with the loop invariants hoisted out of the hot loop |
| **Unified interface** | Both engines consume the *exact same* `EuropeanOption` and `MarketData` inputs |
| **Robust by design** | T = 0 and σ = 0 are handled explicitly, so you never get NaN |
| **Reproducible** | Seeded `std::mt19937` gives deterministic Monte Carlo results, run after run |
| **Python ready** | pybind11 module plus a Jupyter demo notebook |
| **Zero-hassle build** | CMake fetches Catch2 and pybind11 automatically |

---

## Requirements
- CMake ≥ 3.15
- A C++17 compiler
- Catch2 and pybind11 (**fetched automatically** by CMake, nothing to install)
- Python 3 (for the bindings and the demo notebook)

## Build & run tests
Three commands take you from clone to a fully tested build:
```bash
cmake -S . -B build
cmake --build build
ctest --test-dir build
```
```
100% tests passed out of 19
```

## Usage (C++)
```cpp
#include "pricing/black_scholes.hpp"
#include "pricing/monte_carlo.hpp"

EuropeanOption call{100, 1.0, OptionType::Call};  // strike, maturity (in years), type
MarketData market{100, 0.2, 0.05};                // spot, volatility, rate

// Closed-form: price + all Greeks in a single call
pricing::PricingResult res = pricing::price(call, market);
double price = res.price;
double delta = res.delta;   // also: gamma, vega, theta, rho

// Monte Carlo: same inputs, independent engine
double mc_price = pricing::price_mc(call, market, 100000 /* paths */, 42 /* seed */);
```

## Usage (Python)
```python
import pricing_engine as pe

call = pe.EuropeanOption(100, 1.0, pe.OptionType.Call)
market = pe.MarketData(100, 0.2, 0.05)
res = pe.price(call, market)
print(res.price, res.delta)   # price and Greeks
```
See `notebooks/demo.ipynb` for a worked example that plots price and delta against spot.

## Project structure
```
include/pricing/   public headers (API): instrument, market, black_scholes, monte_carlo
src/               implementation (closed-form + Monte Carlo engines)
tests/             unit tests (Catch2): 19 test cases
apps/              demo executable
python/            pybind11 bindings
notebooks/         Python demo (price & delta vs spot)
```

---

## Design notes

**Clean separation of concerns.** Inputs mirror the three parts of any pricing
problem: the *product* (`EuropeanOption`: strike, maturity, type), the
*market state* (`MarketData`: spot, volatility, rate) and the *model*. Because
product and market data are decoupled, new engines plug in without touching
the interface. The Monte Carlo engine already proves this.

**One call, price and every Greek.** `price()` returns a `PricingResult`
bundling the price and all five Greeks. The Greeks share intermediate
quantities (d1, d2, σ√T, e^(−rT)) with the price, so these are computed once
and reused, and the API stays small and easy to use.

**Exact analytical Greeks.** Every Greek comes from its closed-form
expression, never from finite differences, so it is exact and fast. Finite
differences appear only in the test suite, as an *independent* cross-check
of the analytics.

**Numerically bulletproof edge cases.** The degenerate inputs T = 0 and σ = 0
would make a naive implementation divide by zero (d1 divides by σ√T). This
engine catches both up front and returns the exact intrinsic (or discounted
intrinsic) payoff, so it **never returns NaN**.

**Practitioner theta convention.** Theta is quoted as the derivative with
respect to calendar time (time decay), the convention traders actually use,
so it is negative for standard long options.

**Two engines, one interface.** The Monte Carlo engine (`price_mc`) consumes
the *exact same* `EuropeanOption` and `MarketData` structs as the closed-form
engine. It simulates under the risk-neutral measure:

S_T = S · exp((r − σ²/2)·T + σ·√T·Z),  Z ~ N(0, 1)

and averages the discounted payoff over N paths. Drift, diffusion and
discount factors are precomputed outside the simulation loop for speed, and
the seeded `std::mt19937` makes every run reproducible and every test
deterministic. This simulation core is the foundation for path-dependent and
exotic payoffs.

---

## Testing approach: validated from every angle
The engine is checked against financial theory, not just against itself:
- **Reference values:** two independent parameter sets, cross-checked by hand
- **Put-call parity:** C − P = S − K·e^(−rT), across multiple inputs
- **No-arbitrage bounds:** price ≥ 0, call ≤ S, put ≤ K·e^(−rT), plus lower bounds
- **Monotonicity:** prices move correctly with spot and volatility
- **Greeks vs finite differences:** all five Greeks, for both calls and puts
- **Theoretical Greek bounds:** delta ranges, gamma ≥ 0, vega ≥ 0
- **Limit cases:** T = 0 and σ = 0 return the intrinsic payoff, NaN-free
- **Monte Carlo cross-validation:** the MC price converges to Black-Scholes (calls and puts)
- **Convergence property:** the MC error shrinks as the number of paths grows

---

## Model scope
The engine implements the classic Black-Scholes framework: European exercise,
log-normal dynamics, constant volatility and rate. This is the industry's
reference model and the benchmark every other model is measured against.

## Roadmap
The architecture is designed to grow. Natural next steps include:
- Monte Carlo Greeks (pathwise / likelihood-ratio estimators)
- Path-dependent and exotic payoffs (Asian, barrier, lookback) on the existing MC core
- Dividend yield support
- Exposing the Monte Carlo engine through the Python bindings
- Variance reduction (antithetic variates, control variates)
- American exercise (binomial trees / Longstaff-Schwartz)
