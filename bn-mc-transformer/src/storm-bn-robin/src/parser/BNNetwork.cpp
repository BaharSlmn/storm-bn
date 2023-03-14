//
// Created by Bahare Salmani on 2019-04-09.
//

#include "BNNetwork.h"
#include <regex>
#include "BIFFORMAT.h"

using namespace std;

BNNetwork::BNNetwork(const std::string &networkName, const std::vector<BNNode> &nodes,
                     const std::vector<ProbabilityTable> &tables,
                     const std::vector<Evidence> &evidences, const std::vector<Hypothesis> &hypothesises,
                     const std::unordered_set<std::string> &parameterNames)
{

  this->networkName = networkName;
  this->nodes = nodes;
  this->probabilityTables = tables;
  this->evidences = evidences;
  this->hypothesises = hypothesises;
  this->parameterNames = parameterNames;

  /* @refactor here for ommitting duplicated codes */

  for (BNNode bnNode : nodes)
  {
    for (Evidence ev : evidences)
    {
      if (ev.getObservations().count(bnNode.getNodeName()) != 0)
      {
        bnNode.markAsObserved();
      }
    }

    for (Hypothesis hyp : hypothesises)
    {
      if (hyp.getEvaluations().count(bnNode.getNodeName()) != 0)
      {
        bnNode.markAsQuestioned();
      }
    }
    nameToNodeMap[bnNode.getNodeName()] = bnNode;
  }

  int order = 1;
  for (const ProbabilityTable &probabilityTable : probabilityTables)
  {
    nodeNameToTableMap[probabilityTable.getNodeName()] = probabilityTable;
    nodeTopologicalOrder[probabilityTable.getNodeName()] = order;
    order++;
  }

  dag = Graph();
  for (const auto &probabilityTable : probabilityTables)
  {
    dag.addNode(probabilityTable.getNodeName(), probabilityTable.getPossibleValues().size());
  }
  sortTheTables();
}

BNNetwork::BNNetwork(const std::string &fileLoc, const std::string &fileName, const std::string &fileFormat,  bool isTailored,
                    const std::string &varFilePath)
{
  networkName = fileName;
  Utils util;
  fileContent = util.readFile(fileLoc + fileName + fileFormat);

  createEvidenceList();

  createHypothesisList();

  createNodeList();

  createParametersList();

  createTableList();

  dag = Graph();
  std::vector<ProbabilityTable> requiredProbabilityTables;
  std::vector<BNNode> requiredNodes;
  std::vector<std::string> nodesToProcess;
  std::vector<std::string> requiredNodesNames;
  std::vector<std::string> parentsNames;
  //check whether the transformation needs to be tailored to the evidence and hypothesis nodes or not
  if(isTailored == true){
    for (BNNode n : nodes){
      if (n.isObserved() || n.isQuestioned()){
        nodesToProcess.push_back(n.getNodeName());
      }
    }

    if (!nodesToProcess.empty()){
      std::vector<std::string> processedNodes;
      std::string currentNode;

      while (!nodesToProcess.empty()){
        currentNode = nodesToProcess[nodesToProcess.size() - 1];
        nodesToProcess.pop_back();
        processedNodes.push_back(currentNode);
        ProbabilityTable pTable = nodeNameToTableMap[currentNode];
        parentsNames = pTable.getParentsNames();
        for (std::string pName : parentsNames){
          if (std::find(processedNodes.begin(), processedNodes.end(), pName) == processedNodes.end()){
            nodesToProcess.push_back(pName);
          }
        }
      }

      for (BNNode n : nodes){
        if (std::find(processedNodes.begin(), processedNodes.end(), n.getNodeName()) != processedNodes.end()){
          requiredNodesNames.push_back(n.getNodeName());
        }
      }

      for (std::string nodeName : requiredNodesNames){
        requiredProbabilityTables.push_back(nodeNameToTableMap[nodeName]);
        requiredNodes.push_back(nameToNodeMap[nodeName]);
      }
      nodes = requiredNodes;
      probabilityTables = requiredProbabilityTables;
    }
    std::map<std::string, ProbabilityTable> nodeNameToTableMap_red;
    std::map<std::string, int> nodeTopologicalOrder_red;
    int order = 1;
    for(auto pTable: probabilityTables){
      nodeNameToTableMap_red[pTable.getNodeName()] = pTable;
      nodeTopologicalOrder_red[pTable.getNodeName()] = order;
      order++;
    }
    nodeNameToTableMap = nodeNameToTableMap_red;
    nodeTopologicalOrder = nodeTopologicalOrder_red;
  }

  for (const auto &probabilityTable : probabilityTables){
    dag.addNode(probabilityTable.getNodeName(), probabilityTable.getPossibleValues().size());
  }

  // change var file
  ifstream varFile(varFilePath);
  string line;
  string var;
  string varFileContent = "";
  while (getline(varFile, line)){
    std::string::size_type pos = line.find('-');
    if (pos != std::string::npos){
      var = line.substr(0, pos);
    }else{
      var = line;
    }
    if(isTailored){
      if (std::find(requiredNodesNames.begin(), requiredNodesNames.end(), var) != requiredNodesNames.end()){
        varFileContent += line + "\n";
      }
    }
    // check whether variable is still relevant
  }

  addEdgesToDAG();
  topologicalOrdering.clear();
  topologicalOrdering = TopologicalOrderingReader::getTopologicalOrdering(varFileContent, dag);
  sortTheTables();
}

