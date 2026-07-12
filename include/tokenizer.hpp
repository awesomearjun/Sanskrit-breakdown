#include <string>
#include <vector>

/// Takes a shloka and converts it to a vector of strings for each sandhi'd word
class Tokenizer
{
public:
    Tokenizer() = default;
    ~Tokenizer() = default;
    std::vector<std::wstring> tokenize(const std::wstring& shloka);
};
