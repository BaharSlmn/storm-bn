//
// Created by rdrah on 14.08.20.
//

#include "TopologicalOrderingReader.h"

std::vector<Graph::NodeIndex> TopologicalOrderingReader::getTopologicalOrdering(const std::string &input, const Graph& graph) {
  std::vector<Graph::NodeIndex> ordering;
  size_t last = 0;
  size_t next;

  while ((next = input.find(VariableFileConstants::cEndOfLine, last)) != std::string::npos) {
    ordering.push_back(parseLine(input.substr(last, next - last),graph));
    last = next + VariableFileConstants::cEndOfLine.size();
  }
  //ordering.push_back(parseLine(input.substr(last),graph));
  return ordering;
}

Graph::NodeIndex TopologicalOrderingReader::parseLine(const std::string &line, const Graph& graph) {
  return graph.getNodeIndex(line.substr(0, line.find(VariableFileConstants::cColumnDelimiter, 0)));
}
