//
// Created by Robin Drahovsky on 5/25/2020.
//

#include "BayesianNetworkTransformer.h"

BayesianNetworkTransformer::JaniCreationData
BayesianNetworkTransformer::runAlgorithm(const Graph &graph, const std::vector<Graph::NodeIndex> &topologicalOrdering,
                                         const std::unordered_map<Graph::NodeIndex, int> &evidence,
                                         const std::vector<bool> &hypothesis) {
  initialize(graph, topologicalOrdering, evidence);
  return runTransformation(graph, topologicalOrdering, evidence, hypothesis);
}


BayesianNetworkTransformer::JaniCreationData
BayesianNetworkTransformer::runTransformation(const Graph &graph,
                                              const std::vector<Graph::NodeIndex> &topologicalOrdering,
                                              const std::unordered_map<Graph::NodeIndex, int> &evidence,
                                              const std::vector<bool> &hypothesis) {
  JaniCreationData resultData;
  for (const auto nodeIndex : topologicalOrdering) {
    processIfEvidence(nodeIndex, evidence, resultData);
    processParents(graph, nodeIndex, hypothesis, resultData);
    for (const auto item : mOpenSet) {
      resultData.positionToKnownNodesMap[mCurrentPosition].push_back(graph.getNameOfNode(item));
    }
    if (graph.getChildren(nodeIndex).empty()) {
      processNode(graph, nodeIndex, hypothesis, resultData, mCurrentPosition+1);
    } else {
      mOpenSet.push_back(nodeIndex);
    }
    ++mCurrentPosition;
  }
  return resultData;
}

void BayesianNetworkTransformer::processIfEvidence(const Graph::NodeIndex indexOfNode,
                                                   const std::unordered_map<Graph::NodeIndex, int> &evidence,
                                                   JaniCreationData &janiData) {
  auto search = evidence.find(indexOfNode);
  if (search != evidence.end()) {
    janiData.positionToEvidenceValue[mCurrentPosition] = search->second;
  }
}

void BayesianNetworkTransformer::processParents(const Graph &graph, const Graph::NodeIndex indexOfNode,
                                                const std::vector<bool> &hypothesis, JaniCreationData &janiData) {
  for (const auto parent : graph.getParents(indexOfNode)) {
    if (!(--mChildrenCount[parent])) {
      mOpenSet.erase(std::remove(mOpenSet.begin(), mOpenSet.end(), parent),
                     mOpenSet.end());
      processNode(graph, parent, hypothesis, janiData, mCurrentPosition);
    }
  }
}

void BayesianNetworkTransformer::processNode(const Graph &graph, const Graph::NodeIndex indexOfNode,
                                             const std::vector<bool> &hypothesis, JaniCreationData &janiData, int position) {
  if (hypothesis[indexOfNode] && position <= mLastIndexOfEvidence) {
    janiData.positionToNodesToResetMap[mLastIndexOfEvidence + 1].push_back(graph.getNameOfNode(indexOfNode));
    for (auto i = position; i <= mLastIndexOfEvidence; ++i) {
      janiData.positionToKnownNodesMap[i].push_back(graph.getNameOfNode(indexOfNode));
    }
  } else {
    janiData.positionToNodesToResetMap[position].push_back(graph.getNameOfNode(indexOfNode));
    janiData.positionToKnownNodesMap[position].push_back(graph.getNameOfNode(indexOfNode));
  }
}

void
BayesianNetworkTransformer::initialize(const Graph &graph, const std::vector<Graph::NodeIndex> &topologicalOrdering,
                                       const std::unordered_map<Graph::NodeIndex, int> &evidence) {
  mCurrentPosition = 0;
  mChildrenCount.clear();
  mOpenSet.clear();
  mChildrenCount = graph.getCountChildrenAllNodes();
  calculateLastPositionOfEvidence(graph, topologicalOrdering, evidence);
}


void BayesianNetworkTransformer::calculateLastPositionOfEvidence(const Graph &graph,
                                                                 const std::vector<Graph::NodeIndex> &topologicalOrdering,
                                                                 const std::unordered_map<Graph::NodeIndex, int> &evidence) {
  if (evidence.empty()) {
    mLastIndexOfEvidence = 0;
    return;
  }
  size_t index = topologicalOrdering.size() - 1;
  for (auto rit = topologicalOrdering.rbegin(); rit != topologicalOrdering.rend(); ++rit) {
    auto search = evidence.find(*rit);
    if (search != evidence.end()) {
      mLastIndexOfEvidence = index;
      break;
    }
    --index;
  }
}








