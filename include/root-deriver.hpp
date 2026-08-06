#pragma once

#include "database.hpp"
#include "sandhi-splitter.hpp"
#include "wordTypes.hpp"
#include <memory>
#include <string>
#include <vector>

namespace RootDerivation
{
using RootMemo =
    std::unordered_map<const WordAnalysisNode *, std::shared_ptr<WordNode>>;
};

class RootDeriver
{
public:
    RootDeriver(Database &db) : db(db) {}

    /// takes in a vector of trees of WordModes and derives the root of each
    /// word in the list returns a tree of all valid paths of complete WordNode
    /// types
    std::vector<std::shared_ptr<WordNode>>
    deriveRoots(const std::vector<WordAnalysisNode> &nodes);

    // TODO: take out of public
    /// takes in a string word and decides if we should bother checking it
    /// returns ALL metadata about the word on the way
    std::vector<WordAnalysis> isValidWord(const std::string &word);

private:


    /// takes in a tree of WordAnalysisNodes and recursively derives the root of
    /// each word in the list returns a linked-list of the valid paths of Word
    /// types
    std::shared_ptr<WordNode>
    recursiveDeriveRoots(const WordAnalysisNode &tree,
                         RootDerivation::RootMemo &memo);

    /// actually derives the root of the word and returns it
    Word deriveRoot(const std::string &word);

    Database &db;
};
