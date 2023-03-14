//
// Created by Bahare Salmani on 2019-04-09.
//

#include "BNNetwork.h"
#include <regex>
#include "BIFFORMAT.h"

#include <time.h>

using namespace std;


BNNetwork::BNNetwork(const std::string& networkName, const std::vector<BNNode>& nodes, const std::vector<ProbabilityTable>& tables,
                    const std::vector<Evidence>& evidences, const std::vector<Hypothesis>& hypothesises,
                    const std::unordered_set<std::string>& parameterNames) {

    this->networkName = networkName;
    this->nodes = nodes;
    this->probabilityTables = tables;
    this->evidences = evidences;
    this->hypothesises = hypothesises;
    this->parameterNames = parameterNames;

    /* @refactor here for ommitting duplicated codes */

    for (BNNode bnNode : nodes) {
        for (Evidence ev : evidences) {
            if (ev.getObservations().count(bnNode.getNodeName()) != 0) {
                bnNode.markAsObserved();
            }
        }

        for (Hypothesis hyp : hypothesises) {
            if (hyp.getEvaluations().count(bnNode.getNodeName()) != 0) {
                bnNode.markAsQuestioned();
            }
        }
        nameToNodeMap[bnNode.getNodeName()] = bnNode;
    }

    int order = 1;
    for (ProbabilityTable probabilityTable : probabilityTables) {
        nodeNameToTableMap[probabilityTable.getNodeName()] = probabilityTable;
        nodeTopologicalOrder[probabilityTable.getNodeName()] = order;
        order++;
    }

    dag = DAG(probabilityTables.size());
    sortTheTables();

}

BNNetwork::BNNetwork(std::string fileLoc, std::string fileName, std::string fileFormat) {
    networkName = fileName;
    Utils util;

    fileContent = util.readFile(fileLoc + fileName + fileFormat);

    createEvidenceList();

    createHypothesisList();

    createNodeList();

    createParametersList();

    createTableList();

    dag = DAG(probabilityTables.size());

    sortTheTables();

}

int BNNetwork::calculateTheNumberOfStates(){
    int temp = 1;
    int numberOfStates = 1;
    int step = 1;
    int max = 0;
    for(ProbabilityTable table : sortedProbabilityTables){
        if(nameToNodeMap[table.getNodeName()].getNumberOfValues() > max)
            max = nameToNodeMap[table.getNodeName()].getNumberOfValues();
    }
    return max;
}


void BNNetwork::createNodeList(){
    BIF bif;
    std::regex VAR_REG(bif.variableDeclaration);
    std::regex_iterator<std::string::iterator> rit ( fileContent.begin(), fileContent.end(), VAR_REG );
    std::regex_iterator<std::string::iterator> rend;
    while (rit!=rend) {
        BNNode bnNode(rit->str());
        for(Evidence ev : evidences){
            if(ev.getObservations().count(bnNode.getNodeName()) != 0 ){
                bnNode.markAsObserved();
            }
        }

        for(Hypothesis hyp : hypothesises){
            if(hyp.getEvaluations().count(bnNode.getNodeName()) != 0 ){
                    bnNode.markAsQuestioned();
                }
        }

        nodes.push_back(bnNode);
        nameToNodeMap[bnNode.getNodeName()] = bnNode;
        ++rit;
    }
}

void BNNetwork::createTableList(){
   BIF bif;
   std::regex VAR_REG(bif.probabilityDeclaration);
   std::regex_iterator<std::string::iterator> rit ( fileContent.begin(), fileContent.end(), VAR_REG );
   std::regex_iterator<std::string::iterator> rend;
   int order = 1;
   while (rit!=rend) {
        ProbabilityTable probabilityTable(rit->str(), nameToNodeMap);
        probabilityTables.push_back(probabilityTable);
        nodeNameToTableMap[probabilityTable.getNodeName()] = probabilityTable;
        nodeTopologicalOrder[probabilityTable.getNodeName()] = order;
        order ++;
        ++rit;
    }
}

void BNNetwork::createEvidenceList() {
    BIF bif;
    std::regex EV_REG(bif.evidenceDeclaration);
    std::regex_iterator<std::string::iterator> rit ( fileContent.begin(), fileContent.end(), EV_REG );
    std::regex_iterator<std::string::iterator> rend;
    while (rit!=rend) {
        Evidence evidence(rit->str());
        evidences.push_back(evidence);
        ++rit;
    }
}

void BNNetwork::createHypothesisList(){
    BIF bif;
    std::regex EV_REG(bif.hypothesisDeclaration);
    std::regex_iterator<std::string::iterator> rit ( fileContent.begin(), fileContent.end(), EV_REG );
    std::regex_iterator<std::string::iterator> rend;
    while (rit!=rend) {
        Hypothesis hypothesis(rit->str());
        hypothesises.push_back(hypothesis);
        ++rit;
    }
}

void BNNetwork::createParametersList() {
    BIF bif;
    std::regex PAR_REG(bif.parametersDeclaration);
    std::regex_iterator<std::string::iterator> rit ( fileContent.begin(), fileContent.end(), PAR_REG );
    std::regex_iterator<std::string::iterator> rend;
    while (rit!=rend) {
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


std::vector<BNNode> BNNetwork::getNodes() const{
    return nodes;
}

std::vector<ProbabilityTable> BNNetwork::getSortedProbabilityTables() const {
    return sortedProbabilityTables;
}

std::string BNNetwork::getNetworkName() const{
    return networkName;
}


 BNNode BNNetwork::getNodeByName(std::string nodeName) const {
    return nameToNodeMap.at(nodeName);
}

ProbabilityTable BNNetwork::getTableByDependentNodeName(std::string nodeName) const{
    return nodeNameToTableMap.at(nodeName);
}

int BNNetwork::getTopologicalOrder(std::string nodeName) const{
    return nodeTopologicalOrder.at(nodeName);
}



void BNNetwork::addEdgesToDAG(){
    int size = probabilityTables.size();
    for(int i = 0; i < size; i++){
        int size2 = probabilityTables.at(i).getParentsNames().size();
        for(int j = 0; j < size2; j++){
            /* orders are 1-based, but topological sort works 0-based, so -1 needed */
            dag.addEdge(nodeTopologicalOrder[probabilityTables.at(i).getNodeName()] - 1 ,nodeTopologicalOrder[probabilityTables.at(i).getParentsNames().at(j)] - 1);
        }
    }
}

void BNNetwork::sortTheTables() {
    addEdgesToDAG();
    stack<int> Stack = dag.topologicalSort();
    stack<int> sortStack;
    while (Stack.empty() == false)
    {
        /* std::cout << Stack.top() <<  probabilityTables[Stack.top()].getNodeName() << " "; */
        sortStack.push(Stack.top());
        Stack.pop();
    }

    int order = 1;
    while (sortStack.empty() == false)
    {
        ProbabilityTable table = probabilityTables[sortStack.top()];
        sortedProbabilityTables.push_back(table);
        nodeTopologicalOrder[table.getNodeName()] = order;
        order ++;
        sortStack.pop();
    }
}

std::vector<Evidence> BNNetwork::getEvidences() const{
    return evidences;
}

std::vector<Hypothesis> BNNetwork::getHypothesises() const{
    return hypothesises;
}

std::unordered_set<std::string> BNNetwork::getParameterNames() const{
    return parameterNames;
}