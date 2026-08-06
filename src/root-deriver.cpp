#include "root-deriver.hpp"
#include "wordTypes.hpp"
#include <vector>

using namespace RootDerivation;

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
