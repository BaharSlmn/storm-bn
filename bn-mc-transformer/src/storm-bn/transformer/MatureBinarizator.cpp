//
// Created by Bahare Salmani on 2020-03-25.
//

#include "MatureBinarizator.h"
#include <stdlib.h>
#include <stdio.h>

#include <cmath>


MatureBinarizator::MatureBinarizator(BNNetwork& bnNetwork):
        bnNetwork_(bnNetwork)
        {
            binarize();
        }

void MatureBinarizator::binarize() {
    std::string retValue;
    const std::vector<BNNode>& nodes = bnNetwork_.getNodes();
    for(const BNNode& node : nodes){
        binarizeNode(node);
    }

    int i = 0;
    const std::vector<ProbabilityTable>& tables = bnNetwork_.getSortedProbabilityTables();
    for(const ProbabilityTable& table : tables){
        binarizeTable(table);
       // std::cout << "table " << i << "binarized" << std::endl;
        i++;
    }

    for(Evidence ev : bnNetwork_.getEvidences()){
        //binarizeEvidence();
    }

    for(Hypothesis hyp : bnNetwork_.getHypothesises()){
        //binarizeHypothesis();
    }
}

void MatureBinarizator::binarizeNode(const BNNode& node) {
    const std::vector<std::string>& valuesCopy = node.getVariableValueList();
    std::vector<BNNode> newBinarizedNodes;
    for(std::string possibleValue : valuesCopy){
        const BNNode& valueNode = BNNode(node.getNodeName() + "_" + possibleValue, 2,
                                  {"false", "true"}, node.isObserved(), node.isQuestioned());
        binarizedNodes.push_back(valueNode);
        newBinarizedNodes.push_back(valueNode);
    }

    nodeNamesToValueNodes[node.getNodeName()] = newBinarizedNodes;
}



void MatureBinarizator::binarizeTable(const ProbabilityTable& table) {
    std::vector<BNNode> thisBinaryNodes = nodeNamesToValueNodes[table.getNodeName()];


    std::vector<BNNode> binarizedParents;
    std::vector<std::string> binarizedParentsNames;
    const std::vector<BNNode>& parents = table.getParents();
    for(BNNode parent : parents){
        const std::vector<BNNode>& thisBinarizedParents = nodeNamesToValueNodes[parent.getNodeName()];
        binarizedParents.insert(binarizedParents.begin(),
                                thisBinarizedParents.begin(),thisBinarizedParents.end());
    }

    const std::vector<ProbabilityRow>& rows = table.getRows();
    std::vector<ProbabilityRow> newRows;

    for(const ProbabilityRow& row : rows){
        binarizeRow(row, table.getNodeName());
    }


    int index = 0;
    for(BNNode n : thisBinaryNodes){
        if(index > 1)
            binarizedParents.pop_back();
        if(index > 0 & nodeNamesToValueNodes[table.getNodeName()].size() >= index ){
                binarizedParents.push_back(nodeNamesToValueNodes[table.getNodeName()].at(index - 1));
        }


        /* adding extra rows for the sake of .net format */
        const std::vector<ProbabilityRow>& alreadyThereRows = nodeNamesToBinarizedRows[n.getNodeName()];
        std::vector<std::vector<std::string>> alreadyThereParentValues;
        int i = 0;
        for(ProbabilityRow row : alreadyThereRows){
            alreadyThereParentValues.push_back(row.getParentValues());
            std::cout << i << "  " << n.getNodeName() << std::endl;
            i ++;
        }

        std::vector<std::string> binarizedParentsNames;
        for(BNNode node : binarizedParents)
            binarizedParentsNames.push_back(node.getNodeName());

        const std::vector<std::vector<std::string>>& extraRowsParentValues = createExtraRows(binarizedParentsNames, alreadyThereParentValues);
        for (const std::vector<std::string>& extraRowParentValue : extraRowsParentValues) {
            const ProbabilityRow& extraRow = ProbabilityRow(extraRowParentValue, {"1.0", "0.0"}, binarizedParentsNames,
                                                     {"false", "true"});
            (nodeNamesToBinarizedRows[n.getNodeName()]).push_back(extraRow);
        }
        /******************************************** */


        ProbabilityTable newTable(n, binarizedParents, nodeNamesToBinarizedRows[n.getNodeName()]);

        binarizedTables.push_back(newTable);
        index ++;
    }
}

