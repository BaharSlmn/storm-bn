//
// Created by Bahare Salmani on 2019-05-06.
//

#ifndef BNPARSER_JANIFILECREATOR_H
#define BNPARSER_JANIFILECREATOR_H

#include "../parser/BNNetwork.h"
#include "../parser/BNNode.h"
#include "literals.h"
//#include "storm-bn/transformer/EvidenceInjector.h"
//#include "storm-bn/transformer/HypothesisInjector.h"

#include <iostream>

#include "storm-config.h"
#include "storm/storage/jani/visitor/JSONExporter.h"
#include "storm/storage/expressions/Expressions.h"
#include "storm/storage/expressions/ExpressionManager.h"
#include "storm-parsers/parser/ExpressionParser.h"



class JaniFileCreator {
public:
  explicit JaniFileCreator(BNNetwork network);

  std::string create();


private:
  BNNetwork bnNetwork;
  std::string networkName;
  int networkSize; /* number of nodes, which equals to number of Probability Tables, in the bayesian network */
  //int maximumDepth; /* maximum depth of the evidence and hypothesis nodes, which is needed to create the Markov Chain based on */

  std::unordered_map<std::string, storm::expressions::Expression> variableMapping;
  std::shared_ptr<storm::expressions::ExpressionManager> manager;

  std::string createHeaderBlock();

  std::string createVariablesBlock();

  std::string createConstantsBlock();

  static std::string createProperties();

  static std::string createVariableBlock(const std::string& nodeName, int numberOfValues);

  static std::string createTransientVariableBlock(const std::string& nodeName, int numberOfValues);

  std::string createAutomata();

  std::string
  createEdge(std::string tableName, std::vector<std::string> parentsName, std::vector<std::string> parentValues,
             std::vector<std::string> rowProbabilities, int order);

  std::string createGuard(const std::vector<std::string>& parentNamesVector, std::vector<std::string> parentValuesVector);

  std::string
  createCondition(std::vector<std::string> parentNamesSubVector, std::vector<std::string> parentValueSubVector);

  std::string createDestinations(const std::string& tableName, const std::vector<std::string>& rowProbabilities);

  std::string createProbabilityAssignment(const std::string& tableName, std::string prob, int index);

  std::string createLocationsWithTransientValues();

  std::string createSystem();

  std::string convertExpressionToJaniFormat(std::string exp);
  // int determinateMaxDepth();

};


#endif //BNPARSER_JANIFILECREATOR_H
