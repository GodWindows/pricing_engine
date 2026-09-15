struct EuropeanOption{
    double K;
    double T; 
    OptionType type; 
};

enum class OptionType{
    Call, Put
};