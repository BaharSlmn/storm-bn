//
// Created by Bahare Salmani on 2019-04-09.
//

#ifndef BNPARSER_BNNODE_H
#define BNPARSER_BNNODE_H

#include "BIFFORMAT.h"
#include "Utils.h"
#include<string>
#include<vector>
#include<regex>

class BNNode {
    public:
        BNNode() {}
        BNNode(std::string declaration);
        BNNode(std::string nodeName, int domain, std::vector<std::string> variableValues, bool observed, bool questioned);
        std::string getNodeName() const;
        int getNumberOfValues() const;
        std::vector<std::string> getVariableValueList() const;
        int getIndexOfAParticularValue(std::string value) const;
        bool isObserved() const;
        bool isQuestioned() const;
        void markAsObserved();
        void markAsQuestioned();

private:
    /* for discrete values */
        std::string nodeName;
        int numberOfValues;
        std::vector <std::string> variableValuesList;
        bool observed;
        bool questioned;

        void setNodeName();
        void setNumberOfValues();
        void setVariableValuesList();

        std::string nodeDeclaration;
};


#endif //BNPARSER_BNNODE_H
