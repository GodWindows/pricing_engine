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
    REQUIRE(!std::isnan(p));                            // <-- I added to make sure my if-guards actually work
    REQUIRE(p == Catch::Approx(0.0).margin(1e-9));
}

TEST_CASE("Computed value of Delta") {
    double spot = 100;
    double h = (1e-4 )* spot; // slight variation of the spot
    EuropeanOption call{100, 1.0, OptionType::Call};
    double price_at_s_plus_h = pricing::price(call, {spot + h, 0.2, 0.05}).price;
    double price_at_s_minus_h = pricing::price(call, {spot - h, 0.2, 0.05}).price;
    double variation = ((price_at_s_plus_h - price_at_s_minus_h) / (2*h));
    REQUIRE(pricing::price(call, {spot, 0.2, 0.05}).delta == Catch::Approx(variation).margin(1e-5));
}



TEST_CASE("Computed value of Gamma") {
    double spot = 100;
    double h = (1e-4 )* spot; // slight variation of the spot
    EuropeanOption call{100, 1.0, OptionType::Call};
    double price_at_s_plus_h = pricing::price(call, {spot + h, 0.2, 0.05}).price;
    double price_at_s_minus_h = pricing::price(call, {spot - h, 0.2, 0.05}).price;
    double price_at_s = pricing::price(call, {spot, 0.2, 0.05}).price;
    double variation = (price_at_s_plus_h - (2*price_at_s) + price_at_s_minus_h) / (h*h);

    REQUIRE(pricing::price(call, {spot, 0.2, 0.05}).gamma == Catch::Approx(variation).margin(1e-4));
}



TEST_CASE("Computed value of Vega") {
    double sigma = 0.2;
    double h = (1e-4 )* sigma; // slight variation of the volatility
    EuropeanOption call{100, 1.0, OptionType::Call};
    double price_at_sigma_plus_h = pricing::price(call, {100, sigma +h, 0.05}).price;
    double price_at_sigma_minus_h = pricing::price(call, {100,sigma -h, 0.05}).price;
    double variation = ((price_at_sigma_plus_h - price_at_sigma_minus_h) / (2*h));

    REQUIRE(pricing::price(call, {100, sigma, 0.05}).vega == Catch::Approx(variation).margin(1e-4));
}


TEST_CASE("Computed value of Rho") {
    double r = 0.05;
    double h = 1e-4 * r; // slight variation of the interest rate
    EuropeanOption call{100, 1.0, OptionType::Call};
    double price_at_r_plus_h  = pricing::price(call, {100, 0.2, r + h}).price;
    double price_at_r_minus_h = pricing::price(call, {100, 0.2, r - h}).price;
    double variation = (price_at_r_plus_h - price_at_r_minus_h) / (2*h);

    REQUIRE(pricing::price(call, {100, 0.2, r}).rho == Catch::Approx(variation).margin(1e-4));
}


TEST_CASE("Computed value of Theta") {
    double maturity = 1.0;
    double h = 1e-4 * maturity; // slight variation of the maturity
    EuropeanOption call{100, maturity, OptionType::Call};
    EuropeanOption call_plus{100, maturity + h, OptionType::Call};
    EuropeanOption call_minus{100, maturity - h, OptionType::Call};

    double price_at_T_plus_h  = pricing::price(call_plus,  {100, 0.2, 0.05}).price;
    double price_at_T_minus_h = pricing::price(call_minus, {100, 0.2, 0.05}).price;
    double variation = (price_at_T_plus_h - price_at_T_minus_h) / (2*h); // = ∂price/∂T

    // I added a minus on variation beacause theta = amount cause by variation of t = amount cause by variation of -T 
    REQUIRE(pricing::price(call, {100, 0.2, 0.05}).theta == Catch::Approx(-variation).margin(1e-4));
}

