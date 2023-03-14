//
// Created by Bahare Salmani on 2019-05-06.
//

#include "JaniFileCreator.h"
#include <cmath>



JaniFileCreator::JaniFileCreator(BNNetwork network) {
    bnNetwork = network;
    networkName = bnNetwork.getNetworkName();
    networkSize = bnNetwork.getNodes().size();
    maximumDepth = determinMaxDepth();
    std::cout << "maxDepth" << maximumDepth << std::endl;

    /* configure expression parser with model parameters, in order to let models be parametric */
    std::vector<storm::expressions::Expression> constraints;
    manager = std::make_shared<storm::expressions::ExpressionManager>();
    for(std::string paramName : bnNetwork.getParameterNames()){
        auto param = manager->declareRationalVariable(paramName);
        variableMapping[paramName] = param;
    }
}


std::string JaniFileCreator::create(){
    std::string retValue = "";
    retValue += OPENING_BRACE + NEW_LINE;
    retValue += createHeaderBlock();
    retValue += createVariablesBlock();
    retValue += createConstantsBlock();
    retValue += createProperties();
    retValue += createAutomata();
    retValue += createSystem();
    retValue += CLOSING_BRACE + NEW_LINE;
    return retValue;
}

std::string JaniFileCreator::createHeaderBlock() {
    std::string retValue = "";
    retValue += TAB + JANI_VERSION + COLON + SPACE + version + COMMA + NEW_LINE;
    retValue += TAB + NAME + COLON + SPACE + DOUBLE_QUOTE + bnNetwork.getNetworkName() + ".jani" +  DOUBLE_QUOTE + COMMA + NEW_LINE;
    retValue += TAB + TYPE + COLON + SPACE + DTMC + COMMA + NEW_LINE;
    retValue += TAB + FEATURES + COLON + SPACE + DERIVED_OPERATORS + COMMA + NEW_LINE;
    return retValue;
}

std::string JaniFileCreator::createConstantsBlock(){
    std::string retValue = "";
    if(bnNetwork.getParameterNames().size() == 0)
        return retValue;
    retValue += CONSTANTS + COLON + SPACE + OPENING_BRAKET + NEW_LINE;
    for(std::string paramName : bnNetwork.getParameterNames()){
        retValue += TAB + OPENING_BRACE + NEW_LINE;
        retValue += TAB + TAB + NAME + COLON + SPACE + DOUBLE_QUOTE + paramName + DOUBLE_QUOTE + COMMA + NEW_LINE;
        retValue += TAB + TAB + TYPE + COLON + SPACE + REAL + NEW_LINE;
        retValue += TAB + CLOSING_BRACE + COMMA + NEW_LINE;
    }

    if(bnNetwork.getParameterNames().size() > 0){
        retValue.erase(retValue.end() - 2, retValue.end() - 1);
    }
    retValue += CLOSING_BRAKET + COMMA + NEW_LINE;
    return retValue;
}

std::string JaniFileCreator::createVariablesBlock() {
    std::string retValue = "";
    retValue += VARIABLES + COLON + SPACE + OPENING_BRAKET + NEW_LINE;
    for(BNNode node: bnNetwork.getNodes()){
            retValue += TAB + createVariableBlock(node.getNodeName(), node.getNumberOfValues());
            retValue += COMMA + NEW_LINE;
    }

    if(networkSize > 0){
        retValue.erase(retValue.end() - 2, retValue.end() - 1);
    }
    retValue += CLOSING_BRAKET + COMMA + NEW_LINE;
    return retValue;
}

