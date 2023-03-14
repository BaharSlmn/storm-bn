//
// Created by rdrah on 6/27/2020.
//

#include "QueriesCreator.h"

std::string QueriesCreator::createQueries(const std::string &nName, const std::string &varFileContent) {
    networkName = nName;
  initialize();
  parseInput(varFileContent);
  return createAllQueries();
}

void QueriesCreator::initialize() {
  mNumberOfValuesDistributionVector.clear();
  mVariablesVector.clear();
  mFirstQueries.clear();
  mLastQueries.clear();
  mRandomQueries.clear();
  mCurrentVariablesToFirstQuery.clear();
  mCurrentVariablesToLastQuery.clear();
  mCurrentVariablesToRandomQuery.clear();
}

void QueriesCreator::parseInput(const std::string &input) {
  size_t last = 0;
  size_t next;

  while ((next = input.find(VariableFileConstants::cEndOfLine, last)) != std::string::npos) {
    parseLine(input.substr(last, next - last));
    last = next + VariableFileConstants::cEndOfLine.size();
  }
  parseLine(input.substr(last));
}

void QueriesCreator::parseLine(const std::string &line) {
  auto delimiterLocation = line.find(VariableFileConstants::cColumnDelimiter, 0);
  mVariablesVector.push_back(line.substr(0, delimiterLocation));
  int numOfValues = std::stoi(line.substr(delimiterLocation + VariableFileConstants::cColumnDelimiter.size()),
                              nullptr, 10);
  mNumberOfValuesVector.push_back(numOfValues);
  std::uniform_int_distribution<> distrib(0, numOfValues - 1);
  mNumberOfValuesDistributionVector.push_back(distrib);
}

std::string QueriesCreator::createAllQueries() {
  for (size_t numVariables = 1; numVariables < mVariablesVector.size(); numVariables*=cMultiplicationStep) {
    createRandomQueryOnNVariables(numVariables);
    createFirstLastQueriesOnNVariables(numVariables);
  }
  createRandomQueryOnNVariables(mVariablesVector.size());
  createFirstLastQueriesOnNVariables(mVariablesVector.size());
  return mFirstQueries + mLastQueries + mRandomQueries;
}

void QueriesCreator::createFirstLastQueriesOnNVariables(int numberVariables) {
  for (auto i = mCurrentVariablesToFirstQuery.size(); i < numberVariables; ++i) {
    mCurrentVariablesToFirstQuery.push_back(i);
    mCurrentVariablesToLastQuery.push_back(mVariablesVector.size() - 1 - i);
  }
  mFirstQueries += makeQuery(mCurrentVariablesToFirstQuery);
  mLastQueries += makeQuery(mCurrentVariablesToLastQuery);
}

void QueriesCreator::createRandomQueryOnNVariables(int numberVariables) {
  mCurrentVariablesToRandomQuery.clear();
  std::vector<size_t> indices(mVariablesVector.size());
  std::iota(indices.begin(), indices.end(), 0);
  std::shuffle(indices.begin(), indices.end(), std::mt19937(std::random_device()()));
  for (auto it = std::begin(indices); it != std::begin(indices) + numberVariables && it != std::end(indices); ++it) {
    mCurrentVariablesToRandomQuery.push_back(*it);
  }
  mRandomQueries += makeQuery(mCurrentVariablesToRandomQuery);
}

std::string QueriesCreator::makeQuery(const std::vector<int>& variablesToQuery) {
  std::string query;
  query += QueryFileConstants::cQueryBegging;
  std::mt19937 generator(rd());
  for (auto const indexVariable : variablesToQuery) {
    addVariableAndValue(indexVariable, mNumberOfValuesDistributionVector[indexVariable](generator), query);
  }
  query.pop_back();
  query += QueryFileConstants::cQueryEnding;
  return query;
}

void QueriesCreator::addVariableAndValue(const int indexVariable, const int value, std::string& query) {
  query += QueryFileConstants::cLeftBracket;
  query += mVariablesVector[indexVariable];
  query += QueryFileConstants::cEquals;
  query += std::to_string(value);
  query += QueryFileConstants::cOr;
  query += mVariablesVector[indexVariable];
  query += QueryFileConstants::cEquals;
  query += QueryFileConstants::cVariableDefaultValue;
  query += QueryFileConstants::cRightBracket;
  query += QueryFileConstants::cAnd;
}

std::string QueriesCreator::createMARQuery() {
    /* std::string head = "time storm --jani " + networkName + ".jani --prop \"";
    std::string result = head;
    for(std::string var : mVariablesVector)
        result += "P=? [G(" + var + "=" + "0" + "|" + var + "=" + "-1"  ")];";

    result += "\"\n"; */

    std::string head = "time storm -drn " + networkName + ".drn --prop \"";
    std::string head2 = "time storm --explicit " + networkName + ".tra" + " " + networkName + ".lab --prop \"";
    int index = 0;
    std::string prop = "";
    for(std::string var : mVariablesVector){
        int num = mNumberOfValuesVector.at(index);
        for(int i = 0; i < num; i++){
            prop += "P=? [F(" "\\\"" + var + std::to_string(i) + "\\\"" + ")];";
        }
        index++;
    }
    prop += "\"\n";
    return head + prop + head2 + prop;
}
