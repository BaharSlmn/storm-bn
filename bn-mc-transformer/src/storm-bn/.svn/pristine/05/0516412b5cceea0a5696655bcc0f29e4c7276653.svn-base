//
// Created by Bahare Salmani on 2020-03-25.
//

#include "NaiveBinarizator.h"
#include <stdlib.h>
#include <stdio.h>

#include <cmath>


NaiveBinarizator::NaiveBinarizator(BNNetwork bn) {
    bnNetwork = bn;
    binarize();
}

void NaiveBinarizator::binarize() {
    std::string retValue;
    for(BNNode node : bnNetwork.getNodes()){
        binarizeNode(node);
      //  keepValidityNode(node);
    }

    for(ProbabilityTable table : bnNetwork.getSortedProbabilityTables()){
        binarizeTable(table);
     //   keepValidityTable(table.getNode());
    }

    for(Evidence ev : bnNetwork.getEvidences()){
        //binarizeEvidence();
    }

    for(Hypothesis hyp : bnNetwork.getHypothesises()){
        //binarizeHypothesis();
    }
}

void NaiveBinarizator::binarizeNode(BNNode node) {
    std::vector<std::string> valuesCopy = node.getVariableValueList();
    std::vector<BNNode> newBinarizedNodes;
    for(std::string possibleValue : valuesCopy){
        BNNode valueNode = BNNode(node.getNodeName() + possibleValue, 2,
                                {"false", "true"}, node.isObserved(), node.isQuestioned());
        binarizedNodes.push_back(valueNode);
        newBinarizedNodes.push_back(valueNode);
    }

    nodeNamesToValueNodes[node.getNodeName()] = newBinarizedNodes;
}


void NaiveBinarizator::keepValidityNode(BNNode node) {
    /* node for keeping the consistency */
    BNNode validityNode = BNNode(node.getNodeName() + "_validity", 2,
                                    {"false", "true"}, node.isObserved(), node.isQuestioned());
    nodeNamesToConsistencyNode[node.getNodeName()] = validityNode;

    binarizedNodes.push_back(validityNode);
}


void NaiveBinarizator::keepValidityTable(BNNode node){

    std::vector<ProbabilityRow> validityRows;
    std::vector<std::string> possibleValues = node.getVariableValueList();
    int n = possibleValues.size();
    int rowsNum = pow(2, n);

    int parentsValues[rowsNum][n];
    std::vector<std::string> stringParentsValues[rowsNum];
    int howManyOnes[rowsNum];
    for (int i = 0; i < rowsNum ; i++) {
        decToBinary(i, n, parentsValues[i]);
        howManyOnes[i] = 0;
        for (int j = 0; j < n; j++) {
            if (parentsValues[i][j] == 1) {
                howManyOnes[i]++;
                stringParentsValues[i].push_back("true");
            }
            if (parentsValues[i][j] == 0) {
                stringParentsValues[i].push_back("false");
            }
        }
    }
    std::vector<BNNode> validityParentNodes = nodeNamesToValueNodes[node.getNodeName()];
    std::vector<std::string> validityParentsNames;
    for (BNNode valueNode : validityParentNodes) {
        validityParentsNames.push_back(valueNode.getNodeName());
    }

    for (int i = 0; i < rowsNum; i++) {
        std::vector<std::string> newNodeProbabilities;
        if (howManyOnes[i] == 1)
            newNodeProbabilities = {"0.0", "1.0"};
        else
            newNodeProbabilities = {"1.0", "0.0"};
        ProbabilityRow newRow = ProbabilityRow(stringParentsValues[i], newNodeProbabilities, validityParentsNames,
                                               {"false", "true"});
        validityRows.push_back(newRow);
    }

    ProbabilityTable validityTable(nodeNamesToConsistencyNode[node.getNodeName()], validityParentNodes, validityRows);

    binarizedTables.push_back(validityTable);
}



void NaiveBinarizator::binarizeTable(ProbabilityTable table) {
    std::vector<BNNode> thisBinaryNodes = nodeNamesToValueNodes[table.getNodeName()];


    std::vector<BNNode> binarizedParents;
    std::vector<BNNode> parents = table.getParents();
    for(BNNode parent : parents){
        std::vector<BNNode> thisBinarizedParents = nodeNamesToValueNodes[parent.getNodeName()];
        binarizedParents.insert(binarizedParents.begin(),
                                thisBinarizedParents.begin(),thisBinarizedParents.end());
      //  binarizedParents.push_back(nodeNamesToConsistencyNode[parent.getNodeName()]);
    }



    std::vector<ProbabilityRow> rows = table.getRows();
    std::vector<ProbabilityRow> newRows;

    for(ProbabilityRow row : rows){
        binarizeRow(row, table.getNodeName());
    }

    for(BNNode n : thisBinaryNodes){
        ProbabilityTable newTable(n, binarizedParents, nodeNamesToBinarizedRows[n.getNodeName()]);
        binarizedTables.push_back(newTable);
    }
}

