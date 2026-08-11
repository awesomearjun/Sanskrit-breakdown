#include "database.hpp"

#include "rapidjson/document.h"
#include "wordTypes.hpp"
#include <cassert>
#include <fstream>
#include <iostream>
#include <string>

// ----- CONVERSION UTILITIES -----

auto ConjugationStringToType(const std::string &str)
{
    static const std::unordered_map<std::string, ConjugationClass>
        conjugationStringMap = {
            {"CLASS_1_BASE_A", ConjugationClass::CLASS_1_BASE_A},
            {"CLASS_2_DIRECT_ATTACH", ConjugationClass::CLASS_2_DIRECT_ATTACH},
            {"CLASS_3_DUPLICATED", ConjugationClass::CLASS_3_DUPLICATED},
            {"CLASS_4_YA_INFIX", ConjugationClass::CLASS_4_YA_INFIX},
            {"CLASS_5_NU_INFIX", ConjugationClass::CLASS_5_NU_INFIX},
            {"CLASS_6_ACCENTED_A", ConjugationClass::CLASS_6_ACCENTED_A},
            {"CLASS_7_INTERNAL_NASAL",
             ConjugationClass::CLASS_7_INTERNAL_NASAL},
            {"CLASS_8_U_INFIX", ConjugationClass::CLASS_8_U_INFIX},
            {"CLASS_9_NA_INFIX", ConjugationClass::CLASS_9_NA_INFIX},
            {"CLASS_10_CAUSATIVE_AYA",
             ConjugationClass::CLASS_10_CAUSATIVE_AYA}};

    return conjugationStringMap.at(str);
}

auto NominalGenderToType(const std::string &str)
{
    static const std::unordered_map<std::string, NominalGender>
        genderStringMap = {{"MASCULINE", NominalGender::MASCULINE},
                           {"FEMININE", NominalGender::FEMININE},
                           {"NEUTER", NominalGender::NEUTER}};

    return genderStringMap.at(str);
}

auto NominalCaseToType(const std::string &str)
{
    static const std::unordered_map<std::string, NominalCase> caseStringMap = {
        {"SUBJECT", NominalCase::SUBJECT},
        {"OBJECT", NominalCase::OBJECT},
        {"INSTRUMENTAL", NominalCase::INSTRUMENTAL},
        {"RECIPIENT", NominalCase::RECIPIENT},
        {"ORIGIN", NominalCase::ORIGIN},
        {"RELATION", NominalCase::RELATION},
        {"CONTEXT", NominalCase::CONTEXT},
        {"ADDRESS", NominalCase::ADDRESS}};

    return caseStringMap.at(str);
}

auto VoiceStringToType(const std::string &str)
{
    static const std::unordered_map<std::string, RootVoiceType> voiceStringMap =
        {{"ACTIVE_VOICE_MARKERS", RootVoiceType::ACTIVE_VOICE_MARKERS},
         {"REFLEXIVE_VOICE_MARKERS", RootVoiceType::REFLEXIVE_VOICE_MARKERS},
         {"SHARED_VOICE_MARKERS", RootVoiceType::SHARED_VOICE_MARKERS}};

    return voiceStringMap.at(str);
}

auto SuffixPersonStringToType(const std::string &str)
{
    static const std::unordered_map<std::string, VerbPerson> personStringMap = {
        {"FIRST", VerbPerson::FIRST},
        {"SECOND", VerbPerson::SECOND},
        {"THIRD", VerbPerson::THIRD}};

    return personStringMap.at(str);
}

auto SuffixNumberStringToType(const std::string &str)
{
    static const std::unordered_map<std::string, VerbNumber>
        verbNumberStringMap = {{"SINGULAR", VerbNumber::SINGULAR},
                               {"DUAL", VerbNumber::DUAL},
                               {"PLURAL", VerbNumber::PLURAL}};

    return verbNumberStringMap.at(str);
}

auto NominalNumberStringToType(const std::string &str)
{
    static const std::unordered_map<std::string, NominalNumber>
        nominalNumberStringMap = {{"SINGULAR", NominalNumber::SINGULAR},
                                  {"DUAL", NominalNumber::DUAL},
                                  {"PLURAL", NominalNumber::PLURAL}};

    return nominalNumberStringMap.at(str);
}

