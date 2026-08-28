#pragma once

#include "wordTypes.hpp"
#include <memory>
#include <string>
#include <vector>

/// Takes a shloka and converts it to a vector of strings for each sandhi'd word
class Tokenizer
{
public:
    Tokenizer() = default;
    ~Tokenizer() = default;
    WordList tokenize(const std::string &shloka);
};
