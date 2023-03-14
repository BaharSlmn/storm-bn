// Created by Bahare Salmani on 2020-03-25.
//

#ifndef STORM_CHILDBINARIZATOR_H
#define STORM_CHILDBINARIZATOR_H

#include "../parser/BNNetwork.h"
#include "../parser/BNNode.h"
#include "../parser/ProbabilityRow.h"
#include "../parser/ProbabilityTable.h"
#include <math.h>
#include "unordered_map"


class ChildBinarizator {
private:
    BNNetwork& bnNetwork_;
    std::vector<BNNode> binarizedNodes;
    std::vector<ProbabilityTable> binarizedTables;

    std::unordered_map<std::string, std::vector<BNNode>> nodeNamesToValueNodes;
    std::unordered_map<std::string, std::vector<ProbabilityRow>> nodeNamesToBinarizedRows; /* here something has changed */
    void binarizeNode(const BNNode& node);
    void binarizeTable(const ProbabilityTable& table);

    std::vector<std::vector<std::string>> createExtraRows(const std::vector<std::string>& binarizedParentsNames,
                                                          const std::vector<std::vector<std::string>>& alreadyThereParentValues);
    void binarizeRow(const ProbabilityRow& row, std::string nodeName);
    void binarize();



public:
    ChildBinarizator(BNNetwork& bn);
    const BNNetwork& getBinarizedNetwork();
    const std::vector<BNNode>& getBinarizedNodes();
    const std::vector<ProbabilityTable>& getBinarizedTables();
    void decToBinary(int n, int numberOfBits, int binarizedValue[]);
    std::string decToBinary(int n, int numberOfBits);
};


#endif //STORM_MATUREBINARIZATOR_H