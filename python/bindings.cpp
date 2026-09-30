#include <pybind11/pybind11.h>
#include "pricing/black_scholes.hpp"

namespace py = pybind11;
using namespace pricing;

PYBIND11_MODULE(pricing_engine, m) {
    m.doc() = "European option pricing engine (Black-Scholes)";

    py::enum_<OptionType>(m, "OptionType")
        .value("Call", OptionType::Call)
        .value("Put", OptionType::Put);

    py::class_<EuropeanOption>(m, "EuropeanOption")
        .def(py::init<double, double, OptionType>(),
             py::arg("K"), py::arg("T"), py::arg("type"))
        .def_readwrite("K", &EuropeanOption::K)
        .def_readwrite("T", &EuropeanOption::T)
        .def_readwrite("type", &EuropeanOption::type);

    py::class_<MarketData>(m, "MarketData")
        .def(py::init<double, double, double>(),
             py::arg("S"), py::arg("sigma"), py::arg("r"))
        .def_readwrite("S", &MarketData::S)
        .def_readwrite("sigma", &MarketData::sigma)
        .def_readwrite("r", &MarketData::r);

    py::class_<PricingResult>(m, "PricingResult")
        .def_readonly("price", &PricingResult::price)
        .def_readonly("delta", &PricingResult::delta)
        .def_readonly("gamma", &PricingResult::gamma)
        .def_readonly("vega",  &PricingResult::vega)
        .def_readonly("theta", &PricingResult::theta)
        .def_readonly("rho",   &PricingResult::rho);

    m.def("price", &price, py::arg("option"), py::arg("market"),
          "Price a European option and return price + Greeks");
}