#include "tokenizer.hpp"

std::vector<std::wstring> Tokenizer::tokenize(const std::wstring &shloka)
{
    // vector to return
    std::vector<std::wstring> tokens;

    // buffer to hold the current word
    std::wstring wordBuffer;

    // loop over all characters and add each word to the tokens vector
    for (wchar_t c : shloka)
    {
        // prevent adding '|' to the tokens vector
        if (wordBuffer == L"|")
        {
            wordBuffer.clear();
            continue;
        }

        // add word to wordBuffer & add push_back at space, pipe, or newline
        if ((c == L' ' || c == L'|' || c == L'\n') && !wordBuffer.empty())
        {
            tokens.push_back(wordBuffer);
            wordBuffer.clear();
        }
        else
        {
            wordBuffer += c;
        }
    }
    if (!wordBuffer.empty())
        tokens.push_back(wordBuffer);

    return tokens;
}
