#include "root-deriver.hpp"
#include "wordTypes.hpp"
#include <memory>
#include <vector>

using namespace RootDerivation;

std::vector<std::shared_ptr<WordListNode>>
RootDeriver::deriveRoots(const std::vector<WordAnalysisNode> &nodes)
{
    std::vector<std::shared_ptr<WordListNode>> results;

    for (const auto &head: nodes)
    {
        RootDerivation::RootMemo memo;
        std::vector<std::shared_ptr<WordListNode>> derivedRoots =
            recursiveDeriveRoots(head, memo);

        if (derivedRoots.empty())
            continue;

        results.insert(results.end(), derivedRoots.begin(), derivedRoots.end());
    }

    return results;
}

std::vector<std::shared_ptr<WordListNode>>
RootDeriver::recursiveDeriveRoots(const WordAnalysisNode &tree,
                                  RootDerivation::RootMemo &memo)
{
    // 1. Memoization Hit
    auto it = memo.find(&tree);
    if (it != memo.end())
    {
        return it->second;
    }

    std::vector<std::shared_ptr<WordListNode>> results;

    // 2. Validate Current Surface Word
    auto currentNode = std::make_shared<WordListNode>();
    for (auto &analysis : isValidWord(tree.analysis.original))
    {
        if (analysis.success)
        {
            currentNode->word.roots = generateRootCandidates(analysis.components.back());
            currentNode->word.analyses.push_back(analysis);
        }
    }

    // Dead end: stop if this word has no valid root derivations
    if (currentNode->word.analyses.empty())
    {
        return memo[&tree] = results;
    }

    // 3. BASE CASE: Leaf Node (No children)
    if (tree.children.empty())
    {
        results.push_back(currentNode);
        return memo[&tree] = results;
    }

    // 4. RECURSIVE STEP: Combine with valid child branches
    for (const auto &child : tree.children)
    {
        std::vector<std::shared_ptr<WordListNode>> childResults =
            recursiveDeriveRoots(*child, memo);

        for (const auto &childNode : childResults)
        {
            auto newNode = std::make_shared<WordListNode>();
            newNode->word = currentNode->word;
            newNode->next = childNode;
            results.push_back(newNode);
        }
    }

    // 5. Store result in memo cache
    return memo[&tree] = results;
}

//  ======= HELPERS =======

std::vector<WordAnalysis> RootDeriver::isValidWord(const std::string &word)
{
    std::vector<WordAnalysis> validInterpretations;

    // 1. Standalone Indeclinable Check (e.g., if "अनु" appears by itself as an
    // Avyaya)
    if (db.isIndeclinable(word))
    {
        WordAnalysis avyayaAnalysis;
        avyayaAnalysis.success = true;
        avyayaAnalysis.components = {word};
        avyayaAnalysis.matchType = "indeclinable";
        validInterpretations.push_back(avyayaAnalysis);
    }

    // Stack-allocated metadata containers (Thread-Safe)
    VerbMetadata verbInfo;
    NominalMetadata nominalInfo;

    for (size_t len = 3; len <= 18 && len <= word.size(); len += 3)
    {
        // 2. Full Word Verbal Suffix Match (No prefix case: e.g., "गच्छति")
        std::string vsuffix = word.substr(word.length() - len, len);
        if (db.tryMatchVerbalSuffix(vsuffix, verbInfo))
        {
            WordAnalysis verbAnalysis;
            verbAnalysis.success = true;
            verbAnalysis.components = {word};
            verbAnalysis.verbInfo = verbInfo;
            verbAnalysis.matchType = "verb";
            validInterpretations.push_back(verbAnalysis);
        }

        // 3. Full Word Nominal Suffix Match (No prefix case: e.g., "रामेण")
        std::string nsuffix = word.substr(word.length() - len, len);
        if (db.tryMatchNominalSuffix(nsuffix, nominalInfo))
        {
            WordAnalysis nominalAnalysis;
            nominalAnalysis.success = true;
            nominalAnalysis.components = {word};
            nominalAnalysis.nominalInfo = nominalInfo;
            nominalAnalysis.matchType = "nominal";
            validInterpretations.push_back(nominalAnalysis);
        }

        // Skip prefix checks after 4 characters (12 bytes)
        if (len <= 12)
        {

            // 4. Upasarga (Prefix) + Stem Compound Check (e.g., "अनुगच्छति")
            // Here we slice prefix prefixes AND check if the remaining stem is
            // ALSO valid!
            std::string candidatePrefix = word.substr(0, len);
            if (db.isPrefix(candidatePrefix))
            {
                std::string remainingStem = word.substr(len);

                std::vector<WordAnalysis> stemResults =
                    isValidWord(remainingStem);

                for (auto &stemAnalysis : stemResults)
                {
                    // Create a combined analysis
                    WordAnalysis combined = stemAnalysis;

                    // Prepend the current candidate prefix to the component
                    // list
                    combined.components.insert(combined.components.begin(),
                                               candidatePrefix);

                    validInterpretations.push_back(combined);
                }
            }
        }
    }

    return validInterpretations; // Returns all valid interpretations found
                                 // (empty if ghost word)
}

