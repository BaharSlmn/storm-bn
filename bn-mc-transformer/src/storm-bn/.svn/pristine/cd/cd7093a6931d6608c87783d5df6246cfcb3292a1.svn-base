//
// Created by Bahare Salmani on 2019-09-02.
//

#ifndef BNPARSER_MARKOVIANSTATE_H
#define BNPARSER_MARKOVIANSTATE_H

#include "vector"
#include "map"
#include "../parser/BNNode.h"
#include "../parser/ProbabilityRow.h"
#include "../parser/ProbabilityTable.h"
#include "../parser/Evidence.h"




class MarkovianState{
private:
    std::pair<std::string, std::string> newAssignment;
protected:
    int topologicalLevel;
    ProbabilityRow row;
    ProbabilityTable iPlusOneTable;
    Evidence* evidence;
    double reachabilityProbability;
    std::map<std::string, std::string> evaluation;
    std::vector<MarkovianState> nextLevelStates;
    std::vector<MarkovianState> children;

    void build();
    void calculateReachabilityProbability();
    bool isChild(MarkovianState theState);
    double getEdgeProbability(MarkovianState child);

public:
    MarkovianState(){}
    MarkovianState(std::pair<std::string, std::string> newAssignment, ProbabilityTable table, ProbabilityRow row, std::vector<MarkovianState> nextLevelStates, Evidence* evidence);
    MarkovianState( std::pair<std::string, std::string> newAssignment);
    std::map<std::string, std::string> getKeyValues();
    std::string getValue(std::string key);
    double getReachabilityProbability();
};

class MarkovianInitState : public MarkovianState{
private:
    void build();
public:
    MarkovianInitState(ProbabilityTable firstTable, ProbabilityRow row, std::vector<MarkovianState> firstLevel, Evidence* evidence);
};

#endif //BNPARSER_MARKOVIANSTATE_H
