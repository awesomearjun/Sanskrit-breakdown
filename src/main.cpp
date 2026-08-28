#include "database.hpp" // Your header containing Database and related types
#include "list-pruner.hpp"
#include "root-deriver.hpp" // Your header containing RootDeriver and related types
#include "sandhi-splitter.hpp" // Your header containing SandhiSplitter and related types
#include "stem-deriver.hpp"
#include "toJson.hpp"    // Your header containing to_json functions
#include "tokenizer.hpp" // Your header containing
#include "wordTypes.hpp" // Your header containing WordAnalysis and related types
#include <fstream>
#include <iostream>
#include <memory>
#include <nlohmann/json.hpp>
#include <vector>

using json = nlohmann::ordered_json;

int main(int argc, char *argv[])
{
    // 1. Setup Engine & Parse
    Database db;
    db.initialize("./roots.json", "./grammar_constants.json", "./stems.json");
    SandhiSplitter splitter(db);
    splitter.initializePossibilities();
    StemDeriver sDeriver(db);
    RootDeriver rDeriver(db);
    Tokenizer tok;

    std::string shlokaInput = "विद्या ददाति विनयं विनयाद्याति पात्रताम्";

    std::vector<std::shared_ptr<Word>> tokenizerOutput =
        tok.tokenize(shlokaInput);
    SandhiCandidates sandhiCandidates = splitter.splitTree(tokenizerOutput);
    std::vector<SandhiCandidate> candidates =
        sDeriver.treeAssignStem(sandhiCandidates);
    std::vector<ValidDerives> derivedChains = rDeriver.deriveRoots(candidates);

    std::vector<ValidDerives> finalOutput =
        ListPruner::pruneInvalidPaths(derivedChains);

    // 3. Serialize output to JSON
    json response;
    response["status"] = "success";
    response["inputTokens"] = tokenizerOutput;
    response["chains"] = json::array();

    for (const auto &chainHead : finalOutput)
        response["chains"].push_back(wordListToJsonArray(chainHead));

    // 4. Dump formatted JSON to stdout
    std::cout << response.dump(4) << std::endl;

    // Optional: Write to file for visualizers/web frontend
    std::ofstream file("analysis_output.json");
    file << response.dump(4);

    return 0;
}