// Orchestrator function
std::vector<SanskritRoot>
RootDeriver::generateRootCandidates(const std::string &stem)
{
    // 1. Pass initial candidates (from Stage 1 stripTinSuffix) into Stage 2
    std::vector<std::string> thematicCandidates = stripThematicMarkers(stem);

    // 2. Pass Stage 2 candidates into Stage 3 (Reverse Consonant Shifts)
    std::vector<std::string> shiftCandidates =
        reverseConsonantShifts(thematicCandidates);

    // 3. Pass Stage 3 candidates into Stage 4 (Reverse Guṇa / Vowel
    // Reconstruction)
    std::vector<std::string> rawRootCandidates = reverseGuna(shiftCandidates);

    // 4. Final Deduplication Pass
    std::vector<SanskritRoot> finalCandidates;
    std::unordered_set<std::string> seenForms;

    for (const auto &candidate : rawRootCandidates)
    {
        const std::string &form = candidate;

        // Skip empty forms or already checked string patterns
        if (form.empty() || seenForms.count(form))
            continue;
        seenForms.insert(form);

        // Lookup in Dhātupāṭha database
        SanskritRoot fullRoot = db.rootExists(form);

        // If the root exists in DB, keep the enriched metadata and discard bad
        // candidates
        if (!fullRoot.cleanLookupForm.empty())
            finalCandidates.push_back(fullRoot);
    }

    return finalCandidates;
}

// Helper to check if string ends with a specific UTF-8 substring
bool stemEndsWith(const std::string &str, const std::string &suffix)
{
    if (suffix.size() > str.size())
        return false;
    return str.compare(str.size() - suffix.size(), suffix.size(), suffix) == 0;
}

// Helper to append a Virāma (्) if the stem ends in an implicit vowel 'अ'
std::string applyVirama(const std::string &stem)
{
    std::string virama = "्";
    if (!stemEndsWith(stem, virama))
        return stem + virama;
    return stem;
}

std::vector<std::string>
RootDeriver::stripThematicMarkers(const std::string &stem)
{
    std::vector<std::string> candidates;

    // we will temporarily change the cleanLookupForm to be the stem, and then
    // apply transformations to it later then remove the invalid stems

    // -------------------------------------------------------------
    // 1. Class 10 (Curādi) Marker: -अय (-aya)
    // Example: "चोरय" -> "चोर्"
    // -------------------------------------------------------------
    std::string ayaMarker = "अय";
    if (stemEndsWith(stem, ayaMarker))
    {
        std::string core = stem.substr(0, stem.size() - ayaMarker.size());
        candidates.push_back(applyVirama(core));
    }

    // -------------------------------------------------------------
    // 2. Class 9 (Kryādi) Markers: -ना (-nā) / -णी (-ṇī)
    // Example: "क्रीणा" -> "क्री"
    // -------------------------------------------------------------
    std::string naMarker1 = "ना";
    std::string naMarker2 = "णा";
    std::string niMarker1 = "नी";
    std::string niMarker2 = "णी";

    if (stemEndsWith(stem, naMarker1))
    {
        std::string core = stem.substr(0, stem.size() - naMarker1.size());
        candidates.push_back(core);
    }
    else if (stemEndsWith(stem, naMarker2))
    {
        std::string core = stem.substr(0, stem.size() - naMarker2.size());
        candidates.push_back(core);
    }
    else if (stemEndsWith(stem, niMarker1))
    {
        std::string core = stem.substr(0, stem.size() - niMarker1.size());
        candidates.push_back(core);
    }
    else if (stemEndsWith(stem, niMarker2))
    {
        std::string core = stem.substr(0, stem.size() - niMarker2.size());
        candidates.push_back(core);
    }

    // -------------------------------------------------------------
    // 3. Class 4 (Divādi) Marker: -य (-ya)
    // Example: "नृत्य" -> "नृत्"
    // -------------------------------------------------------------
    std::string yaMarker = "य";
    if (stemEndsWith(stem, yaMarker) && !stemEndsWith(stem, ayaMarker))
    {
        std::string core = stem.substr(0, stem.size() - yaMarker.size());
        candidates.push_back(applyVirama(core));
    }

    // -------------------------------------------------------------
    // 4. Class 5 (Svādi) & Class 8 (Tanādi) Markers: -नो/-नु or -ओ/-उ
    // Example: "सुनो" -> "सु", "तनो" -> "तन्"
    // -------------------------------------------------------------
    std::string noMarker = "नो";
    std::string nuMarker = "नु";
    std::string oMarker = "ो";
    std::string uMarker = "ु";

    if (stemEndsWith(stem, noMarker))
    {
        std::string core = stem.substr(0, stem.size() - noMarker.size());
        candidates.push_back(core);
    }
    else if (stemEndsWith(stem, nuMarker))
    {
        std::string core = stem.substr(0, stem.size() - nuMarker.size());
        candidates.push_back(core);
    }
    else if (stemEndsWith(stem, oMarker))
    {
        std::string core = stem.substr(0, stem.size() - oMarker.size());
        candidates.push_back(applyVirama(core));
    }

    // -------------------------------------------------------------
    // 5. Thematic Vowel Marker: -a (Class 1 Bhvādi, Class 6 Tudādi)
    // In Devanagari UTF-8 strings, if the stem ends in an implicit vowel,
    // we convert the stem to Halanta (append Virāma '्') to prepare for
    // consonantal and vowel transformations.
    // -------------------------------------------------------------
    std::string halantaCore = applyVirama(stem);
    candidates.push_back(halantaCore);
    candidates.push_back(halantaCore);

    // -------------------------------------------------------------
    // 6. Class 2 (Adādi) / Class 7 (Rudhādi) Fallback (Athematic / Zero-marker)
    // Core remains unchanged
    // -------------------------------------------------------------
    candidates.push_back(stem);
    candidates.push_back(stem);

    return candidates;
}

