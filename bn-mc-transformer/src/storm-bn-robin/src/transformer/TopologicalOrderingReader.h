//
// Created by rdrah on 14.08.20.
//

#ifndef TRANSFORMATIONTEST_TOPOLOGICALORDERINGREADER_H
#define TRANSFORMATIONTEST_TOPOLOGICALORDERINGREADER_H

#include <string>
#include <stack>
#include <vector>
#include "storm-bn-robin/src/VariablesFileCreator.h"
#include "storm-bn-robin/src/transformer/Graph.h"

class TopologicalOrderingReader {
public:
  /*!
   * Gets topological ordering of a graph for var file content.
   * @return Vector of indexes ordered in topological order.
   */
  static std::vector<Graph::NodeIndex> getTopologicalOrdering(std::string const &varFileContent, const Graph& graph);

private:

  static Graph::NodeIndex parseLine(const std::string &line, const Graph& graph);
};

#endif //TRANSFORMATIONTEST_TOPOLOGICALORDERINGREADER_H
