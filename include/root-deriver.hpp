#pragma once

#include "database.hpp"
#include "wordTypes.hpp"
#include <memory>
#include <string>
#include <vector>

namespace RootDerivation
{
using RootMemo =
    std::unordered_map<const WordAnalysisNode *, std::vector<std::shared_ptr<WordListNode>>>;
};

class RootDeriver
{
public:
    RootDeriver(Database &db) : db(db) {}

    /// takes in a vector of trees of WordModes and derives the root of each
    /// word in the list returns a tree of all valid paths of complete WordListNode
    /// types
    std::vector<std::shared_ptr<WordListNode>>
    deriveRoots(const std::vector<WordAnalysisNode> &nodes);

private:
    /// takes in a string word and decides if we should bother checking it
    /// returns ALL metadata about the word on the way
    std::vector<WordAnalysis> isValidWord(const std::string &word);

    /// takes in a tree of WordAnalysisNodes and recursively derives the root of
    /// each word in the list returns all linked-lists of the valid paths of Word
    /// types
    std::vector<std::shared_ptr<WordListNode>>
    recursiveDeriveRoots(const WordAnalysisNode &tree,
                         RootDerivation::RootMemo &memo);

    /// actually derives the root of the word and returns it
    Word deriveRoot(const std::string &word, const WordAnalysis &analysis);

    // Stem Reversion Helpers (internal to RootDeriver)
    std::vector<std::string> stripThematicMarkers(const std::string &stem);

    std::vector<std::string>
    reverseConsonantShifts(const std::vector<std::string> &candidates);

    std::vector<std::string>
    reverseGuna(const std::vector<std::string> &candidates);

    std::vector<SanskritRoot>
    generateRootCandidates(const std::string &stem);

    Database &db;
};
