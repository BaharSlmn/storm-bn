//
// Created by rdrah on 6/27/2020.
//

#ifndef QUERYPARSER_QUERIESCREATOR_H
#define QUERYPARSER_QUERIESCREATOR_H

#include <string>
#include <vector>
#include <numeric>
#include <algorithm>
#include <random>
#include "VariablesFileCreator.h"

class QueriesCreator {
public:
  /*!
   * Creates first, last, and random queries, based on the varFileContent.
   * @param varFileContent Content of .var file on which queries are based on.
   * @return string with the generated queries.
   */
  std::string createQueries(const std::string &networkName, const std::string &varFileContent);
  std::string createMARQuery();


private:

  /*
   * Adds name and value of variable to the query.
   * @param index Index of variable.
   */
  void addVariableAndValue(int index, int value, std::string& query);

  /*
   * Creates first query and last query on given number of variables.
   * Queries are appended to mFirstQueries and mLastQueries.
   */
  void createFirstLastQueriesOnNVariables(int numberVariables);

  /*
   * Creates a random query on given number of variables.
   * Query is then appended to mRandomQueries.
   */
  void createRandomQueryOnNVariables(int numberVariables);

  /*
   * Sets members of the class to their initial states.
   */
  void initialize();

  /*
   * Creates query on variables in the given vector. Values of variables
   * in the query are randomly generated.
   * @param variablesToQuery Vector of variable indexes.
   * @return query.
   */
  std::string makeQuery(const std::vector<int>& variablesToQuery);

  /*
   * Parses input file, and populates mNumberOfValuesDistributionVector and
   * mVariablesVector.
   * @param input File in Variable file format.
   */
  void parseInput(const std::string &input);

  /*
   * Parses line into variable name and number of values, and stores those
   * into mVariablesVector and mNumberOfValuesDistributionVector.
   *
   * @param line Line of file in Variable file format.
   */
  void parseLine(const std::string &line);

  /*
   * Creates first, last and random queries.
   * @return String containing first + last + random queries.
   */
  std::string createAllQueries();

private:
    std::string networkName;
  std::string mFirstQueries;
  std::string mLastQueries;
  std::string mRandomQueries;
  std::string MARQueries;
  std::vector<int> mCurrentVariablesToFirstQuery;
  std::vector<int> mCurrentVariablesToLastQuery;
  std::vector<int> mCurrentVariablesToRandomQuery;
  std::vector<std::uniform_int_distribution<>> mNumberOfValuesDistributionVector;
  std::vector<int> mNumberOfValuesVector;
  std::vector<std::string> mVariablesVector;
  std::random_device rd;
  /*
   * Queries contains number of variables corresponding to =
   * cMultiplicationStep * (number of variables in previous query)
   */
  static constexpr int cMultiplicationStep = 2;
};


namespace QueryFileConstants {
    const std:: string cQueryBegging = "\"P=?[G(";
    const std:: string cQueryEnding = ")]\"\n";
    const std:: string cLeftBracket = "(";
    const std:: string cRightBracket = ")";
    const std:: string cAnd = "&";
    const std:: string cOr = "|";
    const std:: string cEquals = "=";
    const std:: string cVariableDefaultValue = "-1";
};




#endif //QUERYPARSER_QUERIESCREATOR_H
