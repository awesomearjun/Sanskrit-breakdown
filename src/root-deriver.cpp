#include "root-deriver.hpp"
#include "wordTypes.hpp"
#include <iterator>
#include <memory>
#include <optional>
#include <regex>
#include <string>
#include <vector>

WordList RootDeriver::deriveRoots(const WordList &nodes)
{
    WordList results = std::move(nodes);

    for (std::shared_ptr<Word> &word : results)
    {
        if (!word)
            continue;

        for (std::vector<WordMetadata> &metadatas : word->metadata)
        {
            for (WordMetadata &metadata : metadatas)
            {
                if (metadata.matchType != WordMatchType::VERB)
                    continue;

                std::vector<CoreMetadata> derivedRoots;
                const VerbMetadata &subMetadata = *std::get_if<VerbMetadata>(&metadata.metadata.value());
                std::vector<CoreMetadata> derivedRoot = deriveRoot(subMetadata.stem);
                derivedRoots.insert(derivedRoots.end(), derivedRoot.begin(), derivedRoot.end());

                if (derivedRoots.empty())
                    metadata.success = false;
                else
                    metadata.success = true;

                metadata.cores.insert(
                    metadata.cores.end(),
                    std::make_move_iterator(derivedRoots.begin()),
                    std::make_move_iterator(derivedRoots.end()));
            }
        }
    }

    return results;
}

std::vector<CoreMetadata> RootDeriver::deriveRoot(const std::string &word)
{
    std::vector<CoreMetadata> results;

    for (const CoreMetadata &root : generateRootCandidates(word))
        results.push_back(root);

    return results;
}

//  ======= HELPERS =======

// --- Devanagari UTF-8 Constants & Helpers ---

namespace Devanagari
{
inline bool endsWith(const std::string &str, const std::string &suffix)
{
    return str.size() >= suffix.size() &&
           str.compare(str.size() - suffix.size(), suffix.size(), suffix) == 0;
}

const std::string VIRAMA = "्"; // UTF-8: \xE0\xA5\x8D
const std::string MATRA_I = "ि";
const std::string MATRA_U = "ु";
const std::string MATRA_EE = "े";
const std::string MATRA_O = "ो";
const std::string YA = "य";
const std::string SHAN = "ष";
const std::string SHA = "श";
const std::string SA = "स";
} // namespace Devanagari

// --- PIPELINE STEP 1: Stripping Markers Across All 10 Classes ---