// Helper to check if string ends with a UTF-8 suffix
static bool endsWithUTF8(const std::string &str, const std::string &suffix)
{
    if (suffix.size() > str.size())
        return false;
    return str.compare(str.size() - suffix.size(), suffix.size(), suffix) == 0;
}

// Helper to remove an internal nasal before the final consonant (Class 7
// Rudhādi Infix) e.g., "रुन्ध्" -> "रुध्", "भिन्दू" -> "भिद्", "युञ्ज्" -> "युज्"
static std::string removeNasalInfix(const std::string &str)
{
    // UTF-8 bytes for Devanagari nasals + halanta and anusvara
    static const std::vector<std::string> nasals = {"न्", "ण्", "म्",
                                                    "ञ्", "ङ्", "ं"};

    std::string result = str;
    for (const auto &nasal : nasals)
    {
        size_t pos = result.find(nasal);
        // Ensure nasal is internal (not at index 0 and not at the very end)
        if (pos != std::string::npos && pos > 0 &&
            pos + nasal.size() < result.size())
        {
            result.erase(pos, nasal.size());
            break; // Strip at most one infix nasal
        }
    }
    return result;
}

std::vector<std::string>
RootDeriver::reverseConsonantShifts(const std::vector<std::string> &candidates)
{
    std::vector<std::string> results;
    std::unordered_set<std::string> seenForms;

    auto addCandidate = [&](const std::string &newForm)
    {
        if (newForm.empty() || seenForms.count(newForm))
            return;
        seenForms.insert(newForm);
        results.push_back(newForm);
    };

    for (const std::string &form : candidates)
    {
        // Pass-through: always preserve the current form as a baseline candidate
        addCandidate(form);

        // -------------------------------------------------------------
        // 1. Chhatva Inversion (च्छ् -> म् / ष्)
        // Stems ending in -च्छ् (e.g., "गच्छ्", "इच्छ्", "पृच्छ्")
        // -------------------------------------------------------------
        std::string cchMarker = "च्छ्";
        if (endsWithUTF8(form, cchMarker))
        {
            std::string prefix = form.substr(0, form.size() - cchMarker.size());

            // Path A: -च्छ् -> -म् (e.g., "गच्छ्" -> "गम्")
            addCandidate(prefix + "म्");

            // Path B: -च्छ् -> -ष् (e.g., "इच्छ्" -> "इष्")
            addCandidate(prefix + "ष्");
        }

        // -------------------------------------------------------------
        // 2. Class 7 (Rudhādi) Internal Nasal Infix Inversion
        // Removes inserted internal nasal before final consonant
        // e.g., "रुन्ध्" -> "रुध्", "भिन्दू" -> "भिद्", "भinत्" -> "भिद्"
        // -------------------------------------------------------------
        std::string unnasalized = removeNasalInfix(form);
        if (unnasalized != form)
        {
            addCandidate(unnasalized);

            // Revert terminal unvoicing if present (e.g., -त् -> -द्)
            if (endsWithUTF8(unnasalized, "त्"))
            {
                std::string voiced =
                    unnasalized.substr(0, unnasalized.size() - std::string("त्").size()) + "द्";
                addCandidate(voiced);
            }
        }

        // -------------------------------------------------------------
        // 3. Boundary Unvoicing Inversion (Hard -> Soft consonants)
        // Terminal -त् -> -द्, -क् -> -ग्
        // -------------------------------------------------------------
        if (endsWithUTF8(form, "त्"))
        {
            std::string voiced =
                form.substr(0, form.size() - std::string("त्").size()) + "द्";
            addCandidate(voiced);
        }
        else if (endsWithUTF8(form, "क्"))
        {
            std::string voiced =
                form.substr(0, form.size() - std::string("क्").size()) + "ग्";
            addCandidate(voiced);
        }
    }

    return results;
}

