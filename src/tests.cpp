#include "tests.hpp"

#include <cstddef>
#include <iostream>
#include <vector>

#include "database.hpp"
#include "sandhi-splitter.hpp"
#include "tokenizer.hpp"

#define DEBUG_INFO false

void Tests::TestAll()
{
    TestDatabaseFunctions();
    TestTokenizer();
    TestSplitter();
    TestValidWord();
}

Tests::Tests()
{
    db.initialize("./roots.json", "./grammar_constants.json", "./stems.json");
}

template <typename T>
void Tests::ExpectEqual(const T &expected, const T &actual,
                        const std::string &message, const bool printable)
{
    if (expected != actual)
    {
        std::cerr << "-" << std::endl;
        std::cerr << "[FAIL]: " << message << std::endl;
        if (printable)
        {
            std::cerr << "Expected: " << expected << std::endl;
            std::cerr << "Actual:   " << actual << std::endl;
        }
        else
        {
            std::cerr << "Expected: [non-printable]" << std::endl;
            std::cerr << "Actual:   [non-printable]" << std::endl;
        }
        std::cerr << "-" << std::endl;
    }
    else
    {
        std::cout << "[PASS]: " << message << std::endl;
    }
}

void Tests::TestDatabaseFunctions()
{
    // Test rootExists
    ExpectEqual(true, db.rootExists("वद्"), "Root 'वद्' should exist");
    ExpectEqual(false, db.rootExists("fd"), "Root 'fd' should not exist");
    ExpectEqual(true, db.rootExists("सद्"), "Root 'सद्' should exist");
    ExpectEqual(false, db.rootExists("अस्ति"), "Root 'अस्ति' should not exist");
    ExpectEqual(true, db.rootExists("यज्"), "Root 'यज्' should exist");
    ExpectEqual(false, db.rootExists("वृतु"), "Root 'वृतु' should not exist");

    // Test isPrefix
    ExpectEqual(true, db.isPrefix("अ"), "Prefix 'अ' should exist");
    ExpectEqual(false, db.isPrefix("xyz"), "Prefix 'xyz' should not exist");
    ExpectEqual(true, db.isPrefix("परा"), "Prefix 'परा' should exist");
    ExpectEqual(false, db.isPrefix("झट्वङ्ग्क्षू"),
                "Prefix 'झट्वङ्ग्क्षू' should not exist");

    // Test isIndeclinable
    ExpectEqual(true, db.isIndeclinable("च"), "Indeclinable 'च' should exist");
    ExpectEqual(false, db.isIndeclinable("xyz"),
                "Indeclinable 'xyz' should not exist");
    ExpectEqual(true, db.isIndeclinable("न"), "Indeclinable 'न' should exist");
    ExpectEqual(false, db.isIndeclinable("अस्ति"),
                "Indeclinable 'अस्ति' should not exist");
    ExpectEqual(true, db.isIndeclinable("हि"),
                "Indeclinable 'हि' should exist");

    // Test tryMatchVerbalSuffix
    VerbMetadata verbMeta;
    ExpectEqual(true, db.tryMatchVerbalSuffix("ति", verbMeta),
                "Verbal suffix 'ति' should match");
    ExpectEqual(std::string("ति"), verbMeta.suffix,
                "Verbal suffix 'ति' should have correct metadata");
    ExpectEqual(static_cast<int>(VerbTenseOrMood::PRESENT),
                static_cast<int>(verbMeta.tenseOrMood),
                "Verbal suffix 'ति' should have correct tense/mood", false);
    ExpectEqual(false, db.tryMatchVerbalSuffix("xyz", verbMeta),
                "Verbal suffix 'xyz' should not match");

    // Test tryMatchNominalSuffix
    NominalMetadata nominalMeta;
    ExpectEqual(true, db.tryMatchNominalSuffix("स्य", nominalMeta),
                "Nominal suffix 'स्य' should match");
    ExpectEqual(std::string("स्य"), nominalMeta.suffix,
                "Nominal suffix 'स्य' should have correct metadata");
    ExpectEqual(false, db.tryMatchNominalSuffix("xyz", nominalMeta),
                "Nominal suffix 'xyz' should not match");
}