void NaiveBinarizator::binarizeRow(ProbabilityRow row, std::string nodeName) {

    /* binarize the values that the parents are evaluated to, in the row */
    std::vector<std::string> parentsValues = row.getParentValues();
    std::vector<std::string> parentsNames = row.getParentsNames();
    std::vector<std::string> newParentsNames;
    std::vector<std::string> newParentsValues;
    std::unordered_map<std::string, std::string> newParentValuesMapping;

    /* create new binarized parents names and values for the row */
    int index = 0;
    for(std::string parentName : parentsNames){
        BNNode parentNode = bnNetwork.getNodeByName(parentsNames.at(index)); /* the error is here */
        for(std::string parentValue : parentNode.getVariableValueList()){
            std::string newParentName = parentName + parentsValues.at(index);
            newParentsNames.push_back(newParentName);
            if(parentsValues.at(index) == parentValue){
                newParentValuesMapping[newParentName] = "true";
                newParentsValues.push_back("true");
            }
            else{
                newParentValuesMapping[newParentName] = "false";
                newParentsValues.push_back("false");
            }
        }
        //newParentsNames.push_back(nodeNamesToConsistencyNode[parentName].getNodeName());
       // newParentsValues.push_back("true");
     //   newParentValuesMapping[nodeNamesToConsistencyNode[parentName].getNodeName()] = "true";
        index ++;
    }

    /* Binarize the probability entries in the row */
    BNNode currentNode = bnNetwork.getNodeByName(nodeName);
    std::vector<std::string> possibleValues = currentNode.getVariableValueList();
    int numberOfPossibleValues = possibleValues.size();
    std::vector<std::string> probabilities = row.getProbabilities();
    double refinedProbabilites[numberOfPossibleValues][2];
    index = 0;
    for(std::string prob : probabilities){
        double p = atof(probabilities.at(index).c_str());
        refinedProbabilites[index][0] = 1 - p;
        refinedProbabilites[index][1] = p;
        index ++;
    }


    std::vector<ProbabilityRow> extraRows = createExtraRows(newParentsNames, newParentsValues, nodeName);
    /* create new binarized rows from the row */
    for(int i = 0; i < numberOfPossibleValues; i ++){
        std::vector<std::string> newNodeProbabilities = {to_string(refinedProbabilites[i][0]), to_string(refinedProbabilites[i][1])};
        ProbabilityRow row = ProbabilityRow(newParentsValues, newNodeProbabilities, newParentsNames, {"false", "true"});
        (nodeNamesToBinarizedRows[nodeName + possibleValues.at(i)]).push_back(row);

        for(ProbabilityRow extraRow : extraRows){
            nodeNamesToBinarizedRows[nodeName + possibleValues.at(i)].push_back(extraRow);
        }
    }



}


std::vector<ProbabilityRow> NaiveBinarizator::createExtraRows(std::vector<std::string> binarizedParentsNames,
        std::vector<std::string> binarizedParentsValues, std::string nodeName){

    std::vector<ProbabilityRow> extraRows;
    std::unordered_map<std::string, int> parentValuesMap;
        std::string encodedParentValues = "";
        for(std::string parentValue : binarizedParentsValues){
            encodedParentValues += (parentValue == "true") ? "1" : "0";
        }
        parentValuesMap[encodedParentValues] = 1;

    std::vector<std::string> newParentsValues;
    int n = binarizedParentsNames.size();
    int mustNumberOfRows = pow(2, n);
    for(int i = 0; i < mustNumberOfRows; i ++){
        std::string binarizedNumber = decToBinary(i, n);
        if(parentValuesMap.find(binarizedNumber) != parentValuesMap.end()){
            for(std::string::size_type i = 0; i < binarizedNumber.size(); ++i) {
                newParentsValues.push_back(binarizedNumber);
            }
        }
    }

    for(std::string binarizedNumber : newParentsValues){
        std::vector<std::string> thisParentValues;
        for(std::string::size_type i = 0; i < binarizedNumber.size(); ++i) {
            if(binarizedNumber[i] == '1')
                thisParentValues.push_back("true");
            if(binarizedNumber[i] == '0')
                thisParentValues.push_back("false");
        }

        ProbabilityRow extraRow = ProbabilityRow(thisParentValues, {"1.0", "0.0"}, binarizedParentsNames,
                {"false", "true"});
        extraRows.push_back(extraRow);
    }

    return extraRows;
}


void NaiveBinarizator::decToBinary(int n, int numberOfBits, int binarizedNumber[]){

    for (int i = numberOfBits; i >= 0; i--) {
        int k = n >> i;
        if (k & 1)
            binarizedNumber[i] = 1;
        else
            binarizedNumber[i] = 0;
    }
}

std::string NaiveBinarizator::decToBinary(int n, int numberOfBits){
    int array[numberOfBits];
    decToBinary(n, numberOfBits, array);
    std::string binarizedNumber = "";
    for(int i = 0; i < numberOfBits; i ++){
        if(array[i] == 1)
            binarizedNumber += "1";
        if(array[i] == 0)
            binarizedNumber += "0";
    }
    return binarizedNumber;
}



std::vector<BNNode> NaiveBinarizator::getBinarizedNodes() {
    return binarizedNodes;
}

std::vector<ProbabilityTable> NaiveBinarizator::getBinarizedTables(){
    return binarizedTables;
}

BNNetwork NaiveBinarizator::getBinarizedNetwork() {
    BNNetwork binarizedBN = BNNetwork(bnNetwork.getNetworkName(), binarizedNodes, binarizedTables,
                                      bnNetwork.getEvidences(), bnNetwork.getHypothesises(), bnNetwork.getParameterNames());
    return binarizedBN;
}