std::string JaniFileCreator::createVariableBlock(std::string nodeName, int numberOfValues) {
    std::string retValue = "";
    retValue += OPENING_BRACE + NEW_LINE;
    retValue += TAB + NAME + COLON + SPACE + DOUBLE_QUOTE + nodeName + DOUBLE_QUOTE + COMMA + NEW_LINE;
    retValue += TAB + TYPE + COLON + SPACE + OPENING_BRACE + NEW_LINE;
    retValue += TAB + TAB + BASE + COLON + SPACE + INT + COMMA + NEW_LINE;
    retValue += TAB + TAB + KIND + COLON + SPACE + BOUNDED + COMMA + NEW_LINE;
    retValue += TAB + TAB + LOWER_BOUND + COLON + SPACE + "-1" + COMMA + NEW_LINE;
    retValue += TAB + TAB + UPPER_BOUND + COLON + SPACE + std::to_string(numberOfValues - 1) + NEW_LINE;
    retValue += TAB + CLOSING_BRACE + COMMA + NEW_LINE;
    retValue += TAB + INITIAL_VALUE + COLON + SPACE + "-1" + NEW_LINE;
    retValue += CLOSING_BRACE;
    return  retValue;
}

std::string JaniFileCreator::createProperties() {
    std::string retValue = "";
    retValue += PROPERTIES + COLON + SPACE + OPENING_BRAKET + NEW_LINE;
    /* to be impelemented Later */
    retValue += CLOSING_BRAKET + COMMA + NEW_LINE;
    return retValue;
}

std::string JaniFileCreator::createAutomata(){
    std::string retValue = "";
    retValue += AUTOMATA + COLON + SPACE + OPENING_BRAKET + NEW_LINE;
    retValue += OPENING_BRACE + NEW_LINE;
    retValue += TAB + NAME + COLON + SPACE + DOUBLE_QUOTE + bnNetwork.getNetworkName() + DOUBLE_QUOTE + COMMA + NEW_LINE;
    retValue += TAB + LOCATIONS + COLON + SPACE + OPENING_BRAKET;

    retValue += OPENING_BRACE + NAME + COLON + SPACE + DOUBLE_QUOTE + DEFAULT_LOC + "0" + DOUBLE_QUOTE + CLOSING_BRACE + COMMA;


    for(int i = 1; i <= networkSize; i ++){
        retValue += OPENING_BRACE + NAME + COLON + SPACE + DOUBLE_QUOTE + DEFAULT_LOC + std::to_string(i) + DOUBLE_QUOTE + CLOSING_BRACE + COMMA;
        int x = 1 + 1;
    }


    retValue.erase(retValue.end() - 1, retValue.end());
    retValue += CLOSING_BRAKET + COMMA + NEW_LINE;
    retValue += TAB + INITIAL_LOCATIONS + COLON + SPACE + OPENING_BRAKET + DOUBLE_QUOTE + DEFAULT_LOC + "0" + DOUBLE_QUOTE + CLOSING_BRAKET + COMMA + NEW_LINE;
    retValue += TAB + EDGES + COLON + SPACE + OPENING_BRAKET + NEW_LINE;

    int order = 0;
    for(ProbabilityTable probTable : bnNetwork.getSortedProbabilityTables()){
        for (ProbabilityRow row : probTable.getRows())
            retValue += createEdge(probTable.getNodeName(), probTable.getParentsNames(), row.getParentValues(),
                                       row.getProbabilities(), order);
        order ++;
        if(order >= maximumDepth){
            break;
        }
    }

    if(order > 0){
        retValue.erase(retValue.end() - 2, retValue.end() - 1);
    }
    retValue += TAB + CLOSING_BRAKET + NEW_LINE;
    retValue += CLOSING_BRACE + NEW_LINE;
    retValue += CLOSING_BRAKET + COMMA + NEW_LINE;
    return retValue;
}

std::string JaniFileCreator::createEdge(std::string tableName, std::vector<std::string> parentsName, std::vector<std::string> parentValues,
                                        std::vector<std::string> rowProbabilities, int order){
    std::string retValue = "";
    retValue += TAB + OPENING_BRACE + NEW_LINE;
    retValue += TAB + TAB + LOCATION + COLON + SPACE + DOUBLE_QUOTE + DEFAULT_LOC + std::to_string(order) + DOUBLE_QUOTE + COMMA + NEW_LINE;
    retValue += createGaurd(parentsName, parentValues);
    retValue += createDestinations(tableName, rowProbabilities);

    retValue += TAB + CLOSING_BRACE + COMMA + NEW_LINE;
    return retValue;
}

