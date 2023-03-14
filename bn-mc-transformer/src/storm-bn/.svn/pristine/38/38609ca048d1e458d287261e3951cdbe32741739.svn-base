//
// Created by Bahare Salmani on 2019-09-19.
//

#include "EvidenceInjector.h"


EvidenceInjector::EvidenceInjector(BNNetwork bnNetwork, Evidence* evidence) {
    this->bnNetwork = bnNetwork;
    this->evidence = evidence;
    determinIndexOfLastObservedNode();
    //buildMarkovianChain();
}

int EvidenceInjector::determinIndexOfLastObservedNode() {
    std::vector<ProbabilityTable> tables = bnNetwork.getSortedProbabilityTables();
    ProbabilityTable lastObservedTable;
    for (vector<ProbabilityTable>::reverse_iterator i = tables.rbegin();
         i != tables.rend(); ++i ) {
        if((*i).isTheNodeObserved()){
            lastObservedTable = *i;
            break;
        }
    }
    topoOrder = bnNetwork.getTopologicalOrder(lastObservedTable.getNodeName());
    return topoOrder;

}


void EvidenceInjector::buildMarkovianChain() {

    std::vector<ProbabilityTable> tables = bnNetwork.getSortedProbabilityTables();
    if(tables.empty())
        throw "there is no Table";
    BNNode firstObservedNode = tables.at(topoOrder - 1).getNode(); /* topoOrder is 1 based */
    lastLevel = MarkovianChainLevel(firstObservedNode, &bnNetwork.getEvidences().at(0));
    for(MarkovianState state : lastLevel.getStates()){
        for(std::pair<std::string, std::string> keyValue : state.getKeyValues()){
            std::cout << keyValue.first << " " << keyValue.second << " ";
        }
        std::cout << state.getReachabilityProbability() << std::endl;
    }

    while(topoOrder >= 2){
        BNNode iThNode = tables.at(topoOrder - 2).getNode();
        ProbabilityTable iPlusOneTable = tables.at(topoOrder - 1);
        beforeLastLevel = MarkovianChainLevel(iThNode, iPlusOneTable, &lastLevel, evidence);
        lastLevel = beforeLastLevel;
        for(MarkovianState state : lastLevel.getStates()){
            for(std::pair<std::string, std::string> keyValue : state.getKeyValues()){
                std::cout << keyValue.first << " " << keyValue.second << " ";
            }
            std::cout << state.getReachabilityProbability() << std::endl;
        }

        topoOrder --;
    }

    MarkovianChainInitLevel init = MarkovianChainInitLevel(tables.at(0), &lastLevel, evidence);
    for(MarkovianState state : init.getStates()){
        for(std::pair<std::string, std::string> keyValue : state.getKeyValues()){
            std::cout << keyValue.first << " " << keyValue.second << " ";
        }
        std::cout << state.getReachabilityProbability() << std::endl;
    }
}