std::vector<CoreMetadata> stripClassMarkers(const std::string &input)
{
    std::vector<CoreMetadata> results;

    // --- Class 7 (Rudhādi): Internal Nasal Infix Removal ---
    // Handles strong/weak forms where (न / ण / न् / ण् / ं) is spliced INSIDE
    // the root. Examples:
    //   - रुणद् (from रुणद्धि)  -> removes internal 'ण'  -> रुद्
    //   - भुनक् (from भुनक्ति)  -> removes internal 'न'  -> भुक् (phonetically
    //   converts to भुज् later)
    //   - रुन्ध् (weak ROOT)    -> removes internal 'न्' -> रुध्
    //   - हिंस्  (from हिंसन्ति) -> removes anusvāra 'ं' -> हिस्

    // 1. Full vowel infix (-न- or -ण-) inside the ROOT:
    std::regex internalVowelNasal(
        R"(([\u0900-\u093F]+)(न|ण)([\u0900-\u097F]+))");
    std::smatch match;
    if (std::regex_match(input, match, internalVowelNasal))
    {
        results.emplace_back(CoreMetadata{
            .original = match[1].str() + match[3].str(),
            .matchType = CoreMatchType::ROOT,
            .metadata = Root{.conjugationClass =
                                 ConjugationClass::CLASS_7_INTERNAL_NASAL}});
    }

    // 2. Half-nasal conjunct (-न्- / -ण्-) or Anusvāra (-ं-) inside the ROOT:
    std::regex internalConjunctNasal(
        R"(([\u0900-\u093F]+)(न्|ण्|ं)([\u0900-\u097F]+))");
    if (std::regex_match(input, match, internalConjunctNasal))
    {
        results.emplace_back(CoreMetadata{
            .original = match[1].str() + match[3].str(),
            .matchType = CoreMatchType::ROOT,
            .metadata = Root{.conjugationClass =
                                 ConjugationClass::CLASS_7_INTERNAL_NASAL}});
    }

    // --- Class 10 (Curādi) & Causatives ---
    // full consonant + 'य' (without preceding Virāma)
    if (Devanagari::endsWith(input, Devanagari::YA))
    {
        std::string prefixPart =
            input.substr(0, input.size() - Devanagari::YA.size());
        if (!Devanagari::endsWith(prefixPart, Devanagari::VIRAMA))
        {
            results.emplace_back(CoreMetadata{
                .original = prefixPart,
                .matchType = CoreMatchType::ROOT,
                .metadata =
                    Root{.conjugationClass =
                             ConjugationClass::CLASS_10_CAUSATIVE_AYA}});
        }
    }

    // --- Class 4 (Divādi) ---
    // explicit conjunct half-consonant + 'य' (Virāma + 'य')
    if (input.size() >= 6 &&
        Devanagari::endsWith(input, Devanagari::VIRAMA + Devanagari::YA))
    {
        results.emplace_back(CoreMetadata{
            .original =
                input.substr(0, input.size() - (Devanagari::VIRAMA.size() +
                                                Devanagari::YA.size())),
            .matchType = CoreMatchType::ROOT,
            .metadata =
                Root{.conjugationClass = ConjugationClass::CLASS_4_YA_INFIX}});
    }

    // --- Class 5 (Svādi) ---
    if (Devanagari::endsWith(input, "नु") || Devanagari::endsWith(input, "नो"))
    {
        results.emplace_back(CoreMetadata{
            .original = input.substr(0, input.size() - 3),
            .matchType = CoreMatchType::ROOT,
            .metadata =
                Root{.conjugationClass = ConjugationClass::CLASS_5_NU_INFIX}});
    }

    // --- Class 8 (Tanādi) ---
    if (Devanagari::endsWith(input, Devanagari::MATRA_U) ||
        Devanagari::endsWith(input, Devanagari::MATRA_O))
    {
        results.emplace_back(CoreMetadata{
            .original = input.substr(0, input.size() - 3),
            .matchType = CoreMatchType::ROOT,
            .metadata =
                Root{.conjugationClass = ConjugationClass::CLASS_8_U_INFIX}});
    }

    // --- Class 9 (Kryādi) ---
    if (Devanagari::endsWith(input, "ना") || Devanagari::endsWith(input, "नी"))
    {
        results.emplace_back(CoreMetadata{
            .original = input.substr(0, input.size() - 3),
            .matchType = CoreMatchType::ROOT,
            .metadata =
                Root{.conjugationClass = ConjugationClass::CLASS_9_NA_INFIX}});
    }

    // --- Class 3 (Juhotyādi): Reduplication ---
    const std::vector<std::string> redUpPrefixes = {"जु", "जि", "शु",  "दि", "सु",
                                                    "द", "बि", "वि", "ज"};
    for (const auto &pref : redUpPrefixes)
    {
        if (input.size() > pref.size() &&
            input.compare(0, pref.size(), pref) == 0)
        {
            results.emplace_back(CoreMetadata{
                .original = input.substr(pref.size()),
                .matchType = CoreMatchType::ROOT,
                .metadata = Root{.conjugationClass =
                                     ConjugationClass::CLASS_3_DUPLICATED}});
        }
    }

    // --- Fallback (Classes 1, 2, 6) ---
    results.emplace_back(
        CoreMetadata{.original = input,
                     .matchType = CoreMatchType::ROOT,
                     .metadata = Root{.conjugationClass =
                                          ConjugationClass::CLASS_1_BASE_A}});
    results.emplace_back(CoreMetadata{
        .original = input,
        .matchType = CoreMatchType::ROOT,
        .metadata =
            Root{.conjugationClass = ConjugationClass::CLASS_2_DIRECT_ATTACH}});
    results.emplace_back(CoreMetadata{
        .original = input,
        .matchType = CoreMatchType::ROOT,
        .metadata =
            Root{.conjugationClass = ConjugationClass::CLASS_6_ACCENTED_A}});

    return results;
}

