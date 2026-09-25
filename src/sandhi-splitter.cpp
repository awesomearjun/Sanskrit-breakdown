#include "sandhi-splitter.hpp"
#include "wordTypes.hpp"
#include <cstddef>
#include <memory>
#include <optional>
#include <string>
#include <string_view>
#include <unordered_map>
#include <utility>
#include <vector>

using namespace SandhiSplit;

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

        {"ो",
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

        // === 4. HAL (Consonant Sandhi) ===
        {"द्य", {{"त्", "य"}, {"द्", "य"}}},
        {"द्या", {{"त्", "या"}, {"द्", "या"}}},
        {"च्चि",
         {{"त्", "चि"}, // Handles cases where vowel mark 'ि' follows
          {"द्", "चि"}}},
        {"च्च",
         {
             {"त्", "च"}, // Ścutva: e.g., तत् + चिन्तयति -> तच्चिन्तयति
             {"द्", "च"}  // Ścutva variant
         }},
        {"ज्ज",
         {{"त्", "ज"}, // Ścutva: e.g., सत_ + जनः -> सज्जनः
          {"द्", "ज"}}},
        {"च्छा",
         {{"त्", "छा"}, // Chatva: e.g., तद् + शिवः -> तच्छिवः
          {"त्", "शा"}}},
        {"द्ग",
         {
             {"त्", "ग"} // JŚatva: e.g., जगत् + ईशः -> जगदीशः (voicing t -> d)
         }},
        {"द्ब",
         {
             {"त्", "ब"} // JŚatva: e.g., तत् + बोधः -> तद्बोधः
         }},
        {"न्म",
         {
             {"त्", "म"} // Anunāsika: e.g., जगत् + नाथः -> जगन्नाथः (or -m)
         }},
        {"न्न",
         {
             {"त्", "न"} // Anunāsika: e.g., एतत् + न -> एतन्न
         }},
        {"ंका",
         {
             {"म्", "क"}
             // Parasavarṇa / Anusvāra: e.g., सम् + कल्पः -> सङ्कल्पः / संकल्पः
         }},
        {"ंता",
         {
             {"म्", "त"} // Parasavarṇa: e.g., सम् + तोषः -> सन्तोषः / संतोषः
         }}};
}

WordList SandhiSplitter::splitTree(const WordList &tokens)
{
    WordList output;

    for (const std::shared_ptr<Word> &token : tokens)
    {
        MultiPathMemo memo;

        if (std::vector<SplitPath> tokenPossibilities =
                findSplits(token->text, memo);
            !tokenPossibilities.empty())
        {
            std::shared_ptr<Word> newWord = std::move(token);
            newWord->metadata = std::move(tokenPossibilities);
            output.push_back(newWord);
        }
    }
    return output;
}

// tests for incomplete bit
inline bool isUtf8ContinuationByte(char c)
{
    return (static_cast<unsigned char>(c) & 0xC0) == 0x80;
}

// Returns true if the byte starting at str[i] is a combining mark/matra/virama
// sees if there's a dependant mark on the next character
bool isCombiningMark(const std::string &str, size_t i)
{
    if (i + 2 >= str.size())
        return false;

    unsigned char c1 = static_cast<unsigned char>(str[i]);
    unsigned char c2 = static_cast<unsigned char>(str[i + 1]);
    unsigned char c3 = static_cast<unsigned char>(str[i + 2]);

    // Devanagari Matras, Virama, and Modifiers sit in 0xE0 0xA5 0x80 to 0xE0
    // 0xA5 0xBF
    if (c1 == 0xE0 && c2 == 0xA5 && (c3 >= 0x80 && c3 <= 0xBF))
        return true;
    // Dependent Vowels (e.g., Aa, I, Ii) sit in 0xE0 0xA4 0xBE to 0xE0 0xA4
    // 0xC0
    if (c1 == 0xE0 && c2 == 0xA4 && (c3 >= 0xBE))
        return true;
    return false;
}