int BNNetwork::calculateTheNumberOfStates()
{
  int max = 0;
  for (const ProbabilityTable &table : sortedProbabilityTables)
  {
    if (nameToNodeMap[table.getNodeName()].getNumberOfValues() > max)
      max = nameToNodeMap[table.getNodeName()].getNumberOfValues();
  }
  return max;
}

void BNNetwork::createNodeList()
{
  BIF bif;
  std::regex VAR_REG(bif.variableDeclaration);
  std::regex_iterator<std::string::iterator> rit(fileContent.begin(), fileContent.end(), VAR_REG);
  std::regex_iterator<std::string::iterator> rend;
  while (rit != rend)
  {
    BNNode bnNode(rit->str());
    for (Evidence ev : evidences)
    {
      if (ev.getObservations().count(bnNode.getNodeName()) != 0)
      {
        bnNode.markAsObserved();
      }
    }

    for (Hypothesis hyp : hypothesises)
    {
      if (hyp.getEvaluations().count(bnNode.getNodeName()) != 0)
      {
        bnNode.markAsQuestioned();
      }
    }

    nodes.push_back(bnNode);
    nameToNodeMap[bnNode.getNodeName()] = bnNode;
    ++rit;
  }
}

void BNNetwork::createTableList()
{
  BIF bif;
  std::regex VAR_REG(bif.probabilityDeclaration);
  std::regex_iterator<std::string::iterator> rit(fileContent.begin(), fileContent.end(), VAR_REG);
  std::regex_iterator<std::string::iterator> rend;
  int order = 1;
  while (rit != rend)
  {
    ProbabilityTable probabilityTable(rit->str(), nameToNodeMap);
    probabilityTables.push_back(probabilityTable);
    nodeNameToTableMap[probabilityTable.getNodeName()] = probabilityTable;
    nodeTopologicalOrder[probabilityTable.getNodeName()] = order;
    order++;
    ++rit;
  }
}

void BNNetwork::createEvidenceList()
{
  BIF bif;
  std::regex EV_REG(bif.evidenceDeclaration);
  std::regex_iterator<std::string::iterator> rit(fileContent.begin(), fileContent.end(), EV_REG);
  std::regex_iterator<std::string::iterator> rend;
  while (rit != rend)
  {
    Evidence evidence(rit->str());
    evidences.push_back(evidence);
    ++rit;
  }
}

void BNNetwork::createHypothesisList()
{
  BIF bif;
  std::regex EV_REG(bif.hypothesisDeclaration);
  std::regex_iterator<std::string::iterator> rit(fileContent.begin(), fileContent.end(), EV_REG);
  std::regex_iterator<std::string::iterator> rend;
  while (rit != rend)
  {
    Hypothesis hypothesis(rit->str());
    hypothesises.push_back(hypothesis);
    ++rit;
  }
}

