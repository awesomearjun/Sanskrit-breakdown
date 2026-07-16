#pragma once

#include "rapidjson/document.h"
#include <string>
#include <unordered_map>

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

    bool tryMatchVerbalSuffix(const std::string &text,
                              VerbMetadata &outMeta) const;
    bool tryMatchNominalSuffix(const std::string &text,
                               NominalMetadata &outMeta) const;
    bool rootExists(const std::string &cleanRoot) const;

private:
    void loadRoots(const rapidjson::Document &doc);
    void loadConstants(const rapidjson::Document &doc);

    std::unordered_map<std::string, SanskritRoot> rootCache;
    std::unordered_map<std::string, std::string> prefixCache;
    std::unordered_map<std::string, std::string> indeclinableCache;
    std::unordered_map<std::string, VerbMetadata> verbSuffixCache;
    std::unordered_map<std::string, NominalMetadata> nominalSuffixCache;
};
