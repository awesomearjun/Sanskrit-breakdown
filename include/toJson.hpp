#include <nlohmann/json.hpp>
#include <optional>
#include <memory>
#include <vector>
#include <string>
#include <wordTypes.hpp>

// Use ordered_json to preserve key insertion order instead of alphabetical sorting
using json = nlohmann::ordered_json;

// --- 1. Verb Metadata ---
inline void to_json(json& j, const VerbMetadata& v) {
    j = json{
        {"tenseOrMood", v.tenseOrMood},
        {"person", v.person},
        {"number", v.number},
        {"voice", v.voice},
        {"suffix", v.suffix},
        {"prefix", v.prefix}
    };
}

// --- 2. Nominal Metadata ---
inline void to_json(json& j, const NominalMetadata& n) {
    j = json{
        {"nominalCase", n.nominalCase},
        {"number", n.number},
        {"gender", n.gender},
        {"suffix", n.suffix},
        {"prefix", n.prefix}
    };
}

// --- 3. Root Metadata ---
inline void to_json(json& j, const SanskritRoot& r) {
    j = json{
        {"originalTagForm", r.originalTagForm},
        {"cleanLookupForm", r.cleanLookupForm},
        {"conjugationClass", r.conjugationClass},
        {"internalVowelRule", r.internalVowelRule},
        {"traditionalMeaning", r.traditionalMeaning},
        {"englishMeaning", r.englishMeaning}
    };
}

// --- 4. WordAnalysis (Guaranteed Insertion Order) ---
inline void to_json(json& j, const WordAnalysis& wa) {
    j = json::object();
    
    // Core fields first
    j["original"] = wa.original;

    // Optional metadata fields
    if (wa.verbInfo.has_value())    j["verbInfo"] = *wa.verbInfo;
    if (wa.nominalInfo.has_value()) j["nominalInfo"] = *wa.nominalInfo;
    if (wa.rootInfo.has_value())    j["rootInfo"] = *wa.rootInfo;

    // Sub-components explicitly attached LAST
    if (!wa.components.empty()) {
        j["components"] = wa.components;
    }
}

// --- 5. Word Data Container ---
inline void to_json(json& j, const Word& w) {
    j = json{
        {"text", w.text},
        {"cleanForm", w.cleanForm},
        {"analyses", w.analyses}
    };
}

// --- 6. Linked List -> Flat JSON Array Helper ---
inline json wordListToJsonArray(const std::vector<std::shared_ptr<WordListNode>>& head) {
    json wordsArray = json::array();

    for (const auto& node : head) {
        if (node) {
            wordsArray.push_back(node->word);
        }
    }

    return wordsArray;
}