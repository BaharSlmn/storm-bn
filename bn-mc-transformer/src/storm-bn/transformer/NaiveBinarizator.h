//
// Created by Bahare Salmani on 2020-03-25.
//

#ifndef STORM_NAIVEBINARIZATOR_H
#define STORM_NAIVEBINARIZATOR_H

#include "../parser/BNNetwork.h"
#include "../parser/BNNode.h"
#include "../parser/ProbabilityRow.h"
#include "../parser/ProbabilityTable.h"
#include <math.h>
#include "unordered_map"


class NaiveBinarizator {
    private:
        BNNetwork bnNetwork;
        std::vector<BNNode> binarizedNodes;
        std::vector<ProbabilityTable> binarizedTables;
        std::unordered_map<std::string, std::vector<BNNode>> nodeNamesToValueNodes;
        std::unordered_map<std::string, BNNode> nodeNamesToConsistencyNode;

        std::map<std::string, std::vector<ProbabilityRow>> nodeNamesToBinarizedRows;


        void binarizeNode(BNNode node);
        void binarizeTable(ProbabilityTable table);
        void keepValidityNode(BNNode node);
        void keepValidityTable(BNNode node);

        std::vector<ProbabilityRow> createExtraRows(std::vector<std::string> binarizedParentsNames,
                                                    std::vector<std::string> binarizedParentsValues, std::string nodeName);
        void binarizeRow(ProbabilityRow row, std::string nodeName);
        void binarize();



public:
        NaiveBinarizator(){}
        NaiveBinarizator(BNNetwork bn);
        BNNetwork getBinarizedNetwork();
        std::vector<BNNode> getBinarizedNodes();
        std::vector<ProbabilityTable> getBinarizedTables();
        void decToBinary(int n, int numberOfBits, int binarizedValue[]);
        std::string decToBinary(int n, int numberOfBits);
};


#endif //STORM_NAIVEBINARIZATOR_H
