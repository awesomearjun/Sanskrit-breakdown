#pragma once

#include "rapidjson/document.h"
#include <string>
#include <unordered_map>
#include <unordered_set>
#include <vector>

#include "wordTypes.hpp"

class Database
{
public:
    Database() = default;
    ~Database() = default;

    bool initialize(const std::string &rootsJsonPath,
                    const std::string &constantsJsonPath);

    bool isPrefix(const std::string &text) const;
    bool isIndeclinable(const std::string &text) const;

    const std::vector<VerbMetadata>* tryMatchVerbalSuffix(const std::string &text) const;
    const std::vector<NominalMetadata>* tryMatchNominalSuffix(const std::string &text) const;
    SanskritRoot* rootExists(const std::string &cleanRoot);
    bool stemExists(const std::string &cleanStem) const;

private:
    void loadRoots(const rapidjson::Document &doc);
    void loadConstants(const rapidjson::Document &doc);

    std::unordered_map<std::string, SanskritRoot> rootCache;
    std::unordered_map<std::string, std::string> prefixCache;
    std::unordered_map<std::string, std::string> indeclinableCache;
    std::unordered_map<std::string, std::vector<VerbMetadata>> verbSuffixCache;
    std::unordered_map<std::string, std::vector<NominalMetadata>> nominalSuffixCache;
    std::unordered_set<std::string> stemsCache;
};