void Tests::TestTokenizer()
{
    std::string shlokaInput =
        "न जायते  म्रियते  वा   कदाचिन्न्ायं   भूत्वाSभविता  वा  न  भूय:  । अजो  नित्य:  "
        " शाश्वतोऽयं  पुराणो   न  हन्यते  हन्यमाने  शरीरे   ॥";

    std::vector<std::string> expectedTokens = {
        "न",     "जायते", "म्रियते", "वा",     "कदाचिन्न्ायं", "भूत्वाऽभविता",
        "वा",    "न",    "भूयः",   "अजो",    "नित्यः",     "शाश्वतोऽयं",
        "पुराणो", "न",    "हन्यते",  "हन्यमाने", "शरीरे"};

    Tokenizer tok;
    std::vector<std::string> tokenizerOutput = tok.tokenize(shlokaInput);

    for (size_t i = 0; i < expectedTokens.size(); i++)
    {
        ExpectEqual(expectedTokens[i], tokenizerOutput[i],
                    "Tokenizer word should equal expected word: " +
                        expectedTokens[i]);
    }
}

void Tests::TestSplitter()
{
    std::cout << "[RUNNING] TestComplexVerbalSandhi..." << std::endl;

    SandhiSplitter splitter(db);
    splitter.initializePossibilities();

    // Target: "तच्चिन्तयति" (tat + cintayati = "He/She thinks about that")
    std::vector<std::string> inputTokens = {"तच्चिन्तयति"};

    auto output = splitter.splitTree(inputTokens);

    if (DEBUG_INFO)
    {
        // ------------------------------------------------------------------
        // VISUALIZE TREES IN TERMINAL
        // ------------------------------------------------------------------
        std::cout << "\n\n\n";
        std::cout
            << "=======================================================\n";
        std::cout
            << "               SANDHI SPLIT TREE VISUALIZER            \n";
        std::cout
            << "=======================================================\n\n";

        // Lambda helper for recursive ASCII tree printing
        auto printNode = [](auto &self,
                            const std::shared_ptr<WordAnalysisNode> &node,
                            const std::string &prefix, bool isLast) -> void
        {
            if (!node)
                return;

            std::cout << prefix << (isLast ? "└── " : "├── ")
                      << node->analysis.original << "\n";

            std::string childPrefix = prefix + (isLast ? "    " : "│   ");
            for (size_t i = 0; i < node->children.size(); ++i)
            {
                bool lastChild = (i == node->children.size() - 1);
                self(self, node->children[i], childPrefix, lastChild);
            }
        };

        // Print trees for every token in output
        for (size_t tokenIdx = 0; tokenIdx < output.size(); ++tokenIdx)
        {
            std::cout << "--- [ TOKEN " << (tokenIdx + 1) << " TREES ] ---\n";
            for (size_t treeIdx = 0; treeIdx < output[tokenIdx].size();
                 ++treeIdx)
            {
                std::cout << "Option " << (treeIdx + 1) << ":\n";
                printNode(printNode, output[tokenIdx][treeIdx], "", true);
                std::cout << "\n";
            }
        }

        std::cout
            << "=======================================================\n";
        std::cout << "\n\n\n" << std::endl;
        // ------------------------------------------------------------------
    }

    assert(output.size() == 1 &&
           "Test Failed: Expected output size 1 for batch input");
    const auto &treesForToken = output[0];
    assert(!treesForToken.empty() &&
           "Test Failed: No tree options generated for तच्चिन्तयति");

    // Search for the golden path: "तत्" -> "चिन्तयति"
    bool foundVerbalSandhiPath = false;

    for (const auto &root : treesForToken)
    {
        // Level 1: "तत्" (Pronoun stem before Schutva Sandhi)
        if (root && root->analysis.original == "तत्")
        {
            for (const auto &child : root->children)
            {
                // Level 2: "चिन्तयति" (Verbal form: Present tense 3rd person)
                if (child && child->analysis.original == "चिन्तयति")
                {
                    foundVerbalSandhiPath = true;
                    break;
                }
            }
        }
    }

    assert(foundVerbalSandhiPath &&
           "Test Failed: Path 'तत्' -> 'चिन्तयति' not found for तच्चिन्तयति!");

    std::cout << "[SUCCESS] TestComplexVerbalSandhi passed for Schutva Sandhi "
                 "+ Verbal form!"
              << std::endl;
}

