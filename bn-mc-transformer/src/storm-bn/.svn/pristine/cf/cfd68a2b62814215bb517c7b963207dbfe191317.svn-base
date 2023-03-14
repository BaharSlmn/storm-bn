//
// Created by Bahare Salmani on 2019-08-30.
//

#ifndef BNPARSER_MARKOVIANCHAINLEVEL_H
#define BNPARSER_MARKOVIANCHAINLEVEL_H
#include "../parser/BNNetwork.h"
#include "../parser/Evidence.h"
#include "../parser/ProbabilityRow.h"

#include "MarkovianState.h"

/* Computing Conditional Probabilities in
Markovian Models Efficiently?
Extended Version
Christel Baier, Joachim Klein, Sascha Kl¨uppelholz, Steffen M¨arcker */



class MarkovianChainLevel {
private:
    BNNode ithNode;
    void build();

protected:
    ProbabilityTable iPlusOneTable;
    std::vector<MarkovianState> states;
    MarkovianChainLevel* iPlusOneLevel;
    Evidence* evidence;

public:
    MarkovianChainLevel(){};
    MarkovianChainLevel(BNNode node, Evidence* evidence);
    MarkovianChainLevel(BNNode ithNode, ProbabilityTable iPlusOneTable, MarkovianChainLevel* iPlusOneLevel, Evidence* evidence);
    std::vector<MarkovianState> getStates();
};


class MarkovianChainInitLevel : public MarkovianChainLevel{
public:
    MarkovianChainInitLevel(ProbabilityTable table, MarkovianChainLevel* iPlusOneLevel, Evidence* evidence);

};

#endif //BNPARSER_MARKOVIANCHAINLEVEL_H


