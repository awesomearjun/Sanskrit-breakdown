#include "tests.hpp"

#include <cstddef>
#include <iostream>
#include <vector>

#include "database.hpp"
#include "tokenizer.hpp"

void Tests::TestAll()
{
    TestDatabaseFunctions();
    TestTokenizer();
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
    Database db;
    db.initialize("./roots.json", "./grammar_constants.json");

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