void Tests::TestValidWord()
{
    // 1. Test Standalone Indeclinable (Avyaya) -> e.g., "अनु"
    {
        std::string word = "अनु";
        auto results = rootDeriver.isValidWord(word);

        // Check if at least one valid interpretation is returned
        bool hasMatch = !results.empty();
        Tests::ExpectEqual(true, hasMatch, "Indeclinable 'अनु' should be valid",
                           true);

        if (hasMatch)
        {
            Tests::ExpectEqual(
                std::string("indeclinable"), results[0].matchType,
                "Match type for 'अनु' should be 'indeclinable'", true);
            Tests::ExpectEqual(true, results[0].success,
                               "Analysis success state should be true", true);
        }
    }

    // 2. Test Pure Verb Form (Unprefixed Tiṅanta) -> e.g., "गच्छति"
    {
        std::string word = "गच्छति";
        auto results = rootDeriver.isValidWord(word);

        bool hasMatch = !results.empty();
        Tests::ExpectEqual(true, hasMatch, "Verb 'गच्छति' should be valid",
                           true);

        if (hasMatch)
        {
            Tests::ExpectEqual(std::string("verb"), results[0].matchType,
                               "Match type for 'गच्छति' should be 'verb'", true);
        }
    }

    // 3. Test Pure Nominal Form (Subanta) -> e.g., "रामेण"
    {
        std::string word = "रामेण";
        auto results = rootDeriver.isValidWord(word);

        bool hasMatch = !results.empty();
        Tests::ExpectEqual(true, hasMatch, "Nominal 'रामेण' should be valid",
                           true);

        if (hasMatch)
        {
            Tests::ExpectEqual(std::string("nominal"), results[0].matchType,
                               "Match type for 'रामेण' should be 'nominal'",
                               true);
        }
    }

    // 4. Test Prefixed Verb (Upasarga + Valid Stem) -> e.g., "अनुगच्छति"
    {
        std::string word = "अनुगच्छति";
        auto results = rootDeriver.isValidWord(word);

        bool foundAnuGacchatiSplit = false;

        for (const auto &analysis : results)
        {
            // Search specifically for the 'अनु' + 'गच्छति' split
            if (analysis.components.size() == 2 &&
                analysis.components[0] == "अनु" &&
                analysis.components[1] == "गच्छति")
            {
                foundAnuGacchatiSplit = true;
                Tests::ExpectEqual(
                    std::string("verb"), analysis.matchType,
                    "Match type for 'अनुगच्छति' split should be 'verb'", true);
                break;
            }
        }

        Tests::ExpectEqual(
            true, foundAnuGacchatiSplit,
            "isValidWord should contain the valid split 'अनु' + 'गच्छति'", true);
    }

    // 5. Test Invalid/Ghost Word (Invalid prefix + gibberish stem) -> e.g.,
    // "अनुxyz"
    {
        std::string word = "अनुxyz";
        auto results = rootDeriver.isValidWord(word);

        // Should produce zero valid interpretations
        size_t expectedSize = 0;
        Tests::ExpectEqual(
            expectedSize, results.size(),
            "Invalid word 'अनुxyz' should return 0 interpretations", true);
    }

    {
        std::string word = "अनुप्रविशति";
        auto results = rootDeriver.isValidWord(word);

        // Should produce zero valid interpretations
        size_t expectedSize = 0;
        Tests::ExpectEqual(
            expectedSize, results.size(),
            "Invalid word 'अनुxyz' should return 0 interpretations", true);
    }
}
