#include "sandhi-splitter.hpp"
#include "wordTypes.hpp"
#include <cstddef>
#include <memory>
#include <string>
#include <unordered_map>
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
         {"च्चि",
         {
             {"त्", "चि"}, // Handles cases where vowel mark 'ि' follows
             {"द्", "चि"}
         }},
        {"च्च",
         {
             {"त्", "च"},  // Ścutva: e.g., तत् + चिन्तयति -> तच्चिन्तयति
             {"द्", "च"}   // Ścutva variant
         }},
        {"ज्ज",
         {
             {"त्", "ज"},  // Ścutva: e.g., सत_ + जनः -> सज्जनः
             {"द्", "ज"}
         }},
        {"च्छा",
         {
             {"त्", "छा"}, // Chatva: e.g., तद् + शिवः -> तच्छिवः
             {"त्", "शा"}
         }},
        {"द्ग",
         {
             {"त्", "ग"}   // JŚatva: e.g., जगत् + ईशः -> जगदीशः (voicing t -> d)
         }},
        {"द्ब",
         {
             {"त्", "ब"}   // JŚatva: e.g., तत् + बोधः -> तद्बोधः
         }},
        {"न्म",
         {
             {"त्", "म"}   // Anunāsika: e.g., जगत् + नाथः -> जगन्नाथः (or -m)
         }},
        {"न्न",
         {
             {"त्", "न"}   // Anunāsika: e.g., एतत् + न -> एतन्न
         }},
        {"ंका",
         {
             {"म्", "क"}   // Parasavarṇa / Anusvāra: e.g., सम् + कल्पः -> सङ्कल्पः / संकल्पः
         }},
        {"ंता",
         {
             {"म्", "त"}   // Parasavarṇa: e.g., सम् + तोषः -> सन्तोषः / संतोषः
         }}
    };
}

std::vector<std::vector<std::shared_ptr<WordAnalysisNode>>>
SandhiSplitter::splitTree(std::vector<std::string> &tokens)
{
    std::vector<std::vector<std::shared_ptr<WordAnalysisNode>>> output;

    for (const std::string &token : tokens)
    {
        AnalysisTreeCache memo;

        if (std::vector<std::shared_ptr<WordAnalysisNode>> tokenTrees =
                findSplits(token, memo);
            !tokenTrees.empty())
        {
            output.push_back(tokenTrees);
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

std::vector<std::shared_ptr<WordAnalysisNode>>
SandhiSplitter::findSplits(const std::string &token, AnalysisTreeCache &memo)
{
    std::vector<std::shared_ptr<WordAnalysisNode>> results;
    std::string targetString = token;

    // Base Case: Empty string returns empty vector
    if (token.empty())
        return {};

    // Memo Check: Have we already built the trees for this exact string?
    if (auto it = memo.find(token); it != memo.end())
        return it->second;

    for (size_t pos = 0; pos < targetString.length(); ++pos)
    {
        // doesn't let incomplete bits get through
        if (isUtf8ContinuationByte(targetString[pos]))
            continue;
        for (size_t len = 1; len <= 12 && (pos + len) <= targetString.length();
             ++len)
        {
            // still doesn't let them through
            if (pos + len < targetString.length() &&
                isUtf8ContinuationByte(targetString[pos + len]))
            {
                continue;
            }

            // skip if next character is a dependant one
            if (size_t endPos = pos + len;
                endPos < targetString.length() &&
                isCombiningMark(targetString, endPos))
            {
                continue; // Skip! Don't let rightChunk inherit a stranded
                          // matra like 'ु'
            }

            std::string potentialJunction = targetString.substr(pos, len);

            auto it = rules.find(potentialJunction);
            if (it == rules.end())
                continue;

            std::string leftChunk = targetString.substr(0, pos);
            std::string rightChunk = targetString.substr(pos + len);

            const auto &transformations = it->second;
            for (const auto &[leftRep, rightRep] : transformations)
            {
                std::string candidateLeft = leftChunk + leftRep;
                std::string candidateRight = rightRep + rightChunk;

                WordAnalysis leftAnalysis;
                leftAnalysis.original = candidateLeft;

                auto node = std::make_shared<WordAnalysisNode>(leftAnalysis);
                node->children = findSplits(candidateRight, memo);

                results.push_back(node);
            }
        }
    }

    // Cleanup / Fallback: If no splits were found at all, add the whole token
    // as node
    if (results.empty())
    {
        WordAnalysis buf;
        buf.original = token;
        results.push_back(std::make_shared<WordAnalysisNode>(WordAnalysisNode(buf)));
    }

    return memo[token] = results;
}
