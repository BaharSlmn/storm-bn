//
// Created by Bahare Salmani on 2020-03-11.
//

#include "Binarizator.h"
#include <stdlib.h>     /* atof */


Binarizator::Binarizator(BNNetwork bn) {
    bnNetwork = bn;
    binarize();
}

void Binarizator::binarize() {
    std::string retValue;
    for(BNNode node : bnNetwork.getNodes()){
        binarizeNode(node);
    }

    for(ProbabilityTable table : bnNetwork.getSortedProbabilityTables()){
        binarizeTable(table);
    }

    for(Evidence ev : bnNetwork.getEvidences()){
        //binarizeEvidence();
    }

    for(Hypothesis hyp : bnNetwork.getHypothesises()){
        //binarizeHypothesis();
    }
}

void Binarizator::binarizeNode(BNNode node) {
   /* if(node.getNumberOfValues() > 2){ ommitted to make it general */
        int numberOfNewNodes = ceil(log2(node.getNumberOfValues()));
        std::vector<std::string> valuesCopy = node.getVariableValueList();
        std::vector<BNNode> newBinarizedNodes;
        for(int i = 0; i < numberOfNewNodes; i++){
            BNNode newNode = BNNode(node.getNodeName() + std::to_string(i), 2,
                                    {"false", "true"}, node.isObserved(), node.isQuestioned());
            binarizedNodes.push_back(newNode);
            newBinarizedNodes.push_back(newNode);
        }
        nodeNamesToBinarizedNodes[node.getNodeName()] = newBinarizedNodes;
  /*  } else{
        nodeNamesToBinarizedNodes[node.getNodeName()] = {node};
    } */
}

void Binarizator::binarizeTable(ProbabilityTable table) {
    std::vector<BNNode> thisBinaryNodes = nodeNamesToBinarizedNodes[table.getNodeName()];

    std::vector<BNNode> binarizedParents;
    std::vector<BNNode> parents = table.getParents();
    for(BNNode parent : parents){
        std::vector<BNNode> thisBinarizedParents = nodeNamesToBinarizedNodes[parent.getNodeName()];
        binarizedParents.insert(binarizedParents.begin(),
                thisBinarizedParents.begin(),thisBinarizedParents.end());

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

void Binarizator::binarizeRow(ProbabilityRow row, std::string nodeName) {

    /* binarize the values that the parents are evaluated to, in the row */
    std::vector<std::string> parentsValues = row.getParentValues();
    std::vector<std::string> parentsNames = row.getParentsNames();
    std::vector<std::string> newParentsNames;
    std::vector<std::string> newParentsValues;
    std::map<std::string, std::string> newParentValuesMapping;
    int index = 0;
    for(std::string parentValue : parentsValues){
         BNNode node = bnNetwork.getNodeByName(parentsNames.at(index)); /* the error is here */
        int numberOfParentBits = ceil(log2(node.getNumberOfValues()));
        index++;
        int valueIndex = node.getIndexOfAParticularValue(parentValue);
        int binarizedValue[numberOfParentBits];
        decToBinary(valueIndex, numberOfParentBits, binarizedValue);
        for(int i = 0; i < numberOfParentBits; i++){
            std::string newParentName = node.getNodeName() + std::to_string(i);
            newParentsNames.push_back(newParentName);
            if(binarizedValue[i] == 1) {
                newParentValuesMapping[newParentName] = "true";
                newParentsValues.push_back("true");
            }if(binarizedValue[i] == 0){
                newParentValuesMapping[newParentName] = "false";
                newParentsValues.push_back("false");
            }
        }
    }

    /* Binarize the probability entries in the row */
    std::vector<std::string> probabilities = row.getProbabilities();
    int numberOfValues = probabilities.size();
    int numberOfBits = ceil(log2(numberOfValues));

    double refinedProbabilites[numberOfBits][2];

    for(int i = 0; i < numberOfBits; i++){
        for(int index = 0; index < numberOfValues; index++){
            int binarizedNum[numberOfBits];
            decToBinary(index, numberOfBits, binarizedNum);
            if(binarizedNum[i] == 0){
                refinedProbabilites[i][0] += atof(probabilities.at(index).c_str());
            } else
                refinedProbabilites[i][1] += atof(probabilities.at(index).c_str());
        }
        std::vector<std::string> newNodeProbabilities = {to_string(refinedProbabilites[i][0]), to_string(refinedProbabilites[i][1])};
        ProbabilityRow row = ProbabilityRow(newParentsValues, newNodeProbabilities, newParentsNames, {"false", "true"});
        (nodeNamesToBinarizedRows[nodeName + std::to_string(i)]).push_back(row);
    }


}

void Binarizator::decToBinary(int n, int numberOfBits, int binarizedNumber[])
{

    for (int i = numberOfBits; i >= 0; i--) {
        int k = n >> i;
        if (k & 1)
            binarizedNumber[i] = 1;
        else
            binarizedNumber[i] = 0;
    }
}


std::vector<BNNode> Binarizator::getBinarizedNodes() {
    return binarizedNodes;
}

std::vector<ProbabilityTable> Binarizator::getBinarizedTables(){
    return binarizedTables;
}

BNNetwork Binarizator::getBinarizedNetwork() {
    BNNetwork binarizedBN = BNNetwork(bnNetwork.getNetworkName(), binarizedNodes, binarizedTables,
                                      bnNetwork.getEvidences(), bnNetwork.getHypothesises(), bnNetwork.getParameterNames());
    return binarizedBN;
}