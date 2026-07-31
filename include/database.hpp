#pragma once

#include "rapidjson/document.h"
#include <string>
#include <unordered_map>
#include <unordered_set>

#include "wordTypes.hpp"

class Database
{
public:
    Database() = default;
    ~Database() = default;

    bool initialize(const std::string &rootsJsonPath,
                    const std::string &constantsJsonPath, const std::string &stemsJsonPath);

    bool isPrefix(const std::string &text) const;
    bool isIndeclinable(const std::string &text) const;

    bool tryMatchVerbalSuffix(const std::string &text,
                              VerbMetadata &outMeta) const;
    bool tryMatchNominalSuffix(const std::string &text,
                               NominalMetadata &outMeta) const;
    bool rootExists(const std::string &cleanRoot) const;
    bool stemExists(const std::string &cleanStem) const;

private:
    void loadRoots(const rapidjson::Document &doc);
    void loadConstants(const rapidjson::Document &doc);
    void loadStems(const rapidjson::Document &doc);

    std::unordered_map<std::string, SanskritRoot> rootCache;
    std::unordered_map<std::string, std::string> prefixCache;
    std::unordered_map<std::string, std::string> indeclinableCache;
    std::unordered_map<std::string, VerbMetadata> verbSuffixCache;
    std::unordered_map<std::string, NominalMetadata> nominalSuffixCache;
    std::unordered_set<std::string> stemsCache;
};
