//
// Created by Bahare Salmani on 2019-04-09.
//

#ifndef BNPARSER_PARSER_H
#define BNPARSER_PARSER_H

// reading a text file
#include <iostream>
#include <fstream>
#include <string>
#include <vector>
#include <unordered_set>
#include "map"
#include <stack>
#include "BNNode.h"
#include "ProbabilityTable.h"
#include "Evidence.h"
#include "Hypothesis.h"
#include "../transformer/DAG.h"

class BNNetwork {
public:
    BNNetwork(){};
    BNNetwork(const std::string& networkName, const std::vector<BNNode>& nodes, const std::vector<ProbabilityTable>& tables,
            const std::vector<Evidence>& evidences, const std::vector<Hypothesis>& hypothesises, const std::unordered_set<std::string>& parameterNames);
    BNNetwork(std::string fileLoc, std::string fileName, std::string fileFormat);
    std::vector<BNNode> getNodes() const;
    BNNode getNodeByName(std::string nodeName) const;
    ProbabilityTable getTableByDependentNodeName(std::string nodeName) const;
    std::string getNetworkName() const;
    std::vector<ProbabilityTable> getSortedProbabilityTables() const;
    int getTopologicalOrder(std::string nodeName) const;
    std::vector<Evidence> getEvidences() const;
    std::vector<Hypothesis> getHypothesises() const;
    std::unordered_set<std::string> getParameterNames() const;


private:
    std::string fileContent;
    std::string networkName;
    std::vector<BNNode> nodes;
    std::vector<ProbabilityTable> probabilityTables;
    std::vector<ProbabilityTable> sortedProbabilityTables;
    std::map<std::string, BNNode> nameToNodeMap;
    std::map<std::string, ProbabilityTable> nodeNameToTableMap;
    std::map<std::string, int> nodeTopologicalOrder;
    DAG dag;

    std::vector<Evidence> evidences;
    std::vector<Hypothesis> hypothesises;
    std::unordered_set<std::string> parameterNames;
    void createNodeList();
    void createTableList();
    void createEvidenceList();
    void createHypothesisList();
    void createParametersList();
    void sortTheTables();
    void addEdgesToDAG();
    int calculateTheNumberOfStates();


    };

#endif //BNPARSER_PARSER_H