std::string JaniFileCreator::createGaurd(std::vector<std::string> parentNamesVector, std::vector<std::string> parentValuesVector){
    std::string retValue = "";

    if(!parentNamesVector.empty()) {
        retValue += TAB + TAB + GUARD + COLON + SPACE + OPENING_BRACE + NEW_LINE;
        retValue += TAB + TAB + EXP + COLON + SPACE + OPENING_BRACE + NEW_LINE;
        retValue += createCondition(parentNamesVector, parentValuesVector);
        retValue += TAB + TAB + CLOSING_BRACE + NEW_LINE;
        retValue += TAB + TAB + CLOSING_BRACE;
        retValue += COMMA + NEW_LINE;
    }
    return  retValue;
}

std::string JaniFileCreator::createCondition(std::vector<std::string> parentNamesVector, std::vector<std::string> parentValuesVector){
    std::string retValue = "";
     if(parentNamesVector.empty()){
        return retValue;
    }
    if(parentNamesVector.size() == 1) {
        retValue += TAB + TAB + TAB + TAB + OP + COLON + SPACE + EQUALS + COMMA + NEW_LINE;
        retValue +=
                TAB + TAB + TAB + TAB + LEFT + COLON + SPACE + DOUBLE_QUOTE + parentNamesVector.back() + DOUBLE_QUOTE +
                COMMA + NEW_LINE;
        Utils util;
        retValue += TAB + TAB + TAB + TAB + RIGHT + COLON + SPACE + std::to_string(
                util.getPos(bnNetwork.getNodeByName(parentNamesVector.back()).getVariableValueList(),
                                 parentValuesVector.back())) + NEW_LINE;
    }
    else{
        retValue += TAB + TAB + TAB + TAB + OP + COLON + SPACE + AND + COMMA + NEW_LINE;

        retValue += TAB + TAB + TAB + TAB + LEFT + COLON + SPACE + OPENING_BRACE + NEW_LINE;
        retValue += TAB + TAB + TAB + TAB + OP + COLON + SPACE + EQUALS + COMMA + NEW_LINE;
        retValue += TAB + TAB + TAB + TAB + LEFT + COLON + SPACE + DOUBLE_QUOTE + parentNamesVector.back() + DOUBLE_QUOTE + COMMA + NEW_LINE;
        Utils util;
        retValue += TAB + TAB + TAB + TAB + RIGHT + COLON + SPACE + std::to_string(util.getPos(bnNetwork.getNodeByName(parentNamesVector.back()).getVariableValueList(), parentValuesVector.back()))+ NEW_LINE;
        retValue += TAB + TAB + TAB + TAB + CLOSING_BRACE + COMMA + NEW_LINE;

        std::vector<std::string>::const_iterator namesItFirst = parentNamesVector.begin();
        std::vector<std::string>::const_iterator namesItLast = parentNamesVector.end() - 1;
        std::vector<std::string> parentNamesSubV(namesItFirst, namesItLast);

        std::vector<std::string>::const_iterator valuesItFirst = parentValuesVector.begin();
        std::vector<std::string>::const_iterator valuesItLast = parentValuesVector.end() - 1;
        std::vector<std::string> parentValuesSubV(valuesItFirst, valuesItLast);
        retValue += TAB + TAB + TAB + TAB + RIGHT + COLON + SPACE + OPENING_BRACE + NEW_LINE;
        retValue += createCondition(parentNamesSubV, parentValuesSubV);
        retValue += TAB + TAB + TAB + TAB + CLOSING_BRACE + NEW_LINE;

        return retValue;
    }
    return retValue;
}


std::string JaniFileCreator::createDestinations(std::string tableName, std::vector<std::string> rowProbabilities){
    std::string retValue = "";
    retValue += TAB + TAB + DESTINATIONS + COLON + SPACE + OPENING_BRAKET + NEW_LINE;
    int index = 0;
    for(std::string prob : rowProbabilities) {
        retValue += createProbabilityAssignment(tableName, prob, index);
        index ++;
    }
    if(index > 0){
        retValue.erase(retValue.end() - 2, retValue.end() - 1);
    }
    retValue += TAB + TAB + CLOSING_BRAKET + NEW_LINE;
    return retValue;
}

