//
// Created by rdrah on 5/25/2020.
//

#ifndef TRANSFORMATION_BAYESIANNETWORKTRANSFORMER_H
#define TRANSFORMATION_BAYESIANNETWORKTRANSFORMER_H

#include <string>
#include <memory>
#include <unordered_map>
#include <unordered_set>
#include <utility>
#include <iostream>
#include "Graph.h"


class BayesianNetworkTransformer {
public:

  /*!
   * Data needed to create jani file of corresponding RDTMC.
   */
  struct JaniCreationData {
    /*!
     * Maps position in topological ordering in vector of nodes, whose values
     * can be forgotten in this position and no previous position. If there
     * are no such nodes at position, map does not contain the position.
     */
    std::unordered_map<size_t, std::vector<std::string>> positionToNodesToResetMap;
    /*!
     * Maps position in topological ordering to vector of nodes, whose values
     * are known at that position. If there are no such nodes at position, map
     * does not contain the position.
     */
    std::unordered_map<size_t, std::vector<std::string>> positionToKnownNodesMap;
    /*!
     * Maps position in topological ordering to value of evidence node, that
     * is being set in that position. If there are no such nodes at position, map
     * does not contain the position.
     */
    std::unordered_map<size_t, int> positionToEvidenceValue;
  };

  /*!
   * Creates data needed to create Jani file corresponding to network.
   * @param graph DAG graph.
   * @param topologicalOrdering Topological ordering of the graph.
   * @param evidence Map between nodes in the evidence and their corresponding values.
   * @param hypothesis Vector indicating whether node belongs to hypothesis or not.
   * @return JaniCreationData that are needed to create corresponding RDTMC.
   */
  JaniCreationData runAlgorithm(const Graph &graph, const std::vector<Graph::NodeIndex> &topologicalOrdering,
                                const std::unordered_map<Graph::NodeIndex, int> &evidence,
                                const std::vector<bool> &hypothesis);

private:
  /*
   * Initializes member variables.
   */
  void initialize(const Graph &graph, const std::vector<Graph::NodeIndex> &topologicalOrdering,
                  const std::unordered_map<Graph::NodeIndex , int> &evidence);

  /*
   * Calculates which node that is part of evidence has the largest index in the
   * topological ordering and sets mLastIndexOfEvidence to that values.
   */
  void calculateLastPositionOfEvidence(const Graph &graph, const std::vector<Graph::NodeIndex> &topologicalOrdering,
                                       const std::unordered_map<Graph::NodeIndex, int> &evidence);
  /*
   * Runs the main algorithm that creates the jani data.
   */
  JaniCreationData runTransformation(const Graph &graph, const std::vector<Graph::NodeIndex> &topologicalOrdering,
                                     const std::unordered_map<Graph::NodeIndex , int> &evidence,
                                     const std::vector<bool> &hypothesis);

  /*
   * Processes parents of the current node of the topological ordering. If the current node
   * is the child of a parent with the largest index in the topological ordering, the parent
   * needs to be removed from open set and needs to have its value reset. However, if the
   * parent is in hypothesis, and mLastIndexOfEvidence is larger than the current position,
   * it is necessary to postpone reset of its value until mLastIndexOfEvidence + 1.
   */
  void processParents(const Graph &graph, const Graph::NodeIndex indexOfNode, const std::vector<bool> &hypothesis,
                      JaniCreationData &janiData);

  /*
   * However, if it is in hypothesis, and mLastIndexOfEvidence
   * is larger than the position, it is necessary to postpone reset of its value
   * until mLastIndexOfEvidence + 1.
   */
  void processNode(const Graph &graph, const Graph::NodeIndex indexOfNode, const std::vector<bool> &hypothesis,
                          JaniCreationData &janiData, int position);

private:
  /*
   * Contains nodes that have index in the topological ordering, smaller than
   * mCurrentPosition, but some of its children have index larger than
   * mCurrentPosition.
   */
  std::vector<Graph::NodeIndex> mOpenSet;
  /*
   * Index, in topological ordering, of the currently processed node.
   */
  Graph::NodeIndex mCurrentPosition;
  /*
   * Index of a node from evidence, that has the largest index in the
   * topological ordering.
   */
  Graph::NodeIndex mLastIndexOfEvidence;
  /*
   * For each node contains number of children, that has index in topological
   * ordering larger than the mCurrentPosition.
   */
  std::vector<Graph::NodeCount> mChildrenCount;

  void processIfEvidence(const Graph::NodeIndex indexOfNode, const std::unordered_map<Graph::NodeIndex, int> &evidence,
                         JaniCreationData &janiData);
};

#endif //TRANSFORMATION_BAYESIANNETWORKTRANSFORMER_H
