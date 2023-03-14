//
// Created by Bahare Salmani on 2019-09-02.
//

#include "MarkovianState.h"


MarkovianState::MarkovianState( std::pair<std::string, std::string> eval) {
    evaluation.insert(eval);
   // std::cout << "Bebinin chi mishe " << eval.first << " " << eval.second << std::endl;
   /* for(std::pair<std::string, std::string> keyVal : row.getGuard()){
        evaluation.insert(keyVal);
    } */
    reachabilityProbability = 1;
}

MarkovianState::MarkovianState(std::pair<std::string, std::string> newAssignment, ProbabilityTable table, ProbabilityRow row, std::vector<MarkovianState> nextLevelStates, Evidence* evidence) {
    reachabilityProbability = 0;
    this->newAssignment = newAssignment;
    this->row = row;
    this->iPlusOneTable = table;
    this->evidence = evidence;
    this->nextLevelStates = nextLevelStates;
    evaluation.insert(newAssignment);
    for(std::pair<std::string, std::string> keyValue : row.getGuard()){
        evaluation.insert(keyValue);
    }
    build();
    calculateReachabilityProbability();
}


void MarkovianState::build() {

    for(MarkovianState s : nextLevelStates){
        if (isChild(s)){
            children.push_back(s);
        }
    }

    for(MarkovianState child : children) {
        std::map<std::string, std::string> childEvals = child.getKeyValues();
        for (std::pair<std::string, std::string> vv : childEvals) {
            if (evaluation.find(vv.first) == evaluation.end() && vv.first != iPlusOneTable.getNodeName())
                evaluation.insert(vv);
        }
    }
}

bool MarkovianState::isChild(MarkovianState theState){
    bool match = true;
    std::map<std::string, std::string> evals = theState.getKeyValues();

    for(std::pair<std::string, std::string> eval : evals){
        if((evaluation.count(eval.first) > 0 && evaluation[eval.first] != eval.second) ||
           (newAssignment.first == eval.first && newAssignment.second != eval.second) ) {
            match = false;
        }
    }
    return match;
}

void MarkovianState::calculateReachabilityProbability() {
    for(MarkovianState child : children){
        reachabilityProbability += child.getReachabilityProbability() * getEdgeProbability(child);
       // std::cout << iPlusOneTable.getNodeName() << ":" << child.getReachabilityProbability() << ", " << getEdgeProbability(child) << std::endl;
    }
}

std::map<std::string, std::string> MarkovianState::getKeyValues(){
    return evaluation;
}

std::string MarkovianState::getValue(std::string key){
    return evaluation[key];
}


double MarkovianState::getReachabilityProbability() {
    return reachabilityProbability;
}


double MarkovianState::getEdgeProbability(MarkovianState child){
    double retVal;
    std::string value;

    /* check for child not violating the evidence */
    if(evidence->isViolated(child.getKeyValues())){
        retVal = 0;
        return retVal;
    }

     if(child.getKeyValues().count(iPlusOneTable.getNodeName()) == 0){
        /*retVal = 1.0 / iPlusOneTable.getNode().getNumberOfValues();
        return  retVal; */
    }

    value = child.getValue(iPlusOneTable.getNodeName());
    retVal = std::stod(row.getProbabilityOfAPossibleValue(value));
    return retVal;
}


MarkovianInitState::MarkovianInitState(ProbabilityTable firstTable, ProbabilityRow row, std::vector<MarkovianState> firstLevel, Evidence* evidence) {
    MarkovianState();
    this->nextLevelStates = firstLevel;
    this->iPlusOneTable = firstTable;
    this->row = row;
    this->evidence = evidence;
    build();
}

void MarkovianInitState::build() {
    reachabilityProbability = 0;
    /* all the states in the first level are the children of init state as the init state does not have any key values set
    * and also the first level has no gaurd to be violated */
    for(MarkovianState s : nextLevelStates){
        children.push_back(s);
    }
    calculateReachabilityProbability();
}