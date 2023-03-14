//
// Created by Bahare Salmani on 2019-10-11.
//

#include "HypothesisInjector.h"


HypothesisInjector::HypothesisInjector(BNNetwork bnNetwork, Hypothesis *hypothesis) {
    this->bnNetwork = bnNetwork;
    this->hypothesis = hypothesis;
    determinIndexOfLastQuestionedNode();
}

int HypothesisInjector::determinIndexOfLastQuestionedNode() {
    std::vector<ProbabilityTable> tables = bnNetwork.getSortedProbabilityTables();
    ProbabilityTable lastQuestionedTable;
    for (vector<ProbabilityTable>::reverse_iterator i = tables.rbegin();
         i != tables.rend(); ++i ) {
        if((*i).isTheNodeQuestioned()){
            lastQuestionedTable = *i;
            break;
        }
    }
    topoOrder = bnNetwork.getTopologicalOrder(lastQuestionedTable.getNodeName());
    return topoOrder;

}