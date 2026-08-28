#pragma once

#include "database.hpp"
#include "wordTypes.hpp"
#include <memory>
#include <string>
#include <vector>

class RootDeriver
{
public:
    RootDeriver(Database &db) : db(db) {}

    /// goes through the wordlist and assigns to the stems
    WordList deriveRoots(const WordList &nodes);

private:
    /// actually derives the root(s) of a metadata and returns all the possible
    /// core(s)
    std::vector<CoreMetadata> deriveRoot(const std::string &word);

    // Stem Reversion Helpers (internal to RootDeriver)
    std::vector<std::string> stripThematicMarkers(const std::string &stem);

    std::vector<std::string>
    reverseConsonantShifts(const std::vector<std::string> &candidates);

    std::vector<std::string>
    reverseGuna(const std::vector<std::string> &candidates);

    std::vector<Root> generateRootCandidates(const std::string &stem);

    Database &db;
};
