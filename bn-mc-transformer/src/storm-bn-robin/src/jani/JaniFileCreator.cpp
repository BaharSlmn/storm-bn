//
// Created by Bahare Salmani on 2019-05-06.
//

#include "JaniFileCreator.h"

#include <utility>


JaniFileCreator::JaniFileCreator(BNNetwork network) {
  bnNetwork = std::move(network);
  networkName = bnNetwork.getNetworkName();
  networkSize = bnNetwork.getNodes().size();
  //  maximumDepth = determinMaxDepth();

    /* configure expression parser with model parameters, in order to let models be parametric */
    std::vector<storm::expressions::Expression> constraints;
    manager = std::make_shared<storm::expressions::ExpressionManager>();
    for(std::string paramName : bnNetwork.getParameterNames()){
        auto param = manager->declareRationalVariable(paramName);
        variableMapping[paramName] = param;
    }
}


std::string JaniFileCreator::create() {
  std::string retValue;
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
  std::string retValue;
  retValue += TAB + JANI_VERSION + COLON + SPACE + version + COMMA + NEW_LINE;
  retValue += TAB + NAME + COLON + SPACE + DOUBLE_QUOTE + bnNetwork.getNetworkName() + ".jani" + DOUBLE_QUOTE + COMMA +
              NEW_LINE;
  retValue += TAB + TYPE + COLON + SPACE + DTMC + COMMA + NEW_LINE;
  retValue += TAB + FEATURES + COLON + SPACE + DERIVED_OPERATORS + COMMA + NEW_LINE;
  return retValue;
}

std::string JaniFileCreator::createConstantsBlock() {
  std::string retValue;
  if (bnNetwork.getParameterNames().empty())
    return retValue;
  retValue += CONSTANTS + COLON + SPACE + OPENING_BRAKET + NEW_LINE;
  for (const std::string& paramName : bnNetwork.getParameterNames()) {
    retValue += TAB + OPENING_BRACE + NEW_LINE;
    retValue += TAB + TAB + NAME + COLON + SPACE + DOUBLE_QUOTE + paramName + DOUBLE_QUOTE + COMMA + NEW_LINE;
    retValue += TAB + TAB + TYPE + COLON + SPACE + REAL + NEW_LINE;
    retValue += TAB + CLOSING_BRACE + COMMA + NEW_LINE;
  }

  if (!bnNetwork.getParameterNames().empty()) {
    retValue.erase(retValue.end() - 2, retValue.end() - 1);
  }
  retValue += CLOSING_BRAKET + COMMA + NEW_LINE;
  return retValue;
}

std::string JaniFileCreator::createVariablesBlock() {
  std::string retValue;
  retValue += VARIABLES + COLON + SPACE + OPENING_BRAKET + NEW_LINE;
  for (const BNNode& node: bnNetwork.getNodes()) {
    retValue += TAB + createVariableBlock(node.getNodeName(), node.getNumberOfValues());
    retValue += TAB + createTransientVariableBlock(node.getNodeName(), node.getNumberOfValues());
  }

  if (networkSize > 0) {
    retValue.erase(retValue.end() - 2, retValue.end() - 1);
  }
  retValue += CLOSING_BRAKET + COMMA + NEW_LINE;
  return retValue;
}

std::string JaniFileCreator::createVariableBlock(const std::string& nodeName, int numberOfValues) {
  std::string retValue;
  retValue += OPENING_BRACE + NEW_LINE;
  retValue += TAB + NAME + COLON + SPACE + DOUBLE_QUOTE + nodeName + DOUBLE_QUOTE + COMMA + NEW_LINE;
  retValue += TAB + TYPE + COLON + SPACE + OPENING_BRACE + NEW_LINE;
  retValue += TAB + TAB + BASE + COLON + SPACE + INT + COMMA + NEW_LINE;
  retValue += TAB + TAB + KIND + COLON + SPACE + BOUNDED + COMMA + NEW_LINE;
  retValue += TAB + TAB + LOWER_BOUND + COLON + SPACE + "-1" + COMMA + NEW_LINE;
  retValue += TAB + TAB + UPPER_BOUND + COLON + SPACE + std::to_string(numberOfValues - 1) + NEW_LINE;
  retValue += TAB + CLOSING_BRACE + COMMA + NEW_LINE;
  retValue += TAB + INITIAL_VALUE + COLON + SPACE + "-1" + NEW_LINE;
  retValue += TAB + CLOSING_BRACE + COMMA + NEW_LINE;
  return retValue;
}