void MatureBinarizator::binarizeRow(const ProbabilityRow& row, std::string nodeName) {

    /* binarize the values that the parents are evaluated to, in the row */
    const std::vector<std::string>& parentsValues = row.getParentValues();
    const std::vector<std::string>& parentsNames = row.getParentsNames();
    std::vector<std::string> newParentsNames;
    std::vector<std::string> newParentsValues;
    std::unordered_map<std::string, std::string> newParentValuesMapping;

    /* create new binarized parents names and values for the row */
    int index = 0;
    for(std::string parentName : parentsNames){
        const BNNode& parentNode = bnNetwork_.getNodeByName(parentsNames.at(index)); /* the error is here */
        const std::vector<std::string>& parentDomain = parentNode.getVariableValueList();
        for(std::string parentValue : parentDomain){
            std::string newParentName = parentName + "_" + parentsValues.at(index);
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
        index ++;
    }

    /* Binarize the probability entries in the row */
    const BNNode& currentNode = bnNetwork_.getNodeByName(nodeName);
    const std::vector<std::string>& possibleValues = currentNode.getVariableValueList();
    int numberOfPossibleValues = possibleValues.size();
    const std::vector<std::string>& probabilities = row.getProbabilities();
    double refinedProbabilites[numberOfPossibleValues][2];
    double sumOfProbsSoFar = 0;
    int i = 0;
    for(std::string prob : probabilities){
        double pi = atof(probabilities.at(i).c_str());
        double qi;
        double OneMinusQi;
        if(pi  == (1 - sumOfProbsSoFar)){
            qi = 1.0;
            OneMinusQi =  abs(1 - qi);

        } else if(sumOfProbsSoFar < 1){
            qi = abs(pi / (1 - sumOfProbsSoFar));
            OneMinusQi = abs(1 - qi);
        }
        refinedProbabilites[i][0] = OneMinusQi;
        refinedProbabilites[i][1] = qi;
        sumOfProbsSoFar += pi;
        i ++;
    }


    std::vector<std::vector<std::string>> alreadyThereParentValues;
    /* create new binarized rows from the row */
    std::string previousValueNodeName;
    for(int i = 0; i < numberOfPossibleValues; i ++){
        std::vector<std::string> newNodeProbabilities = {to_string(refinedProbabilites[i][0]), to_string(refinedProbabilites[i][1])};
        if(i > 0){
            previousValueNodeName = nodeName + "_" + possibleValues.at(i - 1);
            if(i > 1) {
                newParentsNames.pop_back();
                newParentsValues.pop_back();
            }
            newParentsValues.push_back("false");
            newParentsNames.push_back(previousValueNodeName);
        }
        alreadyThereParentValues.push_back(newParentsValues);
        ProbabilityRow row = ProbabilityRow(newParentsValues, newNodeProbabilities, newParentsNames, {"false", "true"});
        (nodeNamesToBinarizedRows[nodeName + "_" + possibleValues.at(i)]).push_back(row);
        if(i > 0){
            newParentsValues.pop_back();
            newParentsValues.push_back("true");
            alreadyThereParentValues.push_back(newParentsValues);
            ProbabilityRow row2 = ProbabilityRow(newParentsValues, {"1.0", "0.0"}, newParentsNames, {"false", "true"});
            (nodeNamesToBinarizedRows[nodeName + "_" + possibleValues.at(i)]).push_back(row2);
        }
    }
}


/* for the sake of .net format */
std::vector<std::vector<std::string>> MatureBinarizator::createExtraRows(const std::vector<std::string>& binarizedParentsNames,
                                                              const std::vector<std::vector<std::string>>& alreadyThereParentValues){

    std::vector<std::vector<std::string>> extraRowsParentValues; /* true/false values */

    std::unordered_map<std::string, int> parentValuesMap;
    for(std::vector<std::string> parentValues :  alreadyThereParentValues){
        std::string encodedParentValues = "";
        for(std::string parentValue : parentValues){
            encodedParentValues += (parentValue.compare("true") == 0) ? "1" : "0";
        }
        parentValuesMap[encodedParentValues] = 1;
    }

    std::vector<std::string> mustAddParentsValues; /* zero one values */
    int n = binarizedParentsNames.size();
    int mustBeNumberOfRows = pow(2, n);
    //std::cout << mustBeNumberOfRows << std::endl;
    for(int i = 0; i < mustBeNumberOfRows; i ++){
        std::string binarizedNumber = decToBinary(i, n);
        if(parentValuesMap.find(binarizedNumber) == parentValuesMap.end()){
            mustAddParentsValues.push_back(binarizedNumber);
        }
    }

    for(std::string binarizedNumber : mustAddParentsValues){
        std::vector<std::string> thisParentValues;
        for(std::string::size_type i = 0; i < binarizedNumber.size(); ++i) {
            if(binarizedNumber[i] == '1')
                thisParentValues.push_back("true");
            if(binarizedNumber[i] == '0')
                thisParentValues.push_back("false");
        }
        extraRowsParentValues.push_back(thisParentValues);
    }

    return extraRowsParentValues;
}


void MatureBinarizator::decToBinary(int n, int numberOfBits, int binarizedNumber[]){

    for (int i = numberOfBits; i >= 0; i--) {
        int k = n >> i;
        if (k & 1)
            binarizedNumber[i] = 1;
        else
            binarizedNumber[i] = 0;
    }
}

std::string MatureBinarizator::decToBinary(int n, int numberOfBits){
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



const std::vector<BNNode>& MatureBinarizator::getBinarizedNodes() {
    return binarizedNodes;
}

const std::vector<ProbabilityTable>& MatureBinarizator::getBinarizedTables(){
    return binarizedTables;
}

const BNNetwork& MatureBinarizator::getBinarizedNetwork() {
    BNNetwork binarizedBN = BNNetwork(bnNetwork_.getNetworkName(), binarizedNodes, binarizedTables,
                                      bnNetwork_.getEvidences(), bnNetwork_.getHypothesises(), bnNetwork_.getParameterNames());
    return binarizedBN;
}