auto VowelRuleStringToRule(const std::string &str)
{
    static const std::unordered_map<std::string, InternalVowelRule>
        vowelRuleStringMap = {
            {"ALLOWS_LINKING_VOWEL", InternalVowelRule::ALLOWS_LINKING_VOWEL},
            {"BLOCKS_LINKING_VOWEL", InternalVowelRule::BLOCKS_LINKING_VOWEL},
            {"VARIABLE_LINKING_VOWEL",
             InternalVowelRule::VARIABLE_LINKING_VOWEL}};

    return vowelRuleStringMap.at(str);
}

// ----- DATABASE IMPLEMENTATION -----

bool Database::initialize(const std::string &rootsJsonPath,
                          const std::string &constantsJsonPath)
{
    rapidjson::Document buffer;

    // roots
    std::ifstream rootsFile(rootsJsonPath);
    if (!rootsFile.is_open())
    {
        std::cerr << "Failed to open roots JSON file: " << rootsJsonPath
                  << std::endl;
        return false;
    }

    std::string rootsJson((std::istreambuf_iterator<char>(rootsFile)),
                          std::istreambuf_iterator<char>());

    buffer.Parse(rootsJson.c_str());
    if (buffer.HasParseError() || !buffer.IsArray())
    {
        std::cerr << "Error parsing roots JSON file: " << rootsJsonPath
                  << std::endl;
        return false;
    }
    rootsFile.close();

    loadRoots(buffer);
    buffer.SetObject();

    // constants
    std::ifstream constantsFile(constantsJsonPath);
    if (!constantsFile.is_open())
    {
        std::cerr << "Failed to open constants JSON file: " << constantsJsonPath
                  << std::endl;
        return false;
    }

    std::string constantsJson((std::istreambuf_iterator<char>(constantsFile)),
                              std::istreambuf_iterator<char>());

    buffer.Parse(constantsJson.c_str());

    if (buffer.HasParseError() || !buffer.IsObject())
    {
        std::cerr << "Error parsing constants JSON file: " << constantsJsonPath
                  << std::endl;
        return false;
    }
    constantsFile.close();
    loadConstants(buffer);
    buffer.SetObject();

    return true;
}

// --- Public Query Functions ---

/**
 * Checks if a given substring exists in the prefix cache.
 */
bool Database::isPrefix(const std::string &text) const
{
    return prefixCache.find(text) != prefixCache.end();
}

/**
 * Checks if a given word exists in the indeclinable cache.
 */
bool Database::isIndeclinable(const std::string &text) const
{
    return indeclinableCache.find(text) != indeclinableCache.end();
}

/**
 * Tries to find a matching verbal suffix.
 * If found, populates outMeta and returns true. Otherwise returns false.
 */
const std::vector<VerbMetadata>* Database::tryMatchVerbalSuffix(const std::string &text) const
{
    auto it = verbSuffixCache.find(text);

    if (it != verbSuffixCache.end())
    {
        return &(it->second); // Return the vector of metadata
    }
    return nullptr;
}

/**
 * Tries to find a matching nominal suffix.
 * If found, populates outMeta and returns true. Otherwise returns false.
 */
const std::vector<NominalMetadata>* Database::tryMatchNominalSuffix(const std::string &text) const
{
    auto it = nominalSuffixCache.find(text);

    if (it != nominalSuffixCache.end())
    {
        return &(it->second); // Return the vector of metadata
    }
    return nullptr;
}

SanskritRoot* Database::rootExists(const std::string &cleanRoot)
{
    auto it = rootCache.find(cleanRoot);
    return it != rootCache.end() ? &(it->second) : nullptr;
}

// ----- LOADERS -----

void Database::loadRoots(const rapidjson::Document &doc)
{
    for (const auto &rootEntry : doc.GetArray())
    {
        if (!rootEntry.IsObject())
            continue;

        SanskritRoot root;
        root.originalTagForm = rootEntry["originalTagForm"].GetString();
        root.cleanLookupForm = rootEntry["cleanLookupForm"].GetString();
        root.conjugationClass =
            ConjugationStringToType(rootEntry["conjugationClass"].GetString());
        root.naturalVoiceType =
            VoiceStringToType(rootEntry["naturalVoiceType"].GetString());
        root.internalVowelRule =
            VowelRuleStringToRule(rootEntry["internalVowelRule"].GetString());
        root.traditionalMeaning = rootEntry["traditionalMeaning"].GetString();
        root.englishMeaning = rootEntry["englishMeaning"].GetString();

        // Store the root in the cache
        rootCache[root.cleanLookupForm] = std::move(root);
    }
}