// --- PIPELINE STEP 2: Phonetic Shift Reversals & DB Queries ---

std::vector<CoreMetadata>
resolvePhoneticsAndDatabase(const std::vector<CoreMetadata> &inputs,
                            Database &db)
{
    std::vector<CoreMetadata> outputs = std::move(inputs);

    std::unordered_set<std::string> evaluatedRoots;

    auto testAndAdd = [&](const std::string &candidateStr, CoreMetadataType &candidateRoot)
    {
        if (std::optional<Root> r = db.rootExists(candidateStr); r.has_value())
        {
            r->conjugationClass = std::get<Root>(candidateRoot).conjugationClass;
            candidateRoot = r.value();
        }
    };

    for (auto &cand : outputs)
    {
        std::string base = cand.original;

        // 1. Direct candidate
        testAndAdd(base, cand.metadata.value());

        // 2. Halanta Restoration (Consonant-ending roots)
        if (!Devanagari::endsWith(base, Devanagari::VIRAMA))
            testAndAdd(base + Devanagari::VIRAMA, cand.metadata.value());

        // 3. Class 7 / Velar & Palatal Softening (e.g. क् -> ज / ग्, त् -> द्)
        if (Devanagari::endsWith(base, "क्") || Devanagari::endsWith(base, "क"))
        {
            std::string j_input = base.substr(0, base.size() - 3) + "ज्";
            testAndAdd(j_input, cand.metadata.value());
        }
        if (Devanagari::endsWith(base, "द्") || Devanagari::endsWith(base, "द"))
        {
            std::string dh_input = base.substr(0, base.size() - 3) + "ध्";
            testAndAdd(dh_input, cand.metadata.value());
        }

        // 4. Retroflexion Reversals (ष -> श / स)
        size_t pos = base.find(Devanagari::SHAN);
        if (pos != std::string::npos)
        {
            std::string s_var = base;
            s_var.replace(pos, Devanagari::SHAN.size(), Devanagari::SHA);
            testAndAdd(s_var + Devanagari::VIRAMA, cand.metadata.value());

            std::string s2_var = base;
            s2_var.replace(pos, Devanagari::SHAN.size(), Devanagari::SA);
            testAndAdd(s2_var + Devanagari::VIRAMA, cand.metadata.value());
        }

        // 5. Guṇa Vowel Reversals (ो -> ु, े -> ि)
        pos = base.find(Devanagari::MATRA_O);
        if (pos != std::string::npos)
        {
            std::string u_var = base;
            u_var.replace(pos, Devanagari::MATRA_O.size(), Devanagari::MATRA_U);
            testAndAdd(u_var, cand.metadata.value());
            testAndAdd(u_var + Devanagari::VIRAMA, cand.metadata.value());
        }

        pos = base.find(Devanagari::MATRA_EE);
        if (pos != std::string::npos)
        {
            std::string i_var = base;
            i_var.replace(pos, Devanagari::MATRA_EE.size(),
                          Devanagari::MATRA_I);
            testAndAdd(i_var, cand.metadata.value());
            testAndAdd(i_var + Devanagari::VIRAMA, cand.metadata.value());
        }
    }

    return outputs;
}

// Orchestrator function
std::vector<CoreMetadata>
RootDeriver::generateRootCandidates(const std::string &input)
{
    auto step1Candidates = stripClassMarkers(input);
    return resolvePhoneticsAndDatabase(step1Candidates, db);
}
