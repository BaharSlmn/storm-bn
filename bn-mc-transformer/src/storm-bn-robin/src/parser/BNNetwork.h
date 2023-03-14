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
#include "storm-bn-robin/src/transformer/Graph.h"
#include "storm-bn-robin/src/transformer/TopologicalOrderingFinder.h"
#include "storm-bn-robin/src/transformer/BayesianNetworkTransformer.h"
#include "storm-bn-robin/src/transformer/TopologicalOrderingReader.h"

class BNNetwork {
public:
  BNNetwork() = default;

  BNNetwork(const std::string &networkName, const std::vector<BNNode> &nodes,
            const std::vector<ProbabilityTable> &tables,
            const std::vector<Evidence> &evidences, const std::vector<Hypothesis> &hypothesises,
            const std::unordered_set<std::string> &parameterNames);

  BNNetwork(const std::string& fileLoc, const std::string& fileName, const std::string& fileFormat, bool isTailored, const std::string& varFilePath = "");

  std::vector<BNNode> getNodes() const;

  BNNode getNodeByName(const std::string& nodeName) const;

  ProbabilityTable getTableByDependentNodeName(const std::string& nodeName) const;

  std::string getNetworkName() const;

  std::vector<ProbabilityTable> getSortedProbabilityTables() const;

  int getTopologicalOrder(const std::string& nodeName) const;

  std::vector<Evidence> getEvidences() const;

  std::vector<Hypothesis> getHypothesises() const;

  std::unordered_set<std::string> getParameterNames() const;

  BayesianNetworkTransformer::JaniCreationData janiData;


private:
  std::string fileContent;
  std::string networkName;
  std::vector<BNNode> nodes;
  std::vector<ProbabilityTable> probabilityTables;
  std::vector<ProbabilityTable> sortedProbabilityTables;
  std::map<std::string, BNNode> nameToNodeMap;
  std::map<std::string, ProbabilityTable> nodeNameToTableMap;
  std::map<std::string, int> nodeTopologicalOrder;

  Graph dag;

  std::vector<Evidence> evidences;
  std::vector<Hypothesis> hypothesises;
  std::unordered_set<std::string> parameterNames;
  std::vector<Graph::NodeIndex> topologicalOrdering;

  void createNodeList();

  void createTableList();

  void createEvidenceList();

  void createHypothesisList();

  void createParametersList();

  void sortTheTables();

  void addEdgesToDAG();

  int calculateTheNumberOfStates();

  std::unordered_map<Graph::NodeIndex, int> createEvidenceIndexToValue() const;

  std::vector<bool> createHypothesisIndicator() const;


};

#endif //BNPARSER_PARSER_H