void Database::loadConstants(const rapidjson::Document &doc)
{
    // Load prefixes from an array of objects
    if (doc.HasMember("prefixes") && doc["prefixes"].IsArray())
    {
        for (const auto &prefixObj : doc["prefixes"].GetArray())
        {
            if (prefixObj.IsObject() && prefixObj.HasMember("text") &&
                prefixObj["text"].IsString() &&
                prefixObj.HasMember("englishMeaning") &&
                prefixObj["englishMeaning"].IsString())
            {

                std::string prefixText = prefixObj["text"].GetString();
                std::string meaning = prefixObj["englishMeaning"].GetString();

                // Map the text key to its semantic meaning
                prefixCache[prefixText] = meaning;
            }
        }
    }

    // Load indeclinables
    if (doc.HasMember("indeclinables") && doc["indeclinables"].IsArray())
    {
        for (const auto &indeclinableObj : doc["indeclinables"].GetArray())
        {
            if (indeclinableObj.IsObject() &&
                indeclinableObj.HasMember("text") &&
                indeclinableObj["text"].IsString() &&
                indeclinableObj.HasMember("englishMeaning") &&
                indeclinableObj["englishMeaning"].IsString())
            {

                std::string indeclinableText =
                    indeclinableObj["text"].GetString();
                std::string meaning =
                    indeclinableObj["englishMeaning"].GetString();

                // Map the text key to its semantic meaning
                indeclinableCache[indeclinableText] = meaning;
            }
        }
    }

    // Load suffixes
    if (doc.HasMember("inflectionSuffixes") &&
        doc["inflectionSuffixes"].IsObject())
    {
        const auto &inflectionObj = doc["inflectionSuffixes"].GetObject();

        // present active
        if (inflectionObj.HasMember("verbalPresentActive") &&
            inflectionObj["verbalPresentActive"].IsArray())
        {
            const auto &verbalPresentActiveObj =
                inflectionObj["verbalPresentActive"].GetArray();

            for (const auto &i : verbalPresentActiveObj)
            {
                assert(i.IsObject());
                assert(i.HasMember("suffix") && i["suffix"].IsString());

                VerbMetadata meta;

                meta.suffix = i["suffix"].GetString();
                meta.person = SuffixPersonStringToType(i["person"].GetString());
                meta.number = SuffixNumberStringToType(i["number"].GetString());
                meta.tenseOrMood = VerbTenseOrMood::PRESENT;
                meta.voice = VerbVoice::ACTIVE;

                verbSuffixCache[i["suffix"].GetString()].push_back(std::move(meta));
            }
        }

        // present reflexive
        if (inflectionObj.HasMember("verbalPresentReflexive") &&
            inflectionObj["verbalPresentReflexive"].IsArray())
        {
            const auto &verbalPresentReflexiveObj =
                inflectionObj["verbalPresentReflexive"].GetArray();

            for (const auto &i : verbalPresentReflexiveObj)
            {
                assert(i.IsObject());
                assert(i.HasMember("suffix") && i["suffix"].IsString());
                VerbMetadata meta;

                meta.suffix = i["suffix"].GetString();
                meta.person = SuffixPersonStringToType(i["person"].GetString());
                meta.number = SuffixNumberStringToType(i["number"].GetString());
                meta.tenseOrMood = VerbTenseOrMood::PRESENT;
                meta.voice = VerbVoice::ACTIVE;

                verbSuffixCache[i["suffix"].GetString()].push_back(std::move(meta));
            }
        }
        // nominals
        if (inflectionObj.HasMember("nominals") &&
            inflectionObj["nominals"].IsArray())
        {
            const auto &nominalsObj = inflectionObj["nominals"].GetArray();

            for (const auto &i : nominalsObj)
            {
                assert(i.IsObject());
                assert(i.HasMember("suffix") && i["suffix"].IsString());
                NominalMetadata meta;
                meta.suffix = i["suffix"].GetString();
                meta.gender = NominalGenderToType(i["gender"].GetString());
                meta.nominalCase =
                    NominalCaseToType(i["nominalCase"].GetString());
                meta.number =
                    NominalNumberStringToType(i["number"].GetString());
                meta.gender = NominalGenderToType(i["gender"].GetString());
                nominalSuffixCache[i["suffix"].GetString()].push_back(std::move(meta));
            }
        }
    }
}