std::string JaniFileCreator::createTransientVariableBlock(const std::string& nodeName, int numberOfValues){
    std::string retValue = "";
    for(int i=0; i < numberOfValues; i ++){
        retValue += TAB + OPENING_BRACE + NEW_LINE;
        retValue += TAB + TAB + INITIAL_VALUE + COLON + SPACE + "false" + COMMA + NEW_LINE;
        retValue += TAB + TAB + NAME + COLON + SPACE + DOUBLE_QUOTE + nodeName + "_" + std::to_string(i) + DOUBLE_QUOTE + COMMA + NEW_LINE;
        retValue += TAB + TAB + TRANSIENT + COLON + SPACE + "true" + COMMA + NEW_LINE;
        retValue += TAB + TAB + TYPE + COLON + SPACE + DOUBLE_QUOTE + "bool" + DOUBLE_QUOTE + NEW_LINE;
        retValue += TAB + CLOSING_BRACE + COMMA + NEW_LINE;
    }
    return retValue;
}


std::string JaniFileCreator::createProperties() {
  std::string retValue;
  retValue += PROPERTIES + COLON + SPACE + OPENING_BRAKET + NEW_LINE;
  /* to be impelemented Later */
  retValue += CLOSING_BRAKET + COMMA + NEW_LINE;
  return retValue;
}

std::string JaniFileCreator::createLocationsWithTransientValues() {
    std::string retValue = "";
    retValue += TAB + LOCATIONS + COLON + SPACE + OPENING_BRAKET + NEW_LINE;

    retValue += TAB + TAB + OPENING_BRACE + NEW_LINE;
    retValue += TAB + TAB + NAME + COLON + SPACE + DOUBLE_QUOTE + DEFAULT_LOC + std::to_string(0) + DOUBLE_QUOTE + NEW_LINE;
    retValue += TAB + TAB + CLOSING_BRACE + COMMA + NEW_LINE;

    const std::vector<ProbabilityTable> tables  = bnNetwork.getSortedProbabilityTables();
    for (int i = 1; i <= networkSize; i++) {

        retValue += TAB + TAB + OPENING_BRACE + NEW_LINE;
        retValue += TAB + TAB + NAME + COLON + SPACE + DOUBLE_QUOTE + DEFAULT_LOC + std::to_string(i) + DOUBLE_QUOTE + COMMA + NEW_LINE;

        /* defining transient values in each location */
        retValue += TAB + TAB + TRANSIENTVALUES + COLON + SPACE + OPENING_BRAKET + NEW_LINE;
        const ProbabilityTable table = tables.at(i - 1);
        int numberOfValues = table.getPossibleValues().size();
        std::string name = table.getNodeName();
        for(int j = 0; j < numberOfValues ; j ++){
            retValue += TAB + TAB + OPENING_BRACE + NEW_LINE;
            retValue += TAB + TAB + TAB + REF + COLON + SPACE + DOUBLE_QUOTE + name + "_" + std::to_string(j) + DOUBLE_QUOTE + COMMA + NEW_LINE;
            retValue += TAB + TAB + TAB + VALUE + COLON + SPACE + OPENING_BRACE + NEW_LINE;
            retValue += TAB + TAB + TAB + TAB + OP + COLON + SPACE + EQUALS + COMMA + NEW_LINE;
            retValue += TAB + TAB + TAB + TAB + LEFT + COLON + SPACE + DOUBLE_QUOTE + name + DOUBLE_QUOTE + COMMA + NEW_LINE;
            retValue += TAB + TAB + TAB + TAB + RIGHT + COLON + SPACE + std::to_string(j) + NEW_LINE;
            retValue += TAB + TAB + TAB + CLOSING_BRACE + NEW_LINE; /* end of value definition of the current transient variable */
            retValue += TAB + TAB + CLOSING_BRACE + COMMA + NEW_LINE; /* end of definition of the current transient value */
        }

        /* remove extra comma */
        if (numberOfValues > 0) {
            retValue.erase(retValue.end() - 2, retValue.end() - 1);
        }

        retValue += TAB + TAB + CLOSING_BRAKET + NEW_LINE; /* end of transient values */

        retValue += TAB + TAB + CLOSING_BRACE + COMMA + NEW_LINE; /* end of location definition */
    }

    /* remove extra comma */
    if (networkSize > 0) {
        retValue.erase(retValue.end() - 2, retValue.end() - 1);
    }
    retValue.erase(retValue.end() - 1, retValue.end());


    retValue += NEW_LINE + CLOSING_BRAKET + COMMA + NEW_LINE;

    /* setting the initial location*/
    retValue +=
            TAB + INITIAL_LOCATIONS + COLON + SPACE + OPENING_BRAKET + DOUBLE_QUOTE + DEFAULT_LOC + "0" + DOUBLE_QUOTE +
            CLOSING_BRAKET + COMMA + NEW_LINE;
    return retValue;
}

