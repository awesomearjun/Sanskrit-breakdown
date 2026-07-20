#pragma once

#include <string>
#include <tuple>
#include <unordered_map>
#include <utility>
#include <vector>

#include "database.hpp"
#include "wordTypes.hpp"

using Tokens = std::vector<std::string>;
using TokenAnalysis = std::pair<std::string, WordAnalysis>;

/// Takes tokens given from the tokenizer and splits them into their constituent
/// words; fills out any found metadata for each word in the process
class SandhiSplitter
{
public:
    SandhiSplitter(Database &dbInstance) : db(dbInstance) {}
    ~SandhiSplitter() = default;

    void initializePossibilities();
    std::vector<TokenAnalysis> split(const std::vector<std::string> &tokens);

    NominalMetadata getNominalInfo();
    VerbMetadata getVerbInfo();

private:
    // find splits for one token; vector because it might have multiple words
    std::vector<TokenAnalysis> findSplits(const std::string &token);
    // determines if word is valid
    WordAnalysis isValidWord(const std::string &word);

    // <junction>: { {<left>, <right>}, {<left2, right2>} }
    std::unordered_map<std::string,
                       std::vector<std::pair<std::string, std::string>>>
        rules;

    Database &db;
    VerbMetadata collectedVerbInfo;
    NominalMetadata collectedNominalInfo;
};
