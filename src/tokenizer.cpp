#include "tokenizer.hpp"

std::vector<std::string> Tokenizer::tokenize(const std::string& shloka)
{
    // vector to return
    std::vector<std::string> tokens;

    // buffer to hold the current word
    std::string wordBuffer;

    // loop over all characters and add each word to the tokens vector
    for (char c : shloka)
    {
        // prevent adding '|' to the tokens vector
        if (wordBuffer == "|" || wordBuffer == "।")
        {
            wordBuffer.clear();
            continue;
        }

        // add word to wordBuffer & add push_back at space, pipe, or newline
        if ((c == ' ' || c == '|' || c == '\n') && !wordBuffer.empty())
        {
            tokens.push_back(wordBuffer);
            wordBuffer.clear();
        }
        else
        {
            wordBuffer += c;
        }
    }
    if (!wordBuffer.empty() || wordBuffer == "॥")
        tokens.push_back(wordBuffer);

    return tokens;
}