std::string JaniFileCreator::createAutomata() {
  std::string retValue;
  retValue += AUTOMATA + COLON + SPACE + OPENING_BRAKET + NEW_LINE;
  retValue += OPENING_BRACE + NEW_LINE;
  retValue += TAB + NAME + COLON + SPACE + DOUBLE_QUOTE + bnNetwork.getNetworkName() + DOUBLE_QUOTE + COMMA + NEW_LINE;

  retValue += createLocationsWithTransientValues();
  retValue += TAB + EDGES + COLON + SPACE + OPENING_BRAKET + NEW_LINE;

  int order = 0;
  for (const ProbabilityTable& probTable : bnNetwork.getSortedProbabilityTables()) {
    for (const ProbabilityRow& row : probTable.getRows())
      retValue += createEdge(probTable.getNodeName(), probTable.getParentsNames(), row.getParentValues(),
                             row.getProbabilities(), order);
    order++;
    /*
    if(order >= maximumDepth){
          break;
      }
    */
  }

  if (order > 0) {
    retValue.erase(retValue.end() - 2, retValue.end() - 1);
  }
  retValue += TAB + CLOSING_BRAKET + NEW_LINE;
  retValue += CLOSING_BRACE + NEW_LINE;
  retValue += CLOSING_BRAKET + COMMA + NEW_LINE;
  return retValue;
}

std::string JaniFileCreator::createEdge(std::string tableName, std::vector<std::string> parentsName,
                                        std::vector<std::string> parentValues,
                                        std::vector<std::string> rowProbabilities, int order) {
  std::string retValue;
  retValue += TAB + OPENING_BRACE + NEW_LINE;
  retValue += TAB + TAB + LOCATION + COLON + SPACE + DOUBLE_QUOTE + DEFAULT_LOC + std::to_string(order) + DOUBLE_QUOTE +
              COMMA + NEW_LINE;
  retValue += createGuard(std::move(parentsName), std::move(parentValues));
  retValue += createDestinations(std::move(tableName), std::move(rowProbabilities));

  retValue += TAB + CLOSING_BRACE + COMMA + NEW_LINE;
  return retValue;
}

std::string
JaniFileCreator::createGuard(const std::vector<std::string>& parentNamesVector, std::vector<std::string> parentValuesVector) {
  std::string retValue;

  if (!parentNamesVector.empty()) {
    retValue += TAB + TAB + GUARD + COLON + SPACE + OPENING_BRACE + NEW_LINE;
    retValue += TAB + TAB + EXP + COLON + SPACE + OPENING_BRACE + NEW_LINE;
    retValue += createCondition(parentNamesVector, std::move(parentValuesVector));
    retValue += TAB + TAB + CLOSING_BRACE + NEW_LINE;
    retValue += TAB + TAB + CLOSING_BRACE;
    retValue += COMMA + NEW_LINE;
  }
  return retValue;
}