// Helper to replace all occurrences of a UTF-8 substring
static std::string replaceAll(std::string str, const std::string &from,
                              const std::string &to)
{
    size_t startPos = 0;
    while ((startPos = str.find(from, startPos)) != std::string::npos)
    {
        str.replace(startPos, from.length(), to);
        startPos += to.length();
    }
    return str;
}

std::vector<std::string>
RootDeriver::reverseGuna(const std::vector<std::string> &candidates)
{
    std::vector<std::string> results;
    std::unordered_set<std::string> seenForms;

    auto addCandidate = [&](const std::string &newForm)
    {
        if (newForm.empty() || seenForms.count(newForm))
            return;
        seenForms.insert(newForm);
        results.push_back(newForm);
    };

    for (const std::string &form : candidates)
    {
        // Pass-through: preserve the current form as a baseline
        addCandidate(form);

        // -------------------------------------------------------------
        // 1. Root-Final Guṇa Reversal (-av -> -ū/-u, -ay -> -ī/-i, -ar -> -ṛ)
        // -------------------------------------------------------------

        // Path 1A: Trailing -व् (av) -> -ू / -ु  (e.g., "भव्" -> "भू" / "भु")
        if (endsWithUTF8(form, "व्"))
        {
            std::string stem =
                form.substr(0, form.size() - std::string("व्").size());
            addCandidate(stem + "ू"); // Long ū (e.g., भू)
            addCandidate(stem + "ु"); // Short u (e.g., भु)
        }

        // Path 1B: Trailing -य् (ay) -> -ी / -ि  (e.g., "जय्" -> "जी" / "जि", "नय्" -> "नी")
        if (endsWithUTF8(form, "य्"))
        {
            std::string stem =
                form.substr(0, form.size() - std::string("य्").size());
            addCandidate(stem + "ी"); // Long ī (e.g., नी, जी)
            addCandidate(stem + "ि"); // Short i (e.g., जि)
        }

        // Path 1C: Trailing -र् (ar) -> -ृ / -ॄ  (e.g., "स्मर्" -> "स्मृ", "तर्" -> "तॄ")
        if (endsWithUTF8(form, "र्"))
        {
            std::string stem =
                form.substr(0, form.size() - std::string("र्").size());
            addCandidate(stem + "ृ"); // Short ṛ (e.g., स्मृ, कृ)
            addCandidate(stem + "ॄ"); // Long ṝ (e.g., तॄ)
        }

        // -------------------------------------------------------------
        // 2. Medial / Internal Guṇa Vowel Reversal
        // -------------------------------------------------------------

        // Path 2A: Medial -ो- (o) -> -ु- / -ू-  (e.g., "बोध्" -> "बुध्")
        if (form.find("ो") != std::string::npos)
        {
            addCandidate(replaceAll(form, "ो", "ु"));
            addCandidate(replaceAll(form, "ो", "ू"));
        }

        // Path 2B: Medial -े- (e) -> -ि- / -ी-  (e.g., "चेत्" -> "चित्")
        if (form.find("े") != std::string::npos)
        {
            addCandidate(replaceAll(form, "े", "ि"));
            addCandidate(replaceAll(form, "े", "ी"));
        }

        // Path 2C: Internal -र्- before consonant -> -ृ- (e.g., "वर्ध्" -> "वृध्")
        size_t rPos = form.find("र्");
        if (rPos != std::string::npos && rPos > 0 &&
            rPos + std::string("र्").size() < form.size())
        {
            std::string arReversed = form;
            arReversed.replace(rPos, std::string("र्").size(), "ृ");
            addCandidate(arReversed);
        }
    }

    return results;
}