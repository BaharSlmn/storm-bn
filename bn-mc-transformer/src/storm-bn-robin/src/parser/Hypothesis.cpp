//
// Created by Bahare Salmani on 2019-10-11.
//

#include "Hypothesis.h"


Hypothesis::Hypothesis(std::string declaration) {
  hypothesisDeclaration = declaration;
  parseHypothesis();
}

std::map<std::string, std::string> Hypothesis::getEvaluations() const {
  return evaluations;
}

std::string Hypothesis::getValue(std::string key) {
  return evaluations[key];
}


void Hypothesis::parseHypothesis() {
  BIF bif;
  std::regex EV_REG(bif.assignment);
  std::regex_iterator<std::string::iterator> rit(hypothesisDeclaration.begin(), hypothesisDeclaration.end(), EV_REG);
  std::regex_iterator<std::string::iterator> rend;
  while (rit != rend) {
    parseAssignment(rit->str());
    ++rit;
  }
}

void Hypothesis::parseAssignment(std::string str) {
  BIF bif;
  Utils util;
  std::string match_string = util.returnMatchedString(str, bif.variableName + bif.SPACESTAR + bif.EQUALS);
  util.eraseSubStr(match_string, bif.EQUALS);
  util.eraseAllSubStr(match_string, bif.SPACE);
  std::string name = match_string;
  match_string = util.returnMatchedString(str, bif.EQUALS + bif.SPACESTAR + bif.variableValue);
  util.eraseSubStr(match_string, bif.EQUALS);
  util.eraseAllSubStr(match_string, bif.SPACE);
  util.eraseAllSubStr(match_string, ";");
  std::string value = match_string;
  evaluations[name] = value;
}

bool Hypothesis::isViolated(std::map<std::string, std::string> keyValues) {
  bool flag = false;
  for (std::pair<std::string, std::string> keyValue : keyValues) {
    flag = flag or isViolated(keyValue);
  }
  return flag;
}

bool Hypothesis::isViolated(std::pair<std::string, std::string> keyValue) {
  bool flag = false;
  std::map<std::string, std::string> evaluations = getEvaluations();
  if (evaluations.count(keyValue.first) > 0 && evaluations[keyValue.first] != keyValue.second)
    flag = true;
  return flag;
}