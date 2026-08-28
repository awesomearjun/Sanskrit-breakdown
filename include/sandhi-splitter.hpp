#pragma once

#include <string>
#include <unordered_map>
#include <utility>
#include <vector>

#include "database.hpp"
#include "wordTypes.hpp"

namespace SandhiSplit
{
using Tokens = std::vector<std::string>;

using SplitPath = std::vector<WordMetadata>;
using MultiPathMemo = std::unordered_map<std::string, std::vector<SplitPath>>;
} // namespace SandhiSplit

/// Takes tokens given from the tokenizer and splits them into their constituent
/// words; fills out any found metadata for each word in the process
class SandhiSplitter
{
public:
    SandhiSplitter(Database &dbInstance) : db(dbInstance) {}
    ~SandhiSplitter() = default;

    void initializePossibilities();

    /// takes a WordList and fills it with possibilities
    WordList splitTree(const WordList &tokens);

private:
    /// find splits for one token; returns all trees for that
    std::vector<SandhiSplit::SplitPath>
    findSplits(const std::string &token, SandhiSplit::MultiPathMemo &memo);

    // get some intel about the word; returns all valid interpretations (empty
    // if ghost word)
    std::vector<WordMetadata> isValidWord(const std::string &word);

    // <junction>: { {<left>, <right>}, {<left2, right2>} }
    std::unordered_map<std::string,
                       std::vector<std::pair<std::string, std::string>>>
        rules;

    Database &db;
};
