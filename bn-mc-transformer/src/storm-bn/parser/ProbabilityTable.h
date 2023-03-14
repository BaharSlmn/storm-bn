//
// Created by Bahare Salmani on 2019-04-25.
//

#ifndef BNPARSER_PROBABILITYTABLE_H
#define BNPARSER_PROBABILITYTABLE_H

#include <vector>
#include <string>
#include "BIFFORMAT.h"
#include "Utils.h"
#include "ProbabilityRow.h"
#include "BNNode.h"



class ProbabilityTable {
public:
    ProbabilityTable(){}
    ProbabilityTable(std::string BIFdeclaration, std::map<std::string, BNNode> nodes);
    ProbabilityTable(const BNNode& node, const std::vector<BNNode>& parents, const std::vector<ProbabilityRow>& rows);
    std::string getNodeName() const;
    std::vector<std::string> getParentsNames() const;
    std::vector<ProbabilityRow> getRows() const;
    std::vector<std::string> getPossibleValues() const;
    BNNode getNode() const;
    std::vector<BNNode> getParents() const;
    bool isTheNodeObserved() const;
    bool isTheNodeQuestioned() const;

private:
    std::string theNodeName;
    BNNode node;
    std::vector<std::string> parentsNames;
    std::vector<BNNode> parents;
    std::map<std::string, BNNode> allNamesToNodes;
    void build();
    std::vector<ProbabilityRow> probabilityEntries;
    std::string tableDeclaration;
    bool isIndependent;
    void parseNodes();
    void parseRows();

};


#endif //BNPARSER_PROBABILITYTABLE_H
