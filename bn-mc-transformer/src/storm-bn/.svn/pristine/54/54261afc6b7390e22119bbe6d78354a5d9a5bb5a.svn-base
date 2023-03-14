//
// Created by Bahare Salmani on 2019-10-11.
//

#ifndef BNPARSER_HYPOTHESIS_H
#define BNPARSER_HYPOTHESIS_H


#include <map>
#include <string>
#include "BIFFORMAT.h"
#include "Utils.h"



class Hypothesis {

private:
    std::string hypothesisDeclaration;
    std::map<std::string, std::string> evaluations;

public:
    Hypothesis(std::string declaration);
    std::map<std::string, std::string> getEvaluations();
    std::string getValue(std::string key);
    void parseHypothesis();
    void parseAssignment(std::string str);
    bool isViolated(std::map<std::string, std::string>);
    bool isViolated(std::pair<std::string, std::string>);
};



#endif //BNPARSER_HYPOTHESIS_H
