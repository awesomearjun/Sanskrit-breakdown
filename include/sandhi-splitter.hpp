#pragma once

#include <memory>
#include <string>
#include <tuple>
#include <unordered_map>
#include <utility>
#include <vector>

#include "database.hpp"
#include "wordTypes.hpp"

namespace SandhiSplit
{
using Tokens = std::vector<std::string>;
struct WordNode
{
    WordAnalysis analysis;

    std::vector<std::shared_ptr<WordNode>> children;
};

using TreeCache =
    std::unordered_map<std::string, std::vector<std::shared_ptr<WordNode>>>;
} // namespace SandhiSplit

/// Takes tokens given from the tokenizer and splits them into their constituent
/// words; fills out any found metadata for each word in the process
class SandhiSplitter
{
public:
    SandhiSplitter(Database &dbInstance) : db(dbInstance) {}
    ~SandhiSplitter() = default;

    void initializePossibilities();

    /// takes vector of tokens and returns sandhi trees for each of them;
    /// combines all trees from findSplits
    std::vector<std::vector<std::shared_ptr<SandhiSplit::WordNode>>>
    splitTree(std::vector<std::string> &tokens);

    NominalMetadata getNominalInfo();
    VerbMetadata getVerbInfo();

private:
    /// find splits for one token; returns all trees for that
    std::vector<std::shared_ptr<SandhiSplit::WordNode>>
    findSplits(const std::string &token, SandhiSplit::TreeCache &memo);

    // <junction>: { {<left>, <right>}, {<left2, right2>} }
    std::unordered_map<std::string,
                       std::vector<std::pair<std::string, std::string>>>
        rules;

    Database &db;
};
