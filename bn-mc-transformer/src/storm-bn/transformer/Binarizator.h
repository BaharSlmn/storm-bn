//
// Created by Bahare Salmani on 2020-03-11.
//

#ifndef STORM_BINARIZATOR_H
#define STORM_BINARIZATOR_H

#include "../parser/BNNetwork.h"
#include "../parser/BNNode.h"
#include "../parser/ProbabilityRow.h"
#include "../parser/ProbabilityTable.h"
#include <math.h>

class Binarizator {
private:
    BNNetwork bnNetwork;
    std::vector<BNNode> binarizedNodes;
    std::vector<ProbabilityTable> binarizedTables;
    std::map<std::string, std::vector<BNNode>> nodeNamesToBinarizedNodes;
    std::map<std::string, std::vector<ProbabilityRow>> nodeNamesToBinarizedRows;

    void binarizeNode(BNNode node);
    void binarizeTable(ProbabilityTable table);
    void binarizeRow(ProbabilityRow row, std::string nodeName);
    void binarize();


public:
    Binarizator(){}
    Binarizator(BNNetwork bn);
    BNNetwork getBinarizedNetwork();
    std::vector<BNNode> getBinarizedNodes();
    std::vector<ProbabilityTable> getBinarizedTables();
    void decToBinary(int n, int numberOfBits, int binarizedValue[]);


};


#endif //STORM_BINARIZATOR_H
