//
// Created by rdrah on 5/25/2020.
//

#ifndef TRANSFORMATION_TOPOLOGICALORDERINGFINDER_H
#define TRANSFORMATION_TOPOLOGICALORDERINGFINDER_H

#include <unordered_map>
#include <unordered_set>
#include <limits>
#include <iterator>
#include <stack>
#include <iostream>
#include <random>
#include <cmath>
#include "storm-bn-robin/src/transformer/Graph.h"
#include <chrono>

class TopologicalOrderingFinder {
public:
  /*
   * Used for representing size of the orderings.
   */
  typedef unsigned long long int OrderingSizeType;

  /*!
   * Finds topological ordering of a graph.
   * @return Vector of indexes ordered in topological order.
   */
  static std::vector<Graph::NodeIndex> getTopologicalOrdering(Graph const &graph);

  /*!
   * Transforms topological ordering from vector into stack.
   * @param ordering Topological ordering.
   * @return stack containing the ordering.
   */
  static std::stack<int> getTopologicalOrderingStack(const std::vector<Graph::NodeIndex> &ordering);

  /*!
   * Calculates size of the DTMC resulting from the topological ordering. It does not account for the
   * zero and one probability edges, thus some networks might have even smaller size when build on
   * storm. In case of integer overflow, it returns the maximal value of the OrderingSizeType.
   * @param graph Graph representing the DAC of the network.
   * @param ordering Topological ordering of the graph.
   * @return size of the resulting DTMC.
   */
  static OrderingSizeType calculateSizeTopologicalOrdering(const Graph &graph, std::vector<Graph::NodeIndex> &ordering);

  /*!
 * Calculates size of the DTMC resulting from the topological ordering, but assumes that all the variables
 * are binary. It does not account for the zero and one probability edges. In case of integer overflow,
 * it returns the maximal value of the OrderingSizeType.
 * @param graph Graph representing the DAC of the network.
 * @param ordering Topological ordering of the graph.
 * @return size of the resulting DTMC.
 */
  static OrderingSizeType
  calculateSizeTopologicalOrderingBinary(const Graph &graph, std::vector<Graph::NodeIndex> &ordering);

public:
  /*!
   * Contains information about current state of the algorithm.
   */
  struct AlgorithmData {
    /*
     * Contains nodes that can be added to the ordering - all their parents have been
     * already added to the ordering.
     */
    std::vector<Graph::NodeIndex> mPossibleNodes;
    /*
     * Contains all nodes for which we need to remember values - they were already added
     * to the ordering, but some of its children were not.
     */
    std::vector<Graph::NodeIndex> mOpenSet;
    /*
     * For each node contains number of parents that were not yet added to the ordering.
     */
    std::vector<Graph::NodeCount> mNumberUnfinishedParents;
    /*
     * For each node contains number of children that were not yet added to the ordering.
     */
    std::vector<Graph::NodeCount> mNumberUnfinishedChildren;
    /*
     * Represents for each node approximate number of nodes that will be removed from the
     * open set if this node is added to the ordering. Only accounts for nodes in the possible
     * set.
     */
    std::vector<double> mParentsWaiting;
    /*
     * Represents for each node approximate number of nodes that will be removed from the
     * open set if this node is added to the ordering, but also accounts for nodes with
     * some unfinished parent - nodes that are not currently in the possible set.
     */
    std::vector<double> mParentsWaitingRecursive;
    /*
     * Contains information about whether a node was already added to the ordering or not.
     * If node has value true, it was already added to the ordering.
     */
    std::vector<bool> mIsClosed;
    /*
     * Contains information about whether any parents of any of descendants of a node was
     * already added to the ordering.
     */
    std::vector<bool> mIsVisited;
    /*
     * Maximum complexity of a node. Equals to the sum of maximum complexity of nodes parents + 1.
     */
    std::vector<long> mNodeComplexityMax;
    /*
     * Current complexity of a node. Equals to the maximum complexity of node minus sum of maximum
     * complexity of all its closed parents.
     */
    std::vector<long> mNodeComplexityCurrent;
    /*
     * Indicates which comparison method will be used to compare nodes.
     */
    int mCompareMethod = 0;
    /*
     * Vector of compare functions used to compare nodes.
     */
    std::vector<int (*)(const Graph &graph, Graph::NodeIndex, Graph::NodeIndex,
                        AlgorithmData &data)> mCompareFunctionsVector;