std::string replaceTrailingAnusvara(const std::string &str)
{
    // UTF-8 byte sequences for Devanagari characters
    const std::string anusvara = "\xE0\xA4\x82"; // 'ं' (U+0902)
    const std::string makara = "म्";              // 'म्' (U+092E U+094D)

    // Check if string ends with 'ं' (3 bytes)
    if (str.size() >= anusvara.size() &&
        str.compare(str.size() - anusvara.size(), anusvara.size(), anusvara) ==
            0)
    {

        // Return string without the last 3 bytes, plus 'म्'
        return str.substr(0, str.size() - anusvara.size()) + makara;
    }

    return str; // Return unchanged if it doesn't end with Anusvāra
}

std::vector<SplitPath> SandhiSplitter::findSplits(const std::string &token,
                                                  MultiPathMemo &memo)
{
    std::vector<SplitPath> results;

    // Base Case: Empty string returns empty vector
    if (token.empty())
        return {};

    // Memo Check: Have we already built the trees for this exact string?
    if (auto it = memo.find(token); it != memo.end())
        return it->second;

    for (size_t pos = 0; pos < token.length(); ++pos)
    {
        // doesn't let incomplete bits get through
        if (isUtf8ContinuationByte(token[pos]))
            continue;
        for (size_t len = 1; len <= 12 && (pos + len) <= token.length(); ++len)
        {
            // still doesn't let them through
            if (pos + len < token.length() &&
                isUtf8ContinuationByte(token[pos + len]))
            {
                continue;
            }

            // skip if next character is a dependant one
            if (size_t endPos = pos + len;
                endPos < token.length() && isCombiningMark(token, endPos))
            {
                continue; // Skip! Don't let rightChunk inherit a stranded
                          // matra like 'ु'
            }

            std::string potentialJunction = token.substr(pos, len);

            auto it = rules.find(potentialJunction);
            if (it == rules.end())
                continue;

            std::string leftChunk = token.substr(0, pos);
            std::string rightChunk = token.substr(pos + len);

            const auto &transformations = it->second;
            for (const auto &[leftRep, rightRep] : transformations)
            {
                std::string candidateLeft = leftChunk + leftRep;
                std::string candidateRight = rightRep + rightChunk;

                // if we add more mods in future then we can just easily add it
                // here
                std::string modLeft = replaceTrailingAnusvara(candidateLeft);
                std::string modRight = replaceTrailingAnusvara(candidateRight);

                // 1. Validate LEFT side
                std::vector<WordMetadata> leftAnalyses = isValidWord(modLeft);
                if (leftAnalyses.empty())
                    continue;

                // 2. Validate RIGHT side (recursively)
                auto rightPaths = findSplits(modRight, memo);
                if (rightPaths.empty())
                {
                    continue; // Right remainder cannot be validly parsed, drop
                              // this split path
                }

                // combine vectors
                for (const auto &leftAnalysis : leftAnalyses)
                {
                    for (const auto &rightPath : rightPaths)
                    {
                        SplitPath combinedPath;
                        combinedPath.push_back(leftAnalysis); // Left word
                        combinedPath.insert(
                            combinedPath.end(), rightPath.begin(),
                            rightPath.end()); // Rest of sequence

                        results.push_back(std::move(combinedPath));
                    }
                }
            }
        }
    }

    // Cleanup / Fallback: Add whole token as node regardless of anything
    std::string modToken = replaceTrailingAnusvara(token);
    std::vector<WordMetadata> direct = isValidWord(modToken);

    results.push_back(direct);

    return memo[token] = results;
}
// Returns byte offsets for the start of each Sanskrit Akshara (Grapheme
// Cluster)
static std::vector<size_t> getAksharaByteOffsets(std::string_view str)
{
    std::vector<size_t> offsets;
    if (str.empty())
        return {0};

    offsets.push_back(0);

    bool prevWasVirama = false;
    size_t i = 0;

    while (i < str.size())
    {
        uint32_t cp = 0;
        unsigned char c = static_cast<unsigned char>(str[i]);
        size_t cpLen = 1;

        if ((c & 0x80) == 0)
        {
            cp = c;
            cpLen = 1;
        }
        else if ((c & 0xE0) == 0xC0)
        {
            cp = ((c & 0x1F) << 6) | (str[i + 1] & 0x3F);
            cpLen = 2;
        }
        else if ((c & 0xF0) == 0xE0)
        {
            cp = ((c & 0x0F) << 12) | ((str[i + 1] & 0x3F) << 6) |
                 (str[i + 2] & 0x3F);
            cpLen = 3;
        }
        else if ((c & 0xF8) == 0xF0)
        {
            cp = ((c & 0x07) << 18) | ((str[i + 1] & 0x3F) << 12) |
                 ((str[i + 2] & 0x3F) << 6) | (str[i + 3] & 0x3F);
            cpLen = 4;
        }

        // Combining marks: Matras (U+093A-094C, U+094E-0954), Anusvara/Visarga
        // (U+0900-0903)
        bool isCombining = (cp >= 0x093A && cp <= 0x0954 && cp != 0x094D) ||
                           (cp >= 0x0900 && cp <= 0x0903);
        bool isVirama = (cp == 0x094D); // Halant (्)

        // If it's a new base consonant/independent vowel and NOT preceded by a
        // Virama, mark a new Akshara
        if (i > 0 && !isCombining && !isVirama)
        {
            if (!prevWasVirama)
                offsets.push_back(i);
        }

        prevWasVirama = isVirama;
        i += cpLen;
    }

    offsets.push_back(str.size());
    return offsets;
}

