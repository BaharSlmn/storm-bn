//
// Created by Bahare Salmani on 2020-02-12.
//

#ifndef STORM_PARAMETER_H
#define STORM_PARAMETER_H

#include <map>
#include <string>
#include "BIFFORMAT.h"
#include "Utils.h"


class Parameter {

private:
  std::string parameterDeclaration;
  std::string parameterName;

public:
  Parameter(std::string declaration);

  std::string getName();

  void parseParameter();

};


#endif //STORM_PARAMETER_H