std::string JaniFileCreator::createCondition(std::vector<std::string> parentNamesVector,
                                             std::vector<std::string> parentValuesVector) {
  std::string retValue;
  if (parentNamesVector.empty()) {
    return retValue;
  }
  if (parentNamesVector.size() == 1) {
    retValue += TAB + TAB + TAB + TAB + OP + COLON + SPACE + EQUALS + COMMA + NEW_LINE;
    retValue +=
            TAB + TAB + TAB + TAB + LEFT + COLON + SPACE + DOUBLE_QUOTE + parentNamesVector.back() + DOUBLE_QUOTE +
            COMMA + NEW_LINE;
    Utils util;
    retValue += TAB + TAB + TAB + TAB + RIGHT + COLON + SPACE + std::to_string(
            util.getPos(bnNetwork.getNodeByName(parentNamesVector.back()).getVariableValueList(),
                        parentValuesVector.back())) + NEW_LINE;
  } else {
    retValue += TAB + TAB + TAB + TAB + OP + COLON + SPACE + AND + COMMA + NEW_LINE;

    retValue += TAB + TAB + TAB + TAB + LEFT + COLON + SPACE + OPENING_BRACE + NEW_LINE;
    retValue += TAB + TAB + TAB + TAB + OP + COLON + SPACE + EQUALS + COMMA + NEW_LINE;
    retValue += TAB + TAB + TAB + TAB + LEFT + COLON + SPACE + DOUBLE_QUOTE + parentNamesVector.back() + DOUBLE_QUOTE +
                COMMA + NEW_LINE;
    Utils util;
    retValue += TAB + TAB + TAB + TAB + RIGHT + COLON + SPACE + std::to_string(
            util.getPos(bnNetwork.getNodeByName(parentNamesVector.back()).getVariableValueList(),
                        parentValuesVector.back())) + NEW_LINE;
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


std::string JaniFileCreator::createDestinations(const std::string& tableName, const std::vector<std::string>& rowProbabilities) {
  std::string retValue;
  retValue += TAB + TAB + DESTINATIONS + COLON + SPACE + OPENING_BRAKET + NEW_LINE;
  int index = 0;
  for (const std::string& prob : rowProbabilities) {
    retValue += createProbabilityAssignment(tableName, prob, index);
    index++;
  }
  if (index > 0) {
    retValue.erase(retValue.end() - 2, retValue.end() - 1);
  }
  retValue += TAB + TAB + CLOSING_BRAKET + NEW_LINE;
  return retValue;
}

std::string JaniFileCreator::createProbabilityAssignment(const std::string& tableName, std::string prob, int index) {
  std::string retValue;
  retValue += TAB + TAB + OPENING_BRACE + NEW_LINE;
  int position = bnNetwork.getTopologicalOrder(tableName);

  auto search = bnNetwork.janiData.positionToEvidenceValue.find(position - 1);
  if (search != bnNetwork.janiData.positionToEvidenceValue.end() && index != search->second) {
    retValue += TAB + TAB + TAB + LOCATION + COLON + SPACE + DOUBLE_QUOTE + DEFAULT_LOC + std::to_string(0) +
                DOUBLE_QUOTE + COMMA + NEW_LINE;
  } else {
    retValue += TAB + TAB + TAB + LOCATION + COLON + SPACE + DOUBLE_QUOTE + DEFAULT_LOC + std::to_string(position) +
                      DOUBLE_QUOTE + COMMA + NEW_LINE;
  }

  // in nodes to remove pos is 0 indexed
  --position;
  std::string probInJaniFormat = convertExpressionToJaniFormat(std::move(prob));
  retValue += TAB + TAB + TAB + PROBABILITY + COLON + SPACE + OPENING_BRACE + EXP + COLON + SPACE + probInJaniFormat +
              CLOSING_BRACE + COMMA + NEW_LINE;
  retValue += TAB + TAB + TAB + ASSIGNMENTS + COLON + SPACE + OPENING_BRAKET + NEW_LINE;

  if (search != bnNetwork.janiData.positionToEvidenceValue.end() && index != search->second) {
    auto searchKnown = bnNetwork.janiData.positionToKnownNodesMap.find(position);
    if (searchKnown != bnNetwork.janiData.positionToNodesToResetMap.end()) {
      retValue += TAB + TAB + TAB + OPENING_BRACE + NEW_LINE;
      retValue += TAB + TAB + TAB + TAB + REF + COLON + SPACE + DOUBLE_QUOTE + tableName + DOUBLE_QUOTE + COMMA + NEW_LINE;
      retValue += TAB + TAB + TAB + TAB + VALUE + COLON + SPACE + std::to_string(-1) + NEW_LINE;
      retValue += TAB + TAB + TAB + CLOSING_BRACE;
      for (auto &&name : searchKnown->second) {
        retValue += COMMA + NEW_LINE;
        retValue += TAB + TAB + TAB + OPENING_BRACE + NEW_LINE;
        retValue += TAB + TAB + TAB + TAB + REF + COLON + SPACE + DOUBLE_QUOTE + name + DOUBLE_QUOTE + COMMA + NEW_LINE;
        retValue += TAB + TAB + TAB + TAB + VALUE + COLON + SPACE + std::to_string(-1) + NEW_LINE;
        retValue += TAB + TAB + TAB + CLOSING_BRACE;
      }
      retValue += NEW_LINE;
    }
  } else {
    retValue += TAB + TAB + TAB + OPENING_BRACE + NEW_LINE;
    retValue += TAB + TAB + TAB + TAB + REF + COLON + SPACE + DOUBLE_QUOTE + tableName + DOUBLE_QUOTE + COMMA + NEW_LINE;
    retValue += TAB + TAB + TAB + TAB + VALUE + COLON + SPACE + std::to_string(index) + NEW_LINE;
    retValue += TAB + TAB + TAB + CLOSING_BRACE;
    auto searchReset = bnNetwork.janiData.positionToNodesToResetMap.find(position);
    if (searchReset == bnNetwork.janiData.positionToNodesToResetMap.end()) {
      retValue += NEW_LINE;
    } else {
      for (auto &&name : searchReset->second) {
        retValue += COMMA + NEW_LINE;
        retValue += TAB + TAB + TAB + OPENING_BRACE + NEW_LINE;
        retValue += TAB + TAB + TAB + TAB + REF + COLON + SPACE + DOUBLE_QUOTE + name + DOUBLE_QUOTE + COMMA + NEW_LINE;
        retValue += TAB + TAB + TAB + TAB + VALUE + COLON + SPACE + std::to_string(-1) + NEW_LINE;
        retValue += TAB + TAB + TAB + CLOSING_BRACE;
      }
      retValue += NEW_LINE;
    }
  }

  retValue += TAB + TAB + TAB + CLOSING_BRAKET + NEW_LINE;

  retValue += TAB + TAB + CLOSING_BRACE + COMMA + NEW_LINE;
  return retValue;
}

std::string JaniFileCreator::createSystem() {
  std::string retValue;
  retValue += SYSTEM + COLON + SPACE + OPENING_BRACE + NEW_LINE;
  retValue += TAB + ELEMENTS + COLON + SPACE + OPENING_BRAKET + SPACE + OPENING_BRACE
              + AUTOMATON + COLON + SPACE + DOUBLE_QUOTE + bnNetwork.getNetworkName() + DOUBLE_QUOTE + CLOSING_BRACE +
              SPACE + CLOSING_BRAKET + NEW_LINE;
  retValue += CLOSING_BRACE + NEW_LINE;
  return retValue;
}

std::string JaniFileCreator::convertExpressionToJaniFormat(std::string expString) {
    std::vector<storm::expressions::Expression> constraints;

    storm::parser::ExpressionParser expressionParser(*manager);
    expressionParser.setIdentifierMapping(variableMapping);
    storm::expressions::Expression expr = expressionParser.parseFromString(expString);
    auto json = storm::jani::ExpressionToJson::translate(expr, {}, {}, {}, bnNetwork.getParameterNames());
    return json.dump();
}