    /*
     * Maximum number of time mParentsWaitingRecursive will be propagated to parents - 1.
     * If this is 1, no propagation will happen. If this is 0, there is no limit on number
     * of propagations.
     */
    int mMaxNumberOfParentPropagations = 10;
    /*
     * Value that will be subtracted from maximum complexity of each of the leaf nodes.
     */
    int mBonusForLeafNodesComplexity = 0;
    /*
     * Value that will be added to the maximum complexity of each of the source nodes.
     */
    int mPenaltyForSourceNodesComplexity = 0;
    /*
     * Values that will be subtracted from mParentsWaiting value of each source node.
     */
    double mPenaltyForSourceNodes = 0;
    /*
     * Values that will be subtracted from mParentsWaitingRecursive value of each source node.
     */
    double mPenaltyForSourceNodesRecursive = 0;
    /*
     * Value that will be added to mParentsWaiting value of each leaf node.
     */
    double mBonusForLeafNodes = 0;
    /*
     * Value that will be added to mParentsWaitingRecursive value of each leaf node.
     */
    double mBonusForLeafNodesRecursive = 1;
    /*
     * Minimum difference between complexities of two nodes node such that they are
     * considered different from each other.
     */
    double mMinDifferenceComplexity = 100;
    /*
     * Minimum difference between mParentsWaiting values of two nodes node such that they are
     * considered different from each other.
     */
    double mMinDifferenceParents = 0;
    /*
     * Minimum difference between mParentsWaitingRecursive values of two nodes node such that they are
     * considered different from each other.
     */
    double mMinDifferenceParentsRecursive = 5;
    /*
     * Used to pick from nodes randomly.
     */
    std::mt19937 mGenerator;

    static constexpr bool cAllowRandomness = false;

    /*
     * Clears all algorithm data.
     */
    void clear() {
      mPossibleNodes.clear();
      mOpenSet.clear();
      mNumberUnfinishedParents.clear();
      mNumberUnfinishedChildren.clear();
      mParentsWaiting.clear();
      mParentsWaitingRecursive.clear();
      mIsClosed.clear();
      mIsVisited.clear();
      mNodeComplexityMax.clear();
      mNodeComplexityCurrent.clear();
      mCompareFunctionsVector.clear();
    }
  };

private:

  /*
   * Initializes algorithm data.
   */
  static void initialize(Graph const &graph, AlgorithmData &data);

  /*
   * Runs algorithm. Firstly, adds the startingNode to topological ordering
   * and then while it is still possible chooses one node from possible nodes
   * and adds it to the ordering. When the algorithm finishes ordering should
   * contain topological ordering.
   */
  static void
  runAlgorithm(const Graph &graph, std::vector<Graph::NodeIndex> &ordering, AlgorithmData data,
               Graph::NodeIndex startingNode);

  /*
   * Chooses the next node from possible nodes that should be added to the
   * topological ordering.
   */
  static Graph::NodeIndex findNextNode(const Graph &graph, AlgorithmData &data);

  /*
   * Adds node to the topological ordering, and changes the algorithm data
   * accordingly.
   */
  static void
  addNode(const Graph &graph, std::vector<Graph::NodeIndex> &ordering, AlgorithmData &data,
          Graph::NodeIndex nodeToAdd);

  /*
   * When a node is added to the topological ordering, for each of its children
   * this method is called to propagate visited true to all of its ancestors.
   */
  static void propagateIsVisited(const Graph &graph, Graph::NodeIndex node, AlgorithmData &data);

