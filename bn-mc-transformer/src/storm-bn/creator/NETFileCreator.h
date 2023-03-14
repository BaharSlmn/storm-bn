//
// Created by Bahare Salmani on 2020-03-18.
//

#ifndef STORM_NETFILECREATOR_H
#define STORM_NETFILECREATOR_H

#include "storm-bn/parser/BNNetwork.h"
#include "storm-bn/parser/BNNode.h"
#include "storm-bn/parser/ProbabilityTable.h"
#include "storm-bn/parser/ProbabilityRow.h"
#include "literals.h"


class NETFileCreator {

private:
    const std::vector<BNNode>& nodes;
    const std::vector<ProbabilityTable>& tables;
    std::string buildNodes();
    std::string buildTables();
    bool isRow1lowerEqRow2(const ProbabilityRow& row1, const ProbabilityRow& row2, const std::vector<BNNode>& parents);
    void plusPlus(int parentsCurrentOrders[], int size, const std::vector<BNNode>& parentNodes);
    std::vector<ProbabilityRow> sortTheRows(const ProbabilityTable &table);
        /* sort the tables according to .NET format specifications,
         * which needs the rows come based on the order of parent possible values, from the first to the last according to NET file format ordering on parents
        * for example when the node has three parents and each parents has two possible values,
        * this is the order in the table:
             * parent1: first value, parent2: first value, parent3: first Value
             * parent1: first value, parent 2: first value, parent3: second value
             * parent1: first value, parent2: second value, parent3: first value
             * parent1: first value, parent2: second value, parent 3: second value
             * parent1: second value, parent2: first value, parent3: first Value
             * parent1: second value, parent 2: first value, parent3: second value
             * parent1: second value, parent2: second value, parent3: first value
             * parent1: second value, parent2: second value, parent 3: second value */


    void merge(const ProbabilityTable& table, int arr[], int l, int m, int r);
    void mergeSort(const ProbabilityTable& table, int arr[], int l, int r);
    void handleParanthesises(const ProbabilityTable& table, std::string paranthesises[]);




    public:
    NETFileCreator(const std::vector<BNNode>& nodes, const std::vector<ProbabilityTable>& tables);
    std::string create();


};


#endif //STORM_NETFILECREATOR_H
