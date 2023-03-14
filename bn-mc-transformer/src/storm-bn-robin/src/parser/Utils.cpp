//
// Created by Bahare Salmani on 2019-04-24.
//

#include "Utils.h"


void Utils::eraseSubStr(std::string &mainStr, const std::string &toErase) {
  // Search for the substring in string
  size_t pos = mainStr.find(toErase);

  if (pos != std::string::npos) {
    // If found then erase it from string
    mainStr.erase(pos, toErase.length());
  }
}

void Utils::eraseSubStr(std::string &mainStr, std::string &toErase) {
  std::string::size_type i = mainStr.find(toErase);

  if (i != std::string::npos)
    mainStr.erase(i, toErase.length());
}


void Utils::eraseAllSubStr(std::string &mainStr, const std::string &toErase) {
  size_t pos;
  // Search for the substring in string in a loop until nothing is found
  while ((pos = mainStr.find(toErase)) != std::string::npos) {
    // If found then erase it from string
    mainStr.erase(pos, toErase.length());
  }
}


std::string Utils::returnMatchedString(std::string mainStr, std::string searchStr) {
  std::regex rgx(searchStr);
  std::smatch match;
  std::string match_string;
  if (std::regex_search(mainStr, match, rgx)) {
    match_string = match[0];
  }
  return match_string;
}

ptrdiff_t Utils::getPosition(std::vector<std::string> vector, std::string element) {
  for (int i = 0; i < vector.size(); i++) {
    std::cout << " " << vector.at(i) << " ";
  }
  ptrdiff_t pos = find(vector.begin(), vector.end(), element) - vector.begin();
  if (pos > element.size()) {
    std::cout << "not found" << std::endl;

  }
  return pos;
}

int Utils::getPos(std::vector<std::string> vector, std::string element) {
  int index;
  std::vector<std::string>::iterator it = std::find(vector.begin(), vector.end(), element);
  if (it != vector.end());
    /* std::cout << "Element Found" << std::endl; */
  else
    std::cout << "Element Not Found" << std::endl;
  index = std::distance(vector.begin(), it);
  return index;
}

bool Utils::writeToFile(std::string fileContent, std::string filePath) {
  std::ofstream resutlFile(filePath);
  if (resutlFile.is_open()) {
    resutlFile << fileContent;
    resutlFile.close();
    return true;
  } else return false;
}

std::string Utils::readFile(std::string filePath) {
  std::string fileContent = "";
  std::string line;
  std::ifstream myfile;
  myfile.open(filePath);
  if (myfile.is_open()) {
    fileContent.assign((std::istreambuf_iterator<char>(myfile)),
                       (std::istreambuf_iterator<char>()));
    myfile.close();
  } else std::cout << "Unable to open file";
  return fileContent;
}