#pragma once

#include "database.hpp"
#include "wordTypes.hpp"
#include <memory>
#include <string>
#include <vector>

namespace RootDerivation
{
using RootMemo =
    std::unordered_map<const WordTreeNode *, ValidDerives>;
};

class RootDeriver
{
public:
    RootDeriver(Database &db) : db(db) {}

    /// takes in a vector of trees of WordModes and derives the root of each
    /// word in the list returns a tree of all valid paths of complete WordListNode
    /// types
    std::vector<ValidDerives>
    deriveRoots(const std::vector<SandhiCandidate> &nodes);

private:
    /// takes in a tree of WordTreeNodes and recursively derives the root of
    /// each word in the list returns all linked-lists of the valid paths of Word
    /// types
    ValidDerives
    recursiveDeriveRoots(const std::shared_ptr<WordTreeNode> &tree,
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
