#pragma once

#include "wordTypes.hpp"
#include <vector>

// prunes the list of invalid components
class ListPruner
{
public:
    static std::vector<ValidDerives>
    pruneInvalidPaths(std::vector<ValidDerives> candidates);

private:
    static bool validateNode(const std::shared_ptr<Word> &node);
    static bool validateAnalysisTree(const WordMetadata &analysis);
};