std::vector<WordMetadata> SandhiSplitter::isValidWord(const std::string &word)
{
    std::vector<WordMetadata> validInterpretations;

    if (word.empty())
        return validInterpretations;

    // 1. Standalone Indeclinable Check (e.g., "अनु" or "अत्र")
    if (auto buf = db.isIndeclinable(word); buf.has_value())
    {
        WordMetadata avyayaAnalysis;
        avyayaAnalysis.success = true;
        avyayaAnalysis.original = word;
        avyayaAnalysis.matchType = WordMatchType::INDECLINABLE;
        avyayaAnalysis.metadata = std::move(buf);

        validInterpretations.push_back(std::move(avyayaAnalysis));
    }

    // Direct Bare Nominal Stem Check (e.g., "विद्या")
    if (auto buf = db.stemExists(word); buf.has_value())
    {
        CoreMetadata stemComp;
        stemComp.success = true;
        stemComp.original = word;
        stemComp.metadata = std::move(buf);
        stemComp.matchType = CoreMatchType::STEM;

        WordMetadata directNominal;
        directNominal.success = true;
        directNominal.original = word;
        directNominal.matchType = WordMatchType::NOMINAL;
        directNominal.cores.push_back(std::move(stemComp));

        validInterpretations.push_back(std::move(directNominal));
    }

    // Compute code point byte boundaries
    std::string_view wordView(word);
    const std::vector<size_t> offsets = getAksharaByteOffsets(wordView);
    const size_t charCount = offsets.size() - 1; // Total Unicode code points

    // Iterate by logical character lengths (1 to 6 code points)
    for (size_t charLen = 1; charLen <= 6 && charLen <= charCount; ++charLen)
    {
        const size_t splitCharIdx = charCount - charLen;
        const size_t splitBytePos = offsets[splitCharIdx];

        std::string_view baseView = wordView.substr(0, splitBytePos);
        std::string_view suffixView = wordView.substr(splitBytePos);

        std::string suffixStr(suffixView);
        std::string candidateBaseStr(baseView);

        // 2. Verbal Suffix Match
        if (auto verbInfo = db.tryMatchVerbalSuffix(suffixStr);
            verbInfo.has_value())
        {
            if (!candidateBaseStr.empty())
            {
                for (const auto &suffix : *verbInfo)
                {
                    WordMetadata verbAnalysis;
                    verbAnalysis.success = true;
                    verbAnalysis.original = word;
                    verbAnalysis.matchType = WordMatchType::VERB;

                    VerbMetadata vMeta = suffix;
                    vMeta.suffix = suffixStr;
                    vMeta.stem = candidateBaseStr;

                    verbAnalysis.metadata = std::move(vMeta);
                    validInterpretations.push_back(std::move(verbAnalysis));
                }
            }
        }

        // 3. Nominal Suffix Match
        if (auto nominalInfo = db.tryMatchNominalSuffix(suffixStr);
            nominalInfo.has_value())
        {
            if (auto stemBuf = db.stemExists(candidateBaseStr);
                stemBuf.has_value())
            {
                for (const auto &suffix : *nominalInfo)
                {
                    WordMetadata nominalAnalysis;
                    nominalAnalysis.success = true;
                    nominalAnalysis.original = word;
                    nominalAnalysis.matchType = WordMatchType::NOMINAL;

                    NominalMetadata nAnalysis = suffix;
                    nAnalysis.suffix = suffixStr;
                    nAnalysis.stem = candidateBaseStr;
                    nominalAnalysis.metadata = std::move(nAnalysis);

                    CoreMetadata stemCore;
                    stemCore.success = true;
                    stemCore.original = candidateBaseStr;
                    stemCore.matchType = CoreMatchType::STEM;
                    stemCore.metadata = stemBuf;

                    nominalAnalysis.cores.push_back(std::move(stemCore));
                    validInterpretations.push_back(std::move(nominalAnalysis));
                }
            }
            // secondary suffix check
            else
            {
                auto secEndingInfo = db.secondaryEndingExists(suffixStr);
                if (!secEndingInfo.has_value())
                    continue;

                // 2. Check if the underlying primary stem exists
                auto underlyingStem = db.stemExists(candidateBaseStr);
                if (!underlyingStem.has_value())
                    continue;

                // 3. Both stem and secondary ending exist: Build
                // interpretations
                for (const auto &nominal : *nominalInfo)
                {
                    for (const auto &secEnding : *secEndingInfo)
                    {
                        WordMetadata nominalAnalysis;
                        nominalAnalysis.success = true;
                        nominalAnalysis.original = word;
                        nominalAnalysis.matchType = WordMatchType::NOMINAL;

                        NominalMetadata nAnalysis = std::move(nominal);
                        nAnalysis.suffix = suffixStr;
                        nAnalysis.stem =
                            candidateBaseStr; // Derived secondary stem
                        nominalAnalysis.metadata = std::move(nAnalysis);

                        // Add Primary Stem to Cores
                        CoreMetadata primaryCore;
                        primaryCore.success = true;
                        primaryCore.original = candidateBaseStr;
                        primaryCore.matchType = CoreMatchType::STEM;
                        primaryCore.metadata = underlyingStem;
                        nominalAnalysis.cores.push_back(std::move(primaryCore));

                        // Add Secondary Ending to Cores
                        CoreMetadata secondaryCore;
                        secondaryCore.success = true;
                        secondaryCore.original = suffixStr;
                        secondaryCore.matchType =
                            CoreMatchType::SECONDARY_ENDING;
                        secondaryCore.metadata = secEnding;
                        nominalAnalysis.cores.push_back(
                            std::move(secondaryCore));

                        validInterpretations.push_back(
                            std::move(nominalAnalysis));
                    }
                }
            }
        }
    }

    // upasarga (prefix) check
    // seperated so that stems with valid prefixes (eg vinaya) still register
    for (size_t charLen = 1; charLen <= 4 && charLen < charCount; ++charLen)
    {
        const size_t prefixBytePos = offsets[charLen];
        std::string_view prefixView = wordView.substr(0, prefixBytePos);
        std::string_view remainingView = wordView.substr(prefixBytePos);

        std::string candidatePrefixStr(prefixView);
        std::string remainingStemStr(remainingView);

        if (auto prefix = db.isPrefix(candidatePrefixStr); prefix.has_value())
        {
            std::vector<WordMetadata> stemResults =
                isValidWord(remainingStemStr);

            for (const auto &stemMetadata : stemResults)
            {
                WordMetadata combined = stemMetadata;
                combined.original = word;

                if (combined.metadata.has_value())
                {
                    auto &variantMeta = combined.metadata.value();

                    if (auto *vMeta = std::get_if<VerbMetadata>(&variantMeta))
                    {
                        vMeta->prefix = prefix.value();
                    }
                    else if (auto *nMeta =
                                 std::get_if<NominalMetadata>(&variantMeta))
                    {
                        nMeta->prefix = prefix.value();
                    }
                }

                validInterpretations.push_back(std::move(combined));
            }
        }
    }

    return validInterpretations;
}
