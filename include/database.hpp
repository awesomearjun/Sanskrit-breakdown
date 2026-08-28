#pragma once

#include "rapidjson/document.h"
#include <optional>
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
                    const std::string &constantsJsonPath,
                    const std::string &stemsJsonPath);

    std::optional<Prefix> isPrefix(const std::string &text) const;
    std::optional<IndeclinableMetadata>
    isIndeclinable(const std::string &text) const;

    std::optional<std::vector<VerbMetadata>>
    tryMatchVerbalSuffix(const std::string &text) const;
    std::optional<std::vector<NominalMetadata>>
    tryMatchNominalSuffix(const std::string &text) const;
    std::optional<Root> rootExists(const std::string &cleanRoot);
    std::optional<NominalStem> stemExists(const std::string &cleanStem);

private:
    void loadRoots(const rapidjson::Document &doc);
    void loadConstants(const rapidjson::Document &doc);
    void loadStems(const rapidjson::Document &doc);

    std::unordered_map<std::string, Root> rootCache;
    std::unordered_map<std::string, Prefix> prefixCache;
    std::unordered_map<std::string, IndeclinableMetadata> indeclinableCache;
    std::unordered_map<std::string, std::vector<VerbMetadata>> verbSuffixCache;
    std::unordered_map<std::string, std::vector<NominalMetadata>>
        nominalSuffixCache;
    std::unordered_map<std::string, NominalStem> stemsCache;
};
