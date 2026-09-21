#pragma once 
enum class OptionType{
    Call, Put
};

struct EuropeanOption{
    double K;
    double T; 
    OptionType type; 
};
