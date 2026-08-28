#include "list-pruner.hpp"
#include "wordTypes.hpp"
#include <memory>

std::vector<ValidDerives>
ListPruner::pruneInvalidPaths(std::vector<ValidDerives> candidates)
{
    std::vector<ValidDerives> validCandidates;

    for (auto &candidate : candidates)
    {
        bool candidateIsValid = true;

        // Linearly check every word token in this candidate path
        for (const auto &headNode : candidate)
        {
            if (!validateNode(headNode))
            {
                candidateIsValid = false;
                break; // Prune this candidate path
            }
        }

        if (candidateIsValid)
            validCandidates.push_back(std::move(candidate));
    }

    return validCandidates;
}

bool ListPruner::validateNode(const std::shared_ptr<Word> &node)
{
    if (!node)
        return false;

    // A node is valid if AT LEAST ONE top-level analysis tree is valid
    for (const auto &analysis : node->word.analyses)
        if (validateAnalysisTree(analysis))
            return true;

    return false;
}

bool ListPruner::validateAnalysisTree(const WordMetadata &analysis)
{
    bool noValue =
        !analysis.rootInfo.has_value() && !analysis.nominalInfo.has_value() &&
        !analysis.stemInfo.has_value() && !analysis.verbInfo.has_value();
    bool noType = analysis.matchType == WordMatchType::NONE;

    if (noValue && noType)
        return false; // Stem lookup failed or metadata is missing!

    // Recursive Step: If this component has child subcomponents, validate
    // all of them
    if (!analysis.components.empty())
    {
        for (const auto &childComponent : analysis.components)
        {
            if (!childComponent->rootInfo.has_value() &&
                !childComponent->nominalInfo.has_value() &&
                !childComponent->stemInfo.has_value() &&
                !childComponent->verbInfo.has_value())
            {
                return false;
            }

            if (childComponent->matchType == WordMatchType::NONE)
                return false; // Stem lookup failed or metadata is missing!

            if (!childComponent || !validateAnalysisTree(*childComponent))
            {
                return false; // If ANY child subcomponent fails, this analysis
                              // branch fails
            }
        }
    }

    return true;
}
