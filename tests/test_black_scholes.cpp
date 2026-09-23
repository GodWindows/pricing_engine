#include "pricing/black_scholes.hpp"
#include <catch2/catch_test_macros.hpp>
#include <catch2/catch_approx.hpp>
#include <cmath>
TEST_CASE("Black-Scholes reference values") {
    EuropeanOption call{100, 1.0, OptionType::Call};
    EuropeanOption put{100, 1.0, OptionType::Put};
    MarketData market{100, 0.2, 0.05};
    REQUIRE(pricing::price(call, market).price == Catch::Approx(10.4506).margin(1e-4));
    REQUIRE(pricing::price(put, market).price == Catch::Approx(5.5736).margin(1e-4));
}

static void check_parity(const MarketData& market, double K, double T) {
    EuropeanOption call{K, T, OptionType::Call};
    EuropeanOption put{K, T, OptionType::Put};
    double diff = pricing::price(call, market).price - pricing::price(put, market).price;
    double expected = market.S - K * std::exp(-market.r * T);  // the value of S − K·e^(−rT)
    REQUIRE(diff == Catch::Approx(expected).margin(1e-4));// the parity : C − P == S − K·e^(−rT)
}

TEST_CASE("Call-Put parity across inputs") {
    check_parity(MarketData{100, 0.2, 0.05}, 100, 1.0);   // at the money
    check_parity(MarketData{120, 0.2, 0.05}, 100, 1.0);   // in the money
    check_parity(MarketData{80,  0.2, 0.05}, 100, 1.0);   // out of the money
    check_parity(MarketData{100, 0.5, 0.05}, 100, 2.0);   // high vol and maturity
    check_parity(MarketData{100, 0.1, 0.01}, 100, 0.25);  // short maturity, low rate
    check_parity(MarketData{100, 0.2, 0.0},  90,  1.0);   // zero rate, strike != spot
}

TEST_CASE("Possible edge case: zero maturity returns intrinsic payoff") {
    MarketData market{120, 0.2, 0.05};
    EuropeanOption call{100, 0.0, OptionType::Call};
    REQUIRE(pricing::price(call, market).price == Catch::Approx(20.0).margin(1e-9)); // max(120-100,0)

    EuropeanOption put{100, 0.0, OptionType::Put};
    REQUIRE(pricing::price(put, market).price == Catch::Approx(0.0).margin(1e-9));   // max(100-120,0)
}

TEST_CASE("Possible edge case: zero volatility returns discounted intrinsic") {
    MarketData market{100, 0.0, 0.05};
    EuropeanOption call{90, 1.0, OptionType::Call};
    double fwd = 90 * std::exp(-0.05 * 1.0);
    REQUIRE(pricing::price(call, market).price == Catch::Approx(100 - fwd).margin(1e-9));
}

TEST_CASE("Deep in/out of the money") {
    MarketData deepITM{200, 0.2, 0.05};
    EuropeanOption call{100, 1.0, OptionType::Call};
    double fwd = 100 * std::exp(-0.05 * 1.0);
    REQUIRE(pricing::price(call, deepITM).price == Catch::Approx(200 - fwd).margin(0.5)); // ≈ S − K·e^(−rT)

    MarketData deepOTM{50, 0.2, 0.05};
    REQUIRE(pricing::price(call, deepOTM).price == Catch::Approx(0.0).margin(0.5));
}

TEST_CASE("Zero maturity at the money is not nan") {
    MarketData market{100, 0.2, 0.05};
    EuropeanOption call{100, 0.0, OptionType::Call};   // S == K, T == 0
    double p = pricing::price(call, market).price;
    REQUIRE(!std::isnan(p));                            // <-- échoue sans court-circuit
    REQUIRE(p == Catch::Approx(0.0).margin(1e-9));
}