void BNNetwork::createParametersList()
{
  BIF bif;
  std::regex PAR_REG(bif.parametersDeclaration);
  std::regex_iterator<std::string::iterator> rit(fileContent.begin(), fileContent.end(), PAR_REG);
  std::regex_iterator<std::string::iterator> rend;
  while (rit != rend)
  {
    Utils util;
    std::string str = rit->str();
    std::string match_string = util.returnMatchedString(str, bif.PARAMETER + bif.SPACEPLUS + bif.parameterName);
    util.eraseSubStr(match_string, bif.PARAMETER);
    util.eraseAllSubStr(match_string, " ");
    parameterNames.insert(match_string);
    std::cout << match_string << std::endl;
    ++rit;
  }
}

std::vector<BNNode> BNNetwork::getNodes() const
{
  return nodes;
}

std::vector<ProbabilityTable> BNNetwork::getSortedProbabilityTables() const
{
  return sortedProbabilityTables;
}

std::string BNNetwork::getNetworkName() const
{
  return networkName;
}

BNNode BNNetwork::getNodeByName(const std::string &nodeName) const
{
  return nameToNodeMap.at(nodeName);
}

ProbabilityTable BNNetwork::getTableByDependentNodeName(const std::string &nodeName) const
{
  return nodeNameToTableMap.at(nodeName);
}

int BNNetwork::getTopologicalOrder(const std::string &nodeName) const
{
  return nodeTopologicalOrder.at(nodeName);
}

void BNNetwork::addEdgesToDAG()
{
  int size = probabilityTables.size();
  for (int i = 0; i < size; i++)
  {
    int size2 = probabilityTables.at(i).getParentsNames().size();
    for (int j = 0; j < size2; j++)
    {
      dag.addEdge(nodeTopologicalOrder[probabilityTables.at(i).getNodeName()] - 1,
                  nodeTopologicalOrder[probabilityTables.at(i).getParentsNames().at(j)] - 1);
    }
  }
}

void BNNetwork::sortTheTables()
{
  BayesianNetworkTransformer transformer;
  if (topologicalOrdering.empty())
  {
    topologicalOrdering = TopologicalOrderingFinder::getTopologicalOrdering(dag);
  }
  std::cout << "\nExpected Size of DTMC: "
            << TopologicalOrderingFinder::calculateSizeTopologicalOrdering(dag, topologicalOrdering) << "\n";
  janiData = transformer.runAlgorithm(dag, topologicalOrdering, createEvidenceIndexToValue(), createHypothesisIndicator());

  /*
   * TODO
   * I am sure this here can be changed so that getTopologicalOrderingStack returns
   * already sorted stack.
   */
  stack<int> Stack = TopologicalOrderingFinder::getTopologicalOrderingStack(topologicalOrdering);
  stack<int> sortStack;
  while (!Stack.empty())
  {
    sortStack.push(Stack.top());
    Stack.pop();
  }

  int order = 1;
  while (!sortStack.empty())
  {
    ProbabilityTable table = probabilityTables[sortStack.top()];
    sortedProbabilityTables.push_back(table);
    nodeTopologicalOrder[table.getNodeName()] = order;
    order++;
    sortStack.pop();
  }
}

std::vector<Evidence> BNNetwork::getEvidences() const
{
  return evidences;
}

std::vector<Hypothesis> BNNetwork::getHypothesises() const
{
  return hypothesises;
}

std::unordered_set<std::string> BNNetwork::getParameterNames() const
{
  return parameterNames;
}

std::unordered_map<Graph::NodeIndex, int> BNNetwork::createEvidenceIndexToValue() const
{
  std::unordered_map<Graph::NodeIndex, int> indexToValue;
  for (const auto &item : evidences)
  {
    for (const auto &observation : item.getObservations())
    {
      indexToValue[dag.getNodeIndex(observation.first)] = std::stoi(observation.second, nullptr, 10);
    }
  }
  return indexToValue;
}

std::vector<bool> BNNetwork::createHypothesisIndicator() const
{
  std::vector<bool> indicator;
  for (int i = 0; i < dag.getNumberOfNodes(); ++i)
  {
    indicator.push_back(false);
  }
  for (const auto &item : hypothesises)
  {
    for (const auto &evaluations : item.getEvaluations())
    {
      indicator[dag.getNodeIndex(evaluations.first)] = true;
    }
  }
  return indicator;
}