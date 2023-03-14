//
// Created by Bahare Salmani on 2019-08-30.
//

enum AlgorithmState {before, normal};


#include "MarkovianChainLevel.h"


MarkovianChainLevel::MarkovianChainLevel(BNNode node, Evidence* evidence) {
    this->ithNode = node;
    this->evidence = evidence;
   // std::cout << "injaram check konim " << node.getNodeName() << " " <<  evidence->getObservations()[node.getNodeName()] << std::endl;
    MarkovianState reachedState = MarkovianState(std::make_pair(node.getNodeName(), evidence->getObservations()[node.getNodeName()]));
    states.push_back(reachedState);

}


MarkovianChainLevel::MarkovianChainLevel(BNNode ithNode, ProbabilityTable table, MarkovianChainLevel* nextLevel, Evidence* evidence){
    this->ithNode = ithNode;
    this->iPlusOneTable = table;
    this->iPlusOneLevel = nextLevel;
    this->evidence = evidence;
    build();
}


void MarkovianChainLevel::build() {
    std::vector<ProbabilityRow> rows = iPlusOneTable.getRows();
    std::vector<std::string> values = ithNode.getVariableValueList();

    for (ProbabilityRow row : rows) {
        for (std::string nodeValue: values) {
            std::pair<std::string, std::string> assignment = std::make_pair(ithNode.getNodeName(), nodeValue);
            if(row.isGuardStatisfied(assignment) && !evidence->isViolated(assignment)){
                MarkovianState newState = MarkovianState(assignment, iPlusOneTable, row, iPlusOneLevel->getStates(), evidence);
                states.push_back(newState);
            }
        }
    }
}

std::vector<MarkovianState> MarkovianChainLevel::getStates() {
    return states;
}


MarkovianChainInitLevel::MarkovianChainInitLevel(ProbabilityTable table, MarkovianChainLevel *nextLevel,
                                                 Evidence *evidence) {
    this->iPlusOneTable = table;
    this->iPlusOneLevel = nextLevel;
    this->evidence = evidence;
    for(ProbabilityRow row : table.getRows()){ /* we have only one row here */
        MarkovianInitState initState(table, row, iPlusOneLevel->getStates(), evidence);
        states.push_back(initState);
    }
}
