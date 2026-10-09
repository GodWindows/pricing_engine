#include "pricing/black_scholes.hpp"
#include "pricing/monte_carlo.hpp"
#include <catch2/catch_test_macros.hpp>
#include <catch2/catch_approx.hpp>
#include <cmath>

TEST_CASE("Monte Carlo converges to Black-Scholes") {
    MarketData market{100, 0.2, 0.05};
    int num_paths = 1'000'000;
    double tol = 0.05;   // my statistical tolerance, not exact

    SECTION("Call") {
        EuropeanOption call{100, 1.0, OptionType::Call};
        double bs = pricing::price(call, market).price;
        double mc = pricing::price_mc(call, market, num_paths, 42);
        REQUIRE(mc == Catch::Approx(bs).margin(tol));
    }

    SECTION("Put") {
        EuropeanOption put{100, 1.0, OptionType::Put};
        double bs = pricing::price(put, market).price;
        double mc = pricing::price_mc(put, market, num_paths, 42);
        REQUIRE(mc == Catch::Approx(bs).margin(tol));
    }
}


TEST_CASE("Monte Carlo error decreases with more paths") {
    MarketData market{100, 0.2, 0.05};
    EuropeanOption call{100, 1.0, OptionType::Call};
    double bs = pricing::price(call, market).price;

    double err_small = std::abs(pricing::price_mc(call, market, 1000,    42) - bs);
    double err_large = std::abs(pricing::price_mc(call, market, 1000000, 42) - bs);

    REQUIRE(err_large < err_small);   // more paths → smaller error
}