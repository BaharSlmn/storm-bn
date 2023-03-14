//
// Created by Bahare Salmani on 2019-04-24.
//

#ifndef BNPARSER_UTILS_H
#define BNPARSER_UTILS_H

#include <string>
#include <regex>
#include <iostream>
#include <fstream>


class Utils {


public:
  void eraseSubStr(std::string &mainStr, const std::string &toErase);

  void eraseSubStr(std::string &mainStr, std::string &toErase);

  void eraseAllSubStr(std::string &mainStr, const std::string &toErase);

  std::string returnMatchedString(std::string mainStr, std::string searchStr);

  ptrdiff_t getPosition(std::vector<std::string> vector, std::string element);

  int getPos(std::vector<std::string> vector, std::string element);

  bool writeToFile(std::string fileContent, std::string filePath);

  std::string readFile(std::string filePath);

};


#endif //BNPARSER_UTILS_H
