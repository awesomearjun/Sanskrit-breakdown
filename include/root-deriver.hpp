#pragma once

#include "database.hpp"
#include "wordTypes.hpp"
#include <string>
#include <vector>

class RootDeriver
{
public:
    RootDeriver(Database &db) : db(db) {}

    /// goes through the wordlist and assigns to the stems
    WordList deriveRoots(const WordList &nodes);

private:
    std::vector<CoreMetadata> deriveRoot(const std::string &word);
    std::vector<CoreMetadata> generateRootCandidates(const std::string &stem);

    Database &db;
};
