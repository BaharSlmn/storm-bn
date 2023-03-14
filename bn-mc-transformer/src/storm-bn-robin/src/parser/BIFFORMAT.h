//
// Created by Bahare Salmani on 2019-04-15.
/* Based on Formal Definition of BIF Format found on http://www.cs.washington.edu/dm/vfml/appendixes/bif.htm */
//

#ifndef BNPARSER_BIFFORMAT_H
#define BNPARSER_BIFFORMAT_H

#include <string>

struct BIF {
  std::string SPACE = " ";
  std::string TAB = "\t";
  std::string NEW_LINE = "\n";
  std::string RETURN = "\r";
  std::string comma = ",";
  std::string OR = "\\|";

  std::string SPACEPLUS = "[ \\|,\t\n\r]+";
  std::string SPACESTAR = "[ \\|,\t\n\r]*";

  std::string OPEN_BRAKET = "\\[";
  std::string CLOSE_BRAKET = "\\]";
  std::string OPEN_PARAN = "[\\(]";
  std::string CLOSE_PARAN = "[\\)]";

  std::string LETTER = "[a-z,A-Z,_,-,/,.]";
  std::string DIGIT = "[0-9]";
  std::string WORD = "(" + LETTER + "|" + DIGIT + ")" + "*";


  std::string DECIMAL_LITERAL = "[1-9]([0-9])*";
  std::string EXPONENT = "[eE]([+-])?([0-9])+";
  std::string FLOATING_POINT_LITERAL = "([0-9])+[.]([0-9])+([eE][-+]?[0-9]+)?";


  std::string NETWORK = "network";
  std::string VARIABLE = "variable";
  std::string PROBABILITY = "probability";
  std::string PROPERTY = "property";
  std::string VARIABLETYPE = "type";
  std::string DISCRETE = "discrete";
  std::string DEFAULTVALUE = "default";
  std::string TABLE = "table";
  std::string EVIDENCE = "evidence";
  std::string HYPOTHESIS = "hypothesis";
  std::string PARAMETER = "parameter";
  std::string EQUALS = "=";


  /* Property String: to be defined later, given more thought */
  std::string property = PROPERTY + SPACEPLUS + WORD /*name*/ + SPACESTAR + "[=]" + SPACESTAR + WORD /*value*/;


  std::string variableName = WORD;
  std::string expression =
          "(" + LETTER + "|" + DIGIT + "|" + "[(]" + ")" + "+" +
          "(" + LETTER + "|" + DIGIT + "|" + "[+]" + "|" + "[-]" + "|" + "[)]" + "|" + "[(]" + "|" + "[/]" + "|" + "[*]" + ")" + "*";
  std::string MARKS = "[+-=<>]";
  std::string variableValue = "(" + LETTER + "|" + DIGIT + "|" + MARKS + ")" + "*";
  std::string variableValuesList = variableValue + SPACESTAR + "(" + SPACEPLUS + variableValue + ")" + "*";
  std::string dimensionDefinition = OPEN_BRAKET + SPACESTAR + DECIMAL_LITERAL + SPACESTAR + CLOSE_BRAKET;
  std::string variableDiscrete = VARIABLETYPE + SPACEPLUS + DISCRETE + SPACESTAR + dimensionDefinition + SPACESTAR +
                                 +"[{]" + variableValuesList + "[}]" + "[;]";
  std::string variableDeclaration =
          VARIABLE + SPACEPLUS + variableName + SPACESTAR + "[{]" + SPACESTAR + variableDiscrete + SPACESTAR + "[}]" +
          SPACESTAR;

  std::string Seperator = "[ ,\t\n\r]*";
  std::string floatingPointList =
          FLOATING_POINT_LITERAL + "(" + Seperator + FLOATING_POINT_LITERAL + ")" + "*" + SPACESTAR + "[;]";
  std::string expressionList = expression + "(" + Seperator + expression + ")" + "*" + SPACESTAR + "[;]";
  std::string probabilityVariableName = variableValue;
  std::string probabilityVariableList =
          OPEN_PARAN + SPACESTAR + probabilityVariableName + "(" + SPACESTAR + probabilityVariableName + ")" + "*" +
          SPACESTAR + CLOSE_PARAN;
  std::string probabilityEntry =
          SPACESTAR + probabilityVariableList + SPACEPLUS + "(" + floatingPointList + "|" + expressionList + ")" +
          SPACESTAR;
  std::string probabilityDefualtEntry = DEFAULTVALUE + SPACEPLUS + "(" + floatingPointList + "|" + expressionList + ")";
  std::string probabilityTable = TABLE + SPACEPLUS + "(" + floatingPointList + "|" + expressionList + ")";
  std::string probabilityContent =
          "(" + SPACESTAR + property + "|" + probabilityDefualtEntry + "|" + probabilityTable + "|" + probabilityEntry +
          SPACESTAR + ")" + "*";
  std::string probabilityDeclaration =
          SPACESTAR + PROBABILITY + SPACEPLUS + probabilityVariableList + SPACESTAR + "[{]" + SPACESTAR +
          probabilityContent + SPACESTAR + "[}]" + SPACESTAR;

  std::string networkContent = "[{]" + SPACESTAR + "(" + property + ")" + "*" + SPACESTAR + "[}]";
  std::string networkDeclaration = NETWORK + SPACEPLUS + WORD + SPACESTAR + networkContent;

  std::string assignment =
          SPACESTAR + variableName + SPACESTAR + EQUALS + SPACESTAR + variableValue + SPACESTAR + "[;]" + SPACESTAR;
  std::string evidenceName = WORD;
  std::string evidenceDeclaration = EVIDENCE + SPACEPLUS + evidenceName +
                                    SPACESTAR + "[{]" + SPACESTAR + "(" + SPACESTAR + assignment + ")" + "*" + "[}]";

  std::string hypothesisName = WORD;
  std::string hypothesisDeclaration = HYPOTHESIS + SPACEPLUS + hypothesisName +
                                      SPACESTAR + "[{]" + SPACESTAR + "(" + SPACESTAR + assignment + ")" + "*" + "[}]";

  std::string parameterName = WORD;
  std::string parametersDeclaration = PARAMETER + SPACEPLUS + parameterName +
                                      SPACESTAR + "[{]" + SPACESTAR + "[}]";
  std::string compilationUnit =
          networkDeclaration + SPACESTAR + "(" + variableDeclaration + SPACESTAR + "? |" + probabilityDeclaration +
          SPACESTAR + "? |" + hypothesisDeclaration + SPACESTAR + "? |" + evidenceDeclaration + SPACESTAR + ")" + "*" +
          SPACESTAR;

};

#endif //BNPARSER_BIFFORMAT_H
