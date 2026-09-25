#pragma once

#include <cstddef>
#include <memory>
#include <nlohmann/json.hpp>
#include <optional>
#include <string>
#include <variant>
#include <vector>
#include <wordTypes.hpp>

using json = nlohmann::ordered_json;

// ==========================================
// 0. Enum String Converters
// ==========================================
inline void to_json(json &j, const VerbTenseOrMood &e)
{
    switch (e)
    {
    case VerbTenseOrMood::PRESENT:
        j = "PRESENT";
        break;
    case VerbTenseOrMood::IMPERFECT_PAST:
        j = "IMPERFECT_PAST";
        break;
    case VerbTenseOrMood::IMPERATIVE:
        j = "IMPERATIVE";
        break;
    case VerbTenseOrMood::POTENTIAL:
        j = "POTENTIAL";
        break;
    case VerbTenseOrMood::PERFECT_PAST:
        j = "PERFECT_PAST";
        break;
    case VerbTenseOrMood::PERIPHRASTIC_FUTURE:
        j = "PERIPHRASTIC_FUTURE";
        break;
    case VerbTenseOrMood::SIMPLE_FUTURE:
        j = "SIMPLE_FUTURE";
        break;
    case VerbTenseOrMood::VEDIC_SUBJUNCTIVE:
        j = "VEDIC_SUBJUNCTIVE";
        break;
    case VerbTenseOrMood::BENEDICTION:
        j = "BENEDICTION";
        break;
    case VerbTenseOrMood::AORIST_PAST:
        j = "AORIST_PAST";
        break;
    case VerbTenseOrMood::CONDITIONAL_FUTURE_PAST:
        j = "CONDITIONAL_FUTURE_PAST";
        break;
    default:
        j = "UNKNOWN";
        break;
    }
}

inline void to_json(json &j, const VerbPerson &e)
{
    switch (e)
    {
    case VerbPerson::THIRD:
        j = "THIRD";
        break;
    case VerbPerson::SECOND:
        j = "SECOND";
        break;
    case VerbPerson::FIRST:
        j = "FIRST";
        break;
    default:
        j = "UNKNOWN";
        break;
    }
}

inline void to_json(json &j, const VerbNumber &e)
{
    switch (e)
    {
    case VerbNumber::SINGULAR:
        j = "SINGULAR";
        break;
    case VerbNumber::DUAL:
        j = "DUAL";
        break;
    case VerbNumber::PLURAL:
        j = "PLURAL";
        break;
    default:
        j = "UNKNOWN";
        break;
    }
}

inline void to_json(json &j, const VerbVoice &e)
{
    switch (e)
    {
    case VerbVoice::ACTIVE:
        j = "ACTIVE";
        break;
    case VerbVoice::REFLEXIVE:
        j = "REFLEXIVE";
        break;
    default:
        j = "UNKNOWN";
        break;
    }
}

inline void to_json(json &j, const NominalCase &e)
{
    switch (e)
    {
    case NominalCase::SUBJECT:
        j = "SUBJECT";
        break;
    case NominalCase::OBJECT:
        j = "OBJECT";
        break;
    case NominalCase::INSTRUMENTAL:
        j = "INSTRUMENTAL";
        break;
    case NominalCase::RECIPIENT:
        j = "RECIPIENT";
        break;
    case NominalCase::ORIGIN:
        j = "ORIGIN";
        break;
    case NominalCase::RELATION:
        j = "RELATION";
        break;
    case NominalCase::CONTEXT:
        j = "CONTEXT";
        break;
    case NominalCase::ADDRESS:
        j = "ADDRESS";
        break;
    default:
        j = "UNKNOWN";
        break;
    }
}

inline void to_json(json &j, const NominalNumber &e)
{
    switch (e)
    {
    case NominalNumber::SINGULAR:
        j = "SINGULAR";
        break;
    case NominalNumber::DUAL:
        j = "DUAL";
        break;
    case NominalNumber::PLURAL:
        j = "PLURAL";
        break;
    default:
        j = "UNKNOWN";
        break;
    }
}

inline void to_json(json &j, const NominalGender &e)
{
    switch (e)
    {
    case NominalGender::MASCULINE:
        j = "MASCULINE";
        break;
    case NominalGender::FEMININE:
        j = "FEMININE";
        break;
    case NominalGender::NEUTER:
        j = "NEUTER";
        break;
    case NominalGender::ANY:
        j = "ANY";
        break;
    default:
        j = "UNKNOWN";
        break;
    }
}

