//
// Created by Bahare Salmani on 2019-09-19.
//

#include "../parser/BNNetwork.h"
#include "../parser/Evidence.h"
#include "MarkovianChainLevel.h"
#include "MarkovianState.h"

#ifndef BNPARSER_EVIDENCEINJECTOR_H
#define BNPARSER_EVIDENCEINJECTOR_H


class EvidenceInjector {
private:
    BNNetwork bnNetwork;
    Evidence* evidence;
    int topoOrder;
    MarkovianChainLevel lastLevel;
    MarkovianChainLevel beforeLastLevel;
    void buildMarkovianChain();
public:
    int determinIndexOfLastObservedNode();
    EvidenceInjector(BNNetwork bnNetwork, Evidence* evidence);
};


#endif //BNPARSER_EVIDENCEINJECTOR_H
