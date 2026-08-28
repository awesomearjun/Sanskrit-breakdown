#pragma once

#include "wordTypes.hpp"
#include <vector>

// prunes the list of invalid components
class ListPruner
{
public:
    static WordList pruneInvalidPaths(const WordList &candidates);

private:
    static bool validateNode(const std::shared_ptr<Word> &node);
    static bool validateAnalysisTree(const WordMetadata &analysis);
};
