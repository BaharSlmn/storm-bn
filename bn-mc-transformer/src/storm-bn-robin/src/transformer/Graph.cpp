//
// Created by rdrah on 5/25/2020.
//

#include "Graph.h"

Graph::NodeIndex Graph::addNode(std::string const &name, NodeCount const numberOfValues) {
  mNodesVector.emplace_back(mNumberOfVertices, name, numberOfValues);
  return mNumberOfVertices++;
}

void Graph::addEdge(NodeIndex const indexChild, NodeIndex const indexParent) {
  if (indexParent < 0 || indexChild < 0 || indexParent >= mNumberOfVertices || indexChild >= mNumberOfVertices) {
    return;
  }
  mNodesVector[indexParent].addChild(indexChild);
  mNodesVector[indexChild].addParent(indexParent);
}

Graph::NodeIndex Graph::getNodeIndex(const std::string &nodeName) const {
  auto it = std::find_if(mNodesVector.begin(), mNodesVector.end(),
                         [nodeName](Graph::Node const &node) -> bool { return node.mName == nodeName; });
  return (it == mNodesVector.end()) ? mNodesVector.size() : it->mIndex;
}

void Graph::Node::addChild(NodeIndex index) {
  addIfUnique(index, mChildrenVector);
}

void Graph::Node::addParent(NodeIndex index) {
  addIfUnique(index, mParentsVector);
}

void Graph::Node::addIfUnique(NodeIndex index, std::vector<Graph::NodeIndex>& vector) {
  auto search = std::find(vector.begin(), vector.end(), index);
  if (search == vector.end()) {
    vector.push_back(index);
  }
}

std::vector<Graph::NodeCount> Graph::getCountChildrenAllNodes() const {
  std::vector<NodeCount> counts;
  for (const auto node : mNodesVector) {
    counts.push_back(node.mChildrenVector.size());
  }
  return counts;
}

std::vector<Graph::NodeCount> Graph::getCountParentsAllNodes() const {
  std::vector<NodeCount> counts;
  for (const auto node : mNodesVector) {
    counts.push_back(node.mParentsVector.size());
  }
  return counts;
}