std::string JaniFileCreator::createProbabilityAssignment(std::string tableName, std::string prob, int index){
    std::string retValue = "";
    retValue += TAB + TAB + OPENING_BRACE + NEW_LINE;
    retValue += TAB + TAB + TAB + LOCATION + COLON + SPACE + DOUBLE_QUOTE + DEFAULT_LOC + std::to_string(bnNetwork.getTopologicalOrder(tableName)) + DOUBLE_QUOTE + COMMA + NEW_LINE;
    std::string probInJaniFormat = convertExpressionToJaniFormat(prob);
    retValue += TAB + TAB + TAB + PROBABILITY + COLON + SPACE + OPENING_BRACE + EXP + COLON + SPACE + probInJaniFormat + CLOSING_BRACE + COMMA + NEW_LINE;

    retValue += TAB + TAB + TAB + ASSIGNMENTS + COLON + SPACE + OPENING_BRAKET + NEW_LINE;
    retValue += TAB + TAB + TAB + OPENING_BRACE + NEW_LINE;
    retValue += TAB + TAB + TAB + TAB + REF + COLON + SPACE + DOUBLE_QUOTE + tableName + DOUBLE_QUOTE + COMMA + NEW_LINE;
    retValue += TAB + TAB + TAB + TAB + VALUE + COLON + SPACE + std::to_string(index) + NEW_LINE;
    retValue += TAB + TAB + TAB + CLOSING_BRACE + NEW_LINE;
    retValue += TAB + TAB + TAB + CLOSING_BRAKET + NEW_LINE;

    retValue += TAB + TAB + CLOSING_BRACE + COMMA + NEW_LINE;
    return retValue;
}

std::string JaniFileCreator::createSystem() {
    std::string retValue = "";
    retValue += SYSTEM + COLON + SPACE + OPENING_BRACE + NEW_LINE;
    retValue += TAB + ELEMENTS + COLON + SPACE + OPENING_BRAKET + SPACE + OPENING_BRACE
            + AUTOMATON + COLON + SPACE + DOUBLE_QUOTE + bnNetwork.getNetworkName() + DOUBLE_QUOTE + CLOSING_BRACE + SPACE + CLOSING_BRAKET + NEW_LINE;
    retValue += CLOSING_BRACE + NEW_LINE;
    return retValue;
}

int JaniFileCreator::determinMaxDepth(){
    if((bnNetwork.getEvidences()).empty() || (bnNetwork.getHypothesises()).empty()){
        return networkSize;

    }

    int evMaximumDepth = 0;
    for(Evidence ev : bnNetwork.getEvidences()){
        EvidenceInjector eInj = EvidenceInjector(bnNetwork, &ev);
        int index = eInj.determinIndexOfLastObservedNode();
        std::cout << index << std::endl;
        evMaximumDepth = std::max(evMaximumDepth, index);
    }

    int hypMaxDepth = 0;
    for(Hypothesis hyp : bnNetwork.getHypothesises()){
        HypothesisInjector hInj = HypothesisInjector(bnNetwork, &hyp);
        int index = hInj.determinIndexOfLastQuestionedNode();
        std::cout << index << std::endl;
        hypMaxDepth = std::max(hypMaxDepth, index);
    }
    std::cout << evMaximumDepth << " " << hypMaxDepth << " " << std::max(evMaximumDepth, hypMaxDepth) << std::endl;
    return std::max(evMaximumDepth, hypMaxDepth);
}



std::string JaniFileCreator::convertExpressionToJaniFormat(std::string expString){
    std::vector<storm::expressions::Expression> constraints;

     storm::parser::ExpressionParser expressionParser(*manager);
     expressionParser.setIdentifierMapping(variableMapping);
     storm::expressions::Expression expr = expressionParser.parseFromString(expString);
     auto json = storm::jani::ExpressionToJson::translate(expr, {}, {}, {}, bnNetwork.getParameterNames());
     return json.dump();
}
