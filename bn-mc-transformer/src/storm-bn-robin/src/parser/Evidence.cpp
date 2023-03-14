//
// Created by Bahare Salmani on 2019-08-26.
//

#include "Evidence.h"

#include <utility>


Evidence::Evidence(std::string declaration) {
  evidenceDeclaration = std::move(declaration);
  parseEvidence();
}

std::map<std::string, std::string> Evidence::getObservations() const {
  return observations;
}

std::string Evidence::getValue(const std::string& key) {
  return observations[key];
}


void Evidence::parseEvidence() {
  BIF bif;
  std::regex EV_REG(bif.assignment);
  std::regex_iterator<std::string::iterator> rit(evidenceDeclaration.begin(), evidenceDeclaration.end(), EV_REG);
  std::regex_iterator<std::string::iterator> rend;
  while (rit != rend) {
    parseAssignment(rit->str());
    ++rit;
  }
}

void Evidence::parseAssignment(const std::string& str) {
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
  observations[name] = value;
}

bool Evidence::isViolated(const std::map<std::string, std::string>& keyValues) {
  bool flag = false;
  for (std::pair<std::string, std::string> keyValue : keyValues) {
    flag = flag or isViolated(keyValue);
  }
  return flag;
}

bool Evidence::isViolated(const std::pair<std::string, std::string>& keyValue) {
  bool flag = false;
  std::map<std::string, std::string> observations = getObservations();
  if (observations.count(keyValue.first) > 0 && observations[keyValue.first] != keyValue.second)
    flag = true;
  return flag;

}