//
// Created by rdrah on 27.06.20.
//

#ifndef TRANSFORMATIONTEST_VARIABLESFILECREATOR_H
#define TRANSFORMATIONTEST_VARIABLESFILECREATOR_H

#include <string>
#include "parser/ProbabilityTable.h"


class VariablesFileCreator {
public:
  /*!
   * Creates content of the variable file. Variable file has format:
   * name-2
   * otherName-3
   * Where name and otherName are names of nodes of the network ordered in topological order
   * and 2,3 are number of possible values of the nodes.
   * @param sortedProbabilityTables Sorted probability tables of BNN
   * @return content of the variable file as string
   */
  static std::string createVariableFileContent(const std::vector<ProbabilityTable> &sortedProbabilityTables);
};




namespace VariableFileConstants {
    const std::string cEndOfLine = "\n";
    const std:: string cColumnDelimiter = "-";
};



#endif //TRANSFORMATIONTEST_VARIABLESFILECREATOR_H
