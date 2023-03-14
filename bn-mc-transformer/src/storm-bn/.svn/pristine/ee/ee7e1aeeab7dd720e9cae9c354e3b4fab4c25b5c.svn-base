//
// Created by Bahare Salmani on 2019-10-11.
//

#ifndef BNPARSER_HYPOTHESISINJECTOR_H
#define BNPARSER_HYPOTHESISINJECTOR_H

#include "../parser/BNNetwork.h"
#include "../parser/Hypothesis.h"

class HypothesisInjector {
private:
    BNNetwork bnNetwork;
    Hypothesis* hypothesis;
    int topoOrder;
    void buildMarkovianChain();
public:
    int determinIndexOfLastQuestionedNode();
    HypothesisInjector(BNNetwork bnNetwork, Hypothesis* hypothesis);
};


#endif //BNPARSER_HYPOTHESISINJECTOR_H
