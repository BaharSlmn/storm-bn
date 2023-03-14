//
// Created by Bahare Salmani on 2019-04-09.
//

#include "BNNode.h"
#include <iostream>

BNNode::BNNode(std::string declaration) {
  nodeDeclaration = declaration;
  observed = false;
  questioned = false;
  setNodeName();
  setNumberOfValues();
  setVariableValuesList();
}

BNNode::BNNode(std::string name, int domain, std::vector<std::string> possibleValues, bool ob, bool quest) {
  nodeName = name;
  numberOfValues = domain;
  variableValuesList = possibleValues;
  observed = ob;
  questioned = quest;
}

void BNNode::setNodeName() {
  BIF bif;
  Utils util;
  std::string match_string = util.returnMatchedString(nodeDeclaration, bif.VARIABLE + bif.SPACEPLUS + bif.variableName);

  util.eraseSubStr(match_string, bif.VARIABLE);
  util.eraseAllSubStr(match_string, bif.SPACE);
  nodeName = match_string;
}

void BNNode::setNumberOfValues() {
  BIF bif;
  Utils util;
  std::string match_string = util.returnMatchedString(nodeDeclaration, bif.dimensionDefinition);

  util.eraseSubStr(match_string, "[");
  util.eraseSubStr(match_string, "]");
  util.eraseAllSubStr(match_string, bif.SPACE);
  numberOfValues = std::stoi(match_string);
}

void BNNode::setVariableValuesList() {
  BIF bif;
  Utils util;
  std::string match_string = util.returnMatchedString(nodeDeclaration,
                                                      "[{]" + bif.variableValuesList + "[}]" + bif.SPACESTAR + "[;]");
  util.eraseSubStr(match_string, ";");
  std::regex VAL_REG("(" + bif.LETTER + "|" + bif.DIGIT + "|" + bif.MARKS + ")" + "+");
  std::regex_iterator<std::string::iterator> rit(match_string.begin(), match_string.end(), VAL_REG);
  std::regex_iterator<std::string::iterator> rend;
  while (rit != rend) {
    std::string value = rit->str();
    util.eraseSubStr(value, ",");
    util.eraseSubStr(value, " ");
    variableValuesList.push_back(value);
    ++rit;
  }
}

void BNNode::markAsObserved() {
  observed = true;
}

void BNNode::markAsQuestioned() {
  questioned = true;
}


std::vector<std::string> BNNode::getVariableValueList() const {
  return variableValuesList;
}

int BNNode::getNumberOfValues() const {
  return numberOfValues;
}

std::string BNNode::getNodeName() const {
  return nodeName;
}

bool BNNode::isObserved() const {
  return observed;
}

bool BNNode::isQuestioned() const {
  return questioned;
}

int BNNode::getIndexOfAParticularValue(std::string value) const {
  ptrdiff_t pos = find(variableValuesList.begin(), variableValuesList.end(), value) - variableValuesList.begin();
  if (pos >= variableValuesList.size()) {
    std::cout << "variableNotFoundException";
  }
  return pos;
}