inline void to_json(json &j, const WordMatchType &e)
{
    switch (e)
    {
    case WordMatchType::NONE:
        j = "NONE";
        break;
    case WordMatchType::INDECLINABLE:
        j = "INDECLINABLE";
        break;
    case WordMatchType::VERB:
        j = "VERB";
        break;
    case WordMatchType::NOMINAL:
        j = "NOMINAL";
        break;
    case WordMatchType::IDENTITY:
        j = "IDENTITY";
        break;
    default:
        j = "UNKNOWN";
        break;
    }
}

inline void to_json(json &j, const CoreMatchType &e)
{
    switch (e)
    {
    case CoreMatchType::NONE:
        j = "NONE";
        break;
    case CoreMatchType::ROOT:
        j = "ROOT";
        break;
    case CoreMatchType::STEM:
        j = "STEM";
        break;
    case CoreMatchType::SECONDARY_ENDING:
        j = "SECONDARY_ENDING";
        break;
    default:
        j = "UNKNOWN";
        break;
    }
}

// ==========================================
// 1. Database Model Serializers
// ==========================================
inline void to_json(json &j, const Prefix &p)
{
    j = json{{"text", p.text}, {"englishMeaning", p.englishMeaning}};
}

inline void to_json(json &j, const NominalStem &n)
{
    j = json{{"text", n.text},
             {"ending", n.ending},
             {"gender", n.gender},
             {"englishMeaning", n.meaning.english},
             {"traditionalMeaning", n.meaning.traditional}};
}

inline void to_json(json &j, const SecondaryEnding &n)
{
    j = json{
        {"text", n.suffix.text},
        {"englishMeaning", n.suffix.englishMeaning},
    };
}

inline void to_json(json &j, const Root &r)
{
    if (r.isEmpty())
    {
        j = json::object();
        return;
    }
    j = json{{"originalTagForm", r.originalTagForm},
             {"cleanLookupForm", r.cleanLookupForm},
             {"conjugationClass", r.conjugationClass},
             {"internalVowelRule", r.internalVowelRule},
             {"traditionalMeaning", r.traditionalMeaning},
             {"englishMeaning", r.englishMeaning}};
}

// ==========================================
// 2. Metadata Struct Serializers
// ==========================================
inline void to_json(json &j, const VerbMetadata &v)
{
    j = json{{"tenseOrMood", v.tenseOrMood},
             {"person", v.person},
             {"number", v.number},
             {"voice", v.voice},
             {"suffix", v.suffix},
             {"stem", v.stem},
             {"prefix", v.prefix}};
}

inline void to_json(json &j, const NominalMetadata &n)
{
    j = json{{"nominalCase", n.nominalCase},
             {"number", n.number},
             {"gender", n.gender},
             {"suffix", n.suffix},
             {"stem", n.stem},
             {"prefix", n.prefix}};
}

inline void to_json(json &j, const IndeclinableMetadata &i)
{
    j = json{{"text", i.text}, {"englishMeaning", i.englishMeaning}};
}

// ==========================================
// 3. Core & Word Metadata Serializers
// ==========================================
inline void to_json(json &j, const CoreMetadata &core)
{
    j = json::object();
    j["original"] = core.original;
    j["matchType"] = core.matchType;
    j["success"] = core.success;

    if (core.metadata.has_value())
    {
        std::visit([&j](const auto &arg) { j["metadata"] = arg; },
                   core.metadata.value());
    }
    else
    {
        j["metadata"] = nullptr;
    }
}

inline void to_json(json &j, const WordMetadata &wa)
{
    j = json::object();
    j["original"] = wa.original;
    j["matchType"] = wa.matchType;
    j["success"] = wa.success;

    if (wa.metadata.has_value())
    {
        std::visit([&j](const auto &arg) { j["metadata"] = arg; },
                   wa.metadata.value());
    }
    else
    {
        j["metadata"] = nullptr;
    }

    j["cores"] = wa.cores;
}

// ==========================================
// 4. Word & WordList Containers
// ==========================================
inline void to_json(json &j, const Word &w)
{
    j = json::object();
    j["text"] = w.text;
    j["paths"] = w.metadata;
}

inline void to_json(json &j, const WordList &words)
{
    j = json::array();
    for (const auto &wordPtr : words)
        if (wordPtr)
            j.push_back(*wordPtr);
}

inline json wordListToJsonArray(const WordList &words)
{
    json j;
    to_json(j, words);
    return j;
}

// Overload to handle passing a single std::shared_ptr<Word> directly
inline json wordListToJsonArray(const std::shared_ptr<Word> &wordHead)
{
    json j = json::array();
    if (wordHead)
        j.push_back(*wordHead);
    return j;
}
