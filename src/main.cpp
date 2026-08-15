#include <iostream>
#include <fstream>
#include <nlohmann/json.hpp>
#include "toJson.hpp" // Your header containing to_json functions
#include "wordTypes.hpp" // Your header containing WordAnalysis and related types
#include "root-deriver.hpp" // Your header containing RootDeriver and related types
#include "sandhi-splitter.hpp" // Your header containing SandhiSplitter and related types
#include "database.hpp" // Your header containing Database and related types
#include "tokenizer.hpp" // Your header containing

using json = nlohmann::ordered_json;

int main(int argc, char* argv[]) {
    // 1. Setup Engine & Parse
    Database db;
    db.initialize("./roots.json", "./grammar_constants.json");


    SandhiSplitter splitter(db);
    RootDeriver deriver(db);

    std::string shlokaInput =
        "विद्या ददाति विनयं विनयाद्याति पात्रताम्";

    Tokenizer tok;
    std::vector<std::string> tokenizerOutput = tok.tokenize(shlokaInput);

    std::vector<SandhiCandidate> candidates = splitter.splitTree(tokenizerOutput);
    
    // 2. Derive roots across paths
    std::vector<ValidDerives> derivedChains = deriver.deriveRoots(candidates);

    // 3. Serialize output to JSON
    json response;
    response["status"] = "success";
    response["inputTokens"] = tokenizerOutput;
    response["chains"] = json::array();

    for (const auto& chainHead : derivedChains) {
        response["chains"].push_back(wordListToJsonArray(chainHead));
    }

    // 4. Dump formatted JSON to stdout
    std::cout << response.dump(4) << std::endl;

    // Optional: Write to file for visualizers/web frontend
    std::ofstream file("analysis_output.json");
    file << response.dump(4);
    
    return 0;
}