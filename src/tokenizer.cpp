#include "tokenizer.hpp"

std::string trim(std::string s)
{
    auto not_space = [](unsigned char ch) { return !std::isspace(ch); };

    auto begin = std::find_if(s.begin(), s.end(), not_space);
    auto end = std::find_if(s.rbegin(), s.rend(), not_space).base();

    return (begin < end) ? std::string(begin, end) : std::string{};
}

void sanitize(std::string &token)
{
    token = trim(token);

    // Drop empty tokens completely
    if (token.empty())
        return;

    // delete delimeters
    std::vector<std::string> delimiters = {",", ".", "।", "॥", "|"};
    for (const auto &delimiter : delimiters)
    {
        size_t pos = token.find(delimiter);
        while (pos != std::string::npos)
        {
            token.erase(pos, delimiter.length());
            pos = token.find(delimiter);
        }
    }

    // Map common keyboard hacks to the true Devanagari Avagraha 'ऽ'
    std::vector<std::string> avagrahaHacks = {"S", "$", "'"};
    for (const auto &hack : avagrahaHacks)
    {
        size_t pos = token.find(hack);
        while (pos != std::string::npos)
        {
            token.replace(pos, hack.length(), "ऽ");
            pos = token.find(hack, pos + 3);
        }
    }

    // Replace English colon ':' with Devanagari Visarga 'ः'
    size_t pos = token.find(":");
    while (pos != std::string::npos)
    {
        token.replace(pos, 1, "ः");
        pos =
            token.find(":", pos + 3); // Devanagari chars are multi-byte (UTF-8)
    }

    // Strip out invisible Zero Width Non-Joiner (ZWNJ) bytes
    std::string zwnj = "\xE2\x80\x8C";
    pos = token.find(zwnj);
    while (pos != std::string::npos)
    {
        token.erase(pos, zwnj.length());
        pos = token.find(zwnj);
    }
}

std::vector<std::string> Tokenizer::tokenize(const std::string &shloka)
{
    // vector to return
    std::vector<std::string> tokens;

    // buffer to hold the current word
    std::string wordBuffer;

    // loop over all characters and add each word to the tokens vector
    for (char c : shloka)
    {
        // add word to wordbuffer if we encounter any of these
        if (c == ' ' || c == '\n' || c == '\r' || c == '\t')
        {
            // Only process if we actually accumulated characters
            if (!wordBuffer.empty())
            {
                sanitize(wordBuffer);
                if (!wordBuffer.empty()) // Check again if sanitize made it empty
                {
                    tokens.push_back(wordBuffer);
                }
                wordBuffer.clear();
            }
        }
        else
        {
            wordBuffer += c;
        }
    }
    if (!wordBuffer.empty() || wordBuffer == "॥")
    {
        sanitize(wordBuffer);
        tokens.push_back(wordBuffer);
    }

    return tokens;
}