  /*
   * Used to calculate values of mParentsWaiting and mParentsWaitingRecursive, after a
   * node was added to the topological ordering.
   */
  static void recalculateParentsWaiting(const Graph &graph, AlgorithmData &data);

  /*
   * Compares first and second node. Returns -1 if the first node is better, 1 if the second
   * node is better, and 0 if nodes are considered equal.
   */
  static int compare(const Graph &graph, Graph::NodeIndex first, Graph::NodeIndex second, AlgorithmData &data);

  /*
   * Compares first and second node based on its visited values. If first node is visited and second
   * is not then method returns -1. If second is visited and first is not then 1 is returned. Otherwise,
   * 0 is returned.
   */
  static int compareVisited(const Graph &graph, Graph::NodeIndex first, Graph::NodeIndex second, AlgorithmData &data);

  /*
   * Compares values of the mParentsWaiting of first and second node. If difference of values is
   * smaller than the minimal difference 0 is returned. Otherwise, returns -1 if value of first
   * is larger than the second one, and 1 if the second one is larger.
   */
  static int compareParentsWaiting(const Graph &graph, Graph::NodeIndex first, Graph::NodeIndex second, AlgorithmData &data);

  /*
   * Propagates values of mParentsWaitingRecursive to the parents of a node if possible. N correspond
   * to maximum amount of propagations.
   */
  static void propagateParentsWaiting(const Graph &graph, Graph::NodeIndex node, AlgorithmData &data, double value, int N);

  /*
   * Tries to do small chances to the topological ordering, to see whether it can improve it.
   */
  static OrderingSizeType
  tryMixing(const Graph &graph, std::vector<Graph::NodeIndex> &topologicalOrdering, OrderingSizeType size);

  /*
   * Tries to do small chances to the topological ordering in the node, to see whether it can improve it.
   */
  static OrderingSizeType
  mixNode(const Graph &graph, std::vector<Graph::NodeIndex> &topologicalOrdering,
          std::vector<Graph::NodeIndex> &nodeToTopologicalOrdering, Graph::NodeIndex node, OrderingSizeType size);

  /*
   * Checks whether that topologicalOrdering is actually a topological ordering on the graph.
   */
  static bool checkIfTopologicalOrdering(const Graph &graph, std::vector<Graph::NodeIndex> &topologicalOrdering);

  /*
   * Picks random index from 0 to maxIndex.
   */
  static size_t pickRandomIndex(AlgorithmData &data, size_t maxIndex);

  /*
 * Compares values of the mParentsWaitingRecursive of first and second node. If difference of values is
 * smaller than the minimal difference 0 is returned. Otherwise, returns -1 if value of first
 * is larger than the second one, and 1 if the second one is larger.
 */
  static int
  compareParentsWaitingRecursive(const Graph &graph, Graph::NodeIndex first, Graph::NodeIndex second, AlgorithmData &data);

  /*
   * Outputs information on the standard output.
   */
  static void logResult(const Graph &graph, std::vector<Graph::NodeIndex> &topologicalOrdering, OrderingSizeType size,
                        std::mt19937::result_type seed);

  /*
   * Calculates maximum complexity of nodes.
   */
  static void calculateComplexity(const Graph &graph, AlgorithmData &data);

  /*
 * Compares values of the mParentsWaitingRecursive of first and second node. If difference of values is
 * smaller than the minimal difference 0 is returned. Otherwise, returns -1 if value of first
 * is smaller than the second one, and 1 if the second one is smaller.
 */
  static int compareComplexity(const Graph &graph, Graph::NodeIndex first, Graph::NodeIndex second, AlgorithmData &data);

  /*
   * Calculates average complexity of children of the given node.
   */
  static long calculateCurrentComplexityAverage(const Graph &graph, Graph::NodeIndex node, AlgorithmData &data);

  static std::vector<int (*)(const Graph &, Graph::NodeIndex, Graph::NodeIndex, AlgorithmData &)>
  getCompareFunctionsVector(int compareMethod);
};


#endif //TRANSFORMATION_TOPOLOGICALORDERINGFINDER_H
