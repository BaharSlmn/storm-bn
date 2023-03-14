//
// Created by rdrah on 27.06.20.
//

#include "VariablesFileCreator.h"

std::string
VariablesFileCreator::createVariableFileContent(const std::vector<ProbabilityTable> &sortedProbabilityTables) {
  std::string result;
  for (const auto &probabilityTable : sortedProbabilityTables) {
    result.append(probabilityTable.getNodeName());
    result.append(VariableFileConstants::cColumnDelimiter);
    result.append(std::to_string(probabilityTable.getPossibleValues().size()));
    result.append(VariableFileConstants::cEndOfLine);
  }
  result.pop_back();
  return result;
}
