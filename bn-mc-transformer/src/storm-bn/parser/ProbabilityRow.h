//
// Created by Bahare Salmani on 2019-04-25.
//

#ifndef BNPARSER_PROBABILITYROW_H
#define BNPARSER_PROBABILITYROW_H


#include <vector>
#include <map>
#include <string>
#include <iostream>
#include "Utils.h"
#include "BIFFORMAT.h"

class ProbabilityRow {

public:
    ProbabilityRow(){}
    ProbabilityRow(std::vector<std::string> parentsValues, std::vector<std::string> probabilities, std::vector<std::string> parentNames, std::vector<std::string> possibleValues);
    ProbabilityRow(std::string text, std::vector<std::string> parentNames, std::vector<std::string> possibleValues);

    std::vector<std::string> getParentsNames() const;
    std::vector<std::string> getParentValues() const;
    std::vector<std::string> getProbabilities() const;
    void setUpdatedProbabilities(std::vector<std::string> updatedProbabilities);
    std::vector<std::string> getUpdatedProbabilties() const;

    std::map<std::string, std::string> getGuard();
    std::map<std::string, std::string> getValuesMappingToProbabilities();

    std::string getValueByTheNameOfParent(std::string parentName) const;
    std::string getProbabilityOfAPossibleValue(std::string possibleValue) const;

    bool isGuardStatisfied(std::pair<std::string, std::string> keyValue);
    bool isGuardStatisfied(std::vector<std::pair<std::string, std::string>> keyValues);




private:
    std::string rowDeclaration;
    std::vector<std::string> parentsNames;
    std::vector<std::string> parentsValues;
    std::vector<std::string> possibleValues;
    std::vector<std::string> probabilities;
    std::vector<std::string> updatedProbabilites;
    std::map<std::string, std::string> guard; /* the values of the parents on this row */
    std::map<std::string, std::string> nodeValuesProbabilities; /* maps each possible value of the dependent node
                                                                   //     to the probability of that value being assigned,
                                                                     //   (on this row with respect to parent values) */
    void parse();
    void setParentsValues();
    void setProbabilities();
    void mapParentsToValues();
    void mapNodeValuesToProbabilities();


};


#endif //BNPARSER_PROBABILITYROW_H


