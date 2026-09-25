#include "list-pruner.hpp"
#include "wordTypes.hpp"
#include <memory>

WordList ListPruner::pruneInvalidPaths(const WordList &candidates)
{
    WordList validCandidates;

    for (auto &candidate : candidates)
    {
        bool candidateIsValid = true;

        if (!validateNode(candidate))
        {
            candidateIsValid = false;
            continue; // Prune this candidate path
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

    // A node is valid if AT LEAST ONE top-level metadata tree is valid
    for (const auto &metadatas : node->metadata)
    {
        for (const auto &metadata : metadatas)
            if (validateAnalysisTree(metadata))
                return true;
    }

    return false;
}

bool ListPruner::validateAnalysisTree(const WordMetadata &analysis)
{
    if (!analysis.metadata.has_value() &&
        analysis.matchType == WordMatchType::NONE)
    {
        return false; // Stem lookup failed or metadata is missing!
    }

    // Recursive Step: If this component has child cores, validate
    // all of them
    if (analysis.cores.empty())
        return true;
    for (const auto &childComponent : analysis.cores)
    {
        if (!childComponent.success)
            return false;

        if (!childComponent.metadata.has_value())
            return false;

        if (const Root *r = std::get_if<Root>(&childComponent.metadata.value());
            r != nullptr && r->isEmpty())
        {
            return false;
        }

        if (const NominalStem *n =
                std::get_if<NominalStem>(&childComponent.metadata.value());
            n != nullptr && n->isEmpty())
        {
            return false;
        }

        if (childComponent.matchType == CoreMatchType::NONE)
            return false; // Stem lookup failed or metadata is missing!

        const Root *root = std::get_if<Root>(&childComponent.metadata.value());

        if (!root)
            return false;
        if (root->isEmpty())
            return false;
    }

    return true;
}
