#include "tests.hpp"

#include <cstddef>
#include <iostream>
#include <memory>
#include <vector>

#include "database.hpp"
#include "sandhi-splitter.hpp"
#include "tokenizer.hpp"
#include "wordTypes.hpp"

void Tests::TestAll()
{
    TestDatabaseFunctions();
    TestTokenizer();
    TestSplitter();
    TestRootDeriver();
}

Tests::Tests()
{
    db.initialize("./roots.json", "./grammar_constants.json");
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
    ExpectEqual(true, db.rootExists("वद्").has_value(), "Root 'वद्' should exist");
    ExpectEqual(false, db.rootExists("fd").has_value(),
                "Root 'fd' should not exist");
    ExpectEqual(true, db.rootExists("सद्").has_value(), "Root 'सद्' should exist");
    ExpectEqual(false, db.rootExists("अस्ति").has_value(),
                "Root 'अस्ति' should not exist");
    ExpectEqual(true, db.rootExists("यज्").has_value(), "Root 'यज्' should exist");
    ExpectEqual(false, db.rootExists("वृतु").has_value(),
                "Root 'वृतु' should not exist");

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
    auto verbMatchesOpt = db.tryMatchVerbalSuffix("ति");
    ExpectEqual(true, verbMatchesOpt.has_value(),
                "Verbal suffix 'ति' should match");

    if (verbMatchesOpt && !verbMatchesOpt->empty())
    {
        const auto& verbMeta = verbMatchesOpt->front();
        ExpectEqual(std::string("ति"), verbMeta.suffix,
                    "Verbal suffix 'ति' should have correct metadata");
        ExpectEqual(static_cast<int>(VerbTenseOrMood::PRESENT),
                    static_cast<int>(verbMeta.tenseOrMood),
                    "Verbal suffix 'ति' should have correct tense/mood", false);
    }

    auto invalidVerbMatchesOpt = db.tryMatchVerbalSuffix("xyz");
    ExpectEqual(false, invalidVerbMatchesOpt.has_value(),
                "Verbal suffix 'xyz' should not match");

    // Test tryMatchNominalSuffix
    auto nominalMatchesOpt = db.tryMatchNominalSuffix("स्य");
    ExpectEqual(true, nominalMatchesOpt.has_value(),
                "Nominal suffix 'स्य' should match");

    if (nominalMatchesOpt && !nominalMatchesOpt->empty())
    {
        const auto& nominalMeta = nominalMatchesOpt->front();
        ExpectEqual(std::string("स्य"), nominalMeta.suffix,
                    "Nominal suffix 'स्य' should have correct metadata");
    }

    auto invalidNominalMatchesOpt = db.tryMatchNominalSuffix("xyz");
    ExpectEqual(false, invalidNominalMatchesOpt.has_value(),
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

    // Target: "तच्चिन्तयति" (tat + cintayati)
    std::vector<std::string> inputTokens = {"तच्चिन्तयति"};

    auto output = splitter.splitTree(inputTokens);

    ExpectEqual(false, output.empty(),
                "Expected at least one valid split tree.", true);

    for (const auto &head: output)
    {
        ExpectEqual(false, head.empty(),
                    "Expected at least one valid split tree head.", true);
        ExpectEqual(std::string("तत्"), head[0]->word.cleanForm,
                    "Expected head node to match input token \"तत्\".", true);

        // should be guarrenteed one analysis (so we don't need to test)
        for (const auto &analysis : head[0]->word.analyses)
        {
            ExpectEqual(std::string("तत्"), analysis.original,
                        "Expected head node analysis to match input token \"तत्\".",
                        true);
            ExpectEqual(WordMatchType::INDECLINABLE, analysis.matchType,
                        "Expected head node analysis matchType to be \"indeclinable\".",
                        false);
        }

        ExpectEqual(false, head[0]->children.empty(),
                    "Expected head node to have at least one child.", true);

        for (const auto &child : head[0]->children)
        {
            ExpectEqual(std::string("चिन्तयति"), child->word.cleanForm,
                        "Expected child node to match input token \"चिन्तयति\".",
                        true);

            for (const auto &analysis : child->word.analyses)
            {
                ExpectEqual(std::string("चिन्तयति"), analysis.original,
                            "Expected child node analysis to match input token \"चिन्तयति\".",
                            true);
                ExpectEqual(WordMatchType::VERB, analysis.matchType,
                            "Expected child node analysis matchType to be \"verb\".",
                            false);

                // morphological analysis should yield a stem and suffix
                for (const auto &component : analysis.components)
                {
                    ExpectEqual(true, component.success,
                                "Expected child node analysis component to be successful.",
                                true);
                    ExpectEqual(false, component.original.empty(),
                                "Expected child node analysis component to have non-empty original text.",
                                true);
                }
            }
        }
    }
}

void Tests::TestRootDeriver()
{
    std::cout << "[RUN] Tests::TestRootDeriver..." << std::endl;

    RootDeriver deriver(db);
    SandhiSplitter splitter(db);

    // 1. Pass fully inflected tokens (or sandhi-combined shloka words)
    // "गच्छति" (present tense 3rd sg) + "पुस्तकम्" (neuter nom/acc sg)
    std::vector<std::string> tokens = {"गच्छति", "पुस्तकम्"};

    // 2. Parse into candidate paths (SplitCandidates / ParseForest)
    std::vector<SandhiCandidate> candidates = splitter.splitTree(tokens);

    ExpectEqual(false, candidates.empty(),
                "Expected at least one candidate parse path from SandhiSplitter.", true);

    // 3. Execute root derivation across candidate paths
    std::vector<ValidDerives> derivedChains =
        deriver.deriveRoots(candidates);

    ExpectEqual(false, derivedChains.empty(),
                "Expected at least one valid root derivation chain from RootDeriver.", true);
}