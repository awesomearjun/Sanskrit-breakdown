#include "sandhi-splitter.hpp"
#include "wordTypes.hpp"
#include <cstddef>
#include <string>
#include <vector>

void SandhiSplitter::initializePossibilities()
{
    rules = {

        // === 1. DĪRGHA (Vowel Elongation) ===
        {"ा",
         {
             {"", "अ"},  // Variant A: e.g., पुस्तक + आलय (if right side handles
                         // 'अ'/'आ')
             {"", "आ"},  // Variant B
             {"ा", "अ"}, // Variant C
             {"ा", "आ"}  // Variant D
         }},

        {"ी",
         {{"", "इ"}, // e.g., गिरि + ईश
          {"", "ई"},
          {"ी", "इ"},
          {"ी", "ई"}}},

        {"ू",
         {{"", "उ"}, // e.g., साधु + उक्तम्
          {"", "ऊ"},
          {"ू", "उ"},
          {"ू", "ऊ"}}},

        // === 2. GUṆA & VṚDDHI (Vowel Blending) ===
        {"ो",
         {{"", "उ"},  // e.g., हित + उपदेशः -> हितोपदेशः
          {"ा", "उ"}, // e.g., महा + उत्सवः -> महोत्सवः
          {"", "ऊ"},
          {"ा", "ऊ"},
          {"ः", "अ"}}},

        {"े",
         {{"", "इ"},  // e.g., नर + इन्द्र -> नरेन्द्र
          {"ा", "इ"}, // e.g., महा + इन्द्र -> महेन्द्र
          {"", "ई"},
          {"ा", "ई"}}},

        {"ै",
         {{"", "ए"},  // e.g., तव + एव -> तवैव
          {"ा", "ए"}, // e.g., तथा + एव -> तथैव
          {"", "ऐ"},
          {"ा", "ऐ"}}},

        {"ौ",
         {{"", "ओ"},  // e.g., जल + ओघः -> जलौघः
          {"ा", "ओ"}, // e.g., महा + ओषधि -> महौषधि
          {"", "औ"},
          {"ा", "औ"}}},

        // === 3. VISARGA (Crucial for Nominal Endings) ===
        {"ोऽ",
         {
             {"ः", "अ"} // Avagraha: e.g., अधर्मोऽस्ति -> अधर्मः + अस्ति
         }},

        {"श्च",
         {
             {"ः", "च"} // Palatal: e.g., रामश्च -> रामः + च
         }},

        {"श्छ", {{"ः", "छ"}}},

        {"स्त",
         {
             {"ः", "त"} // Dental: e.g., नमस्ते -> नमः + ते
         }},

        {"स्थ", {{"ः", "थ"}}},

        {"ष्ट",
         {
             {"ः", "ट"} // Retroflex: e.g., धनुष्टङ्कार -> धनुः + टङ्कार
         }},

        {"ष्ठ", {{"ः", "ठ"}}},

        // === 4. YAN (Semi-vowel Conversion) ===
        {"्य",
         {{"ि", "अ"}, // e.g., यदि + अपि -> यद्यपि
          {"ी", "अ"}}},
        {"्या",
         {{"ि", "आ"}, // e.g., इति + आदि -> इत्यादि
          {"ी", "आ"}}},
        {"्यु",
         {{"ि", "उ"}, // e.g., प्रति + उत्तरम् -> प्रत्युत्तरम्
          {"ी", "उ"}}},
        {"्यू",
         {{"ि", "ऊ"}, // e.g., प्रति + ऊषः -> प्रत्यूषः
          {"ी", "ऊ"}}},
        {"्ये",
         {{"ि", "ए"}, // e.g., प्रति + एकम् -> प्रत्येकम्
          {"ी", "ए"}}},
        {"्यै",
         {{"ि", "ऐ"}, // e.g., अति + ऐश्वर्यम् -> अत्यैश्वर्यम्
          {"ी", "ऐ"}}},
        {"्व",
         {{"ु", "अ"}, // e.g., सु + अल्प -> स्वल्प
          {"ू", "अ"}}},
        {"्वा",
         {{"ु", "आ"}, // e.g., सु + आगतम् -> स्वागतम्
          {"ू", "आ"}}},
        {"्वि",
         {{"ु", "इ"}, // e.g., अनु + इत -> अन्वित
          {"ू", "इ"}}},
        {"्वे",
         {{"ु", "ए"}, // e.g., अनु + एषणम् -> अन्वेषणम्
          {"ू", "ए"}}},
        {"त्र",
         {
             {"ृ", "अ"} // e.g., पितृ + अनुमति -> पित्रनुमति
         }},
        {"त्रा",
         {
             {"ृ", "आ"} // e.g., पितृ + आज्ञा -> पित्राज्ञा
         }},
        {"त्रि",
         {
             {"ृ", "इ"} // e.g., भ्रातृ + इति -> भ्रात्रिति
         }},
    };
}

