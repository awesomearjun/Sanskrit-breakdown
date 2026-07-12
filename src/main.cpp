#include <iostream>

#include "tokenizer.hpp"

int main()
{
    Tokenizer tokenizer;
    auto tokens = tokenizer.tokenize(""
        "इमं विवस्वते योगं प्रोक्तवानहमव्ययम् |\n"
        "विवस्वान्मनवे प्राह मनुरिक्ष्वाकवेऽब्रवीत् ||\n"
    );
    for (const auto &token : tokens)
        std::cout << token << std::endl;
    return 0;
}