std::vector<TokenAnalysis> SandhiSplitter::split(const Tokens &tokens)
{
    std::vector<TokenAnalysis> output;

    for (std::string token : tokens)
    {
        if (std::vector<TokenAnalysis> possibilities = findSplits(token);
            !possibilities.empty())
        {
            for (TokenAnalysis possibility : possibilities)
                output.push_back(possibility);
        }
    }
    return output;
}

std::vector<TokenAnalysis> SandhiSplitter::findSplits(const std::string &token)
{
    std::vector<TokenAnalysis> results;
    std::string targetString = token;

    // loop until we break (when we hit the token's end)
    while (true)
    {
    restart:
        if (targetString.length() == 0)
            break;

        // loop over every character
        for (size_t pos = 0; pos < targetString.length(); ++pos)
        {

            // Scan up to 9 bytes to capture multi-byte modifiers and conjuncts
            for (size_t len = 1;
                 len <= 12 && (pos + len) <= targetString.length(); ++len)
            {
                std::string potentialJunction = targetString.substr(pos, len);

                // Look up the exact byte slice in our rules matrix
                auto it = rules.find(potentialJunction);
                if (it != rules.end())
                {

                    // Isolate the text completely before and after this
                    // junction
                    std::string leftChunk = targetString.substr(0, pos);
                    std::string rightChunk = targetString.substr(pos + len);

                    // Iterate through all potential left/right replacement
                    // pairs for this junction
                    const auto &transformations = it->second;
                    for (const auto &[leftRep, rightRep] : transformations)
                    {

                        std::string candidateLeft = leftChunk + leftRep;
                        std::string candidateRight = rightRep + rightChunk;

                        // Hit the validation database layer
                        WordAnalysis leftAnalysis = isValidWord(candidateLeft);
                        WordAnalysis rightAnalysis =
                            isValidWord(candidateRight);
                        if (leftAnalysis.success && rightAnalysis.success)
                        {
                            results.push_back(
                                TokenAnalysis(candidateLeft, leftAnalysis));
                            targetString = candidateRight;

                            // restart all the way until the while loop
                            goto restart;
                        }
                    }
                }
            }

            // if it didn't restart, then just stop cuz we don't have a split
            break;
        }
    }

    // if this entire thing didn't yield any matches, just test the word itself
    if (results.empty())
    {
        WordAnalysis finalAnalysis = isValidWord(token);
        results.push_back(TokenAnalysis(token, finalAnalysis));
    }
    return results;
}

WordAnalysis SandhiSplitter::isValidWord(const std::string &word)
{
    // 1. Is it a standalone indeclinable word (like च, अपि, इति)?
    if (db.isIndeclinable(word))
    {
        return WordAnalysis(true, {word}, VerbMetadata(), NominalMetadata(),
                            "indeclinable");
    }

    // the rest of the word (used in the next steps)
    std::string rest;

    // loops through at least 4 aksharas
    for (size_t len = 1; len <= 12; len++)
    {
        if (len > word.size())
        {
            return WordAnalysis(false, {}, VerbMetadata(), NominalMetadata(),
                                "none");
        }

        // 2. Is it a recognized prefix (upasarga like सु, अनु, प्रति)?
        if (std::string prefix = word.substr(0, len); db.isPrefix(prefix))
        {
            rest = word.substr(len);
            return WordAnalysis(true, {prefix, rest}, VerbMetadata(),
                                NominalMetadata(), "none");
        }
        // 3. Is it a valid verbal suffix?
        else if (std::string vsuffix = word.substr(word.length() - len, len);
                 db.tryMatchVerbalSuffix(vsuffix, collectedVerbInfo))
        {
            rest = word.substr(0, word.length() - len);
            return WordAnalysis(true, {rest, vsuffix}, collectedVerbInfo,
                                NominalMetadata(), "verb");
        }
        // 4. Is it a valid inflected nominal form (noun/adjective case ending)?
        else if (std::string nsuffix = word.substr(word.length() - len, len);
                 db.tryMatchNominalSuffix(nsuffix, collectedNominalInfo))
        {
            rest = word.substr(0, word.length() - len);
            return WordAnalysis(true, {rest, nsuffix}, VerbMetadata(),
                                collectedNominalInfo, "nominal");
        }
    }

    // If it fails all 5 of your specific morphological checks, it's a ghost
    // word.
    return WordAnalysis(false, {}, VerbMetadata(), NominalMetadata(), "none");
}

VerbMetadata SandhiSplitter::getVerbInfo() { return collectedVerbInfo; }

NominalMetadata SandhiSplitter::getNominalInfo()
{
    return collectedNominalInfo;
}
