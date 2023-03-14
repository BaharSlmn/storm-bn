//
// Created by rdrah on 5/25/2020.
//

#include "TopologicalOrderingFinder.h"

std::vector<Graph::NodeIndex> TopologicalOrderingFinder::getTopologicalOrdering(Graph const &graph) {
  std::vector<Graph::NodeIndex> topologicalOrdering;
  std::vector<Graph::NodeIndex> topologicalOrderingBest;
  TopologicalOrderingFinder::OrderingSizeType size;
  TopologicalOrderingFinder::OrderingSizeType sizeBest = std::numeric_limits<TopologicalOrderingFinder::OrderingSizeType>::max();
  std::random_device dev;
  std::mt19937::result_type currentSeed;
  std::mt19937::result_type seedBest;
  AlgorithmData data;

  initialize(graph, data);
  std::cout << "Looking for topological ordering" << "\n";
  for (size_t nodeIndex = 0; nodeIndex < data.mPossibleNodes.size(); ++nodeIndex) {
    std::cout << "Node " << nodeIndex + 1 << " with name: " << graph.getNameOfNode(data.mPossibleNodes[nodeIndex])
              << " out of " << data.mPossibleNodes.size() << "\n";
    auto start = std::chrono::high_resolution_clock::now();
    for (int i = 0; i < 1; ++i) {
      currentSeed = dev();
      std::mt19937 generator(currentSeed);
      data.mGenerator = generator;
      runAlgorithm(graph, topologicalOrdering, data, data.mPossibleNodes[nodeIndex]);
      size = calculateSizeTopologicalOrdering(graph, topologicalOrdering);
      if (size < sizeBest) {
        sizeBest = size;
        topologicalOrderingBest = std::move(topologicalOrdering);
        seedBest = currentSeed;
        logResult(graph, topologicalOrderingBest, sizeBest, seedBest);
      }
      topologicalOrdering.clear();
    }
    auto stop = std::chrono::high_resolution_clock::now();
    auto duration = std::chrono::duration_cast<std::chrono::milliseconds>(stop - start);
    std::cout << "Time taken by function: "
         << duration.count() << " microseconds" << std::endl;
  }
  std::cout << "Trying mixing" << "\n";
  auto result = tryMixing(graph, topologicalOrderingBest, sizeBest);
  if (!checkIfTopologicalOrdering(graph, topologicalOrderingBest)) {
    std::cout << "Not a ordering!" << "\n";
  }
  logResult(graph, topologicalOrderingBest, result, seedBest);
  return topologicalOrderingBest;
}

void TopologicalOrderingFinder::logResult(Graph const &graph, std::vector<Graph::NodeIndex> &topologicalOrdering, OrderingSizeType size, std::mt19937::result_type seed) {
  std::string output;
  output += "Current minimum: ";
  output += std::to_string(size);
  output += " seed: ";
  output += std::to_string(seed);
  output += "\n";
  std::cout << output;
  for (const auto node : topologicalOrdering) {
    std::cout << " " << graph.getNameOfNode(node);
  }
  std::cout << "\n";
}

bool TopologicalOrderingFinder::checkIfTopologicalOrdering(Graph const &graph,
                                                           std::vector<Graph::NodeIndex> &topologicalOrdering) {
  std::vector<Graph::NodeIndex> nodeToTopologicalOrdering(topologicalOrdering.size(), 0);
  if (topologicalOrdering.size() != graph.getNumberOfNodes()) {
    return false;
  }
  // creates vector that can be used to get for a node its position in topological ordering.
  for (int i = 0; i < topologicalOrdering.size(); ++i) {
    nodeToTopologicalOrdering[topologicalOrdering[i]] = i;
  }
  // for each node, we check that all parents are before it in the topological ordering,
  // and all children are after it.
  for (const auto item : topologicalOrdering) {
    for (const auto parent : graph.getParents(item)) {
      if (nodeToTopologicalOrdering[parent] >= nodeToTopologicalOrdering[item]) {
        return false;
      }
    }
    for (const auto child : graph.getChildren(item)) {
      if (nodeToTopologicalOrdering[child] <= nodeToTopologicalOrdering[item]) {
        return false;
      }
    }
  }
  return true;
}

TopologicalOrderingFinder::OrderingSizeType
TopologicalOrderingFinder::tryMixing(Graph const &graph, std::vector<Graph::NodeIndex> &topologicalOrdering,
                                     OrderingSizeType size) {
  std::vector<Graph::NodeIndex> nodeToTopologicalOrdering(topologicalOrdering.size(), 0);
  // creates vector that can be used to get for a node its position in topological ordering.
  for (int i = 0; i < topologicalOrdering.size(); ++i) {
    nodeToTopologicalOrdering[topologicalOrdering[i]] = i;
  }
  OrderingSizeType currentBest = size;
  // tries mixing for each node in order of the topological ordering.
  for (int j = 0; j < topologicalOrdering.size(); ++j) {
    auto result = mixNode(graph, topologicalOrdering, nodeToTopologicalOrdering, topologicalOrdering[j], currentBest);
    if (result < currentBest) {
      currentBest = result;
      // we successfully mixed current node with index j with other node. Now index j contains node for which
      // we have not tried mixing yet, thus we need to decrease j.
      --j;
    }
  }
  return currentBest;
}

TopologicalOrderingFinder::OrderingSizeType
TopologicalOrderingFinder::mixNode(Graph const &graph, std::vector<Graph::NodeIndex> &topologicalOrdering,
                                   std::vector<Graph::NodeIndex> &nodeToTopologicalOrdering, Graph::NodeIndex node,
                                   OrderingSizeType size) {
  size_t indexNode = nodeToTopologicalOrdering[node];
  size_t indexClosestChild = topologicalOrdering.size();
  auto bestSize = size;
  size_t bestIndex;
  // finds index of the closest child of the node in the topological ordering
  for (const auto child : graph.getChildren(node)) {
    if (nodeToTopologicalOrdering[child] < indexClosestChild) {
      indexClosestChild = nodeToTopologicalOrdering[child];
    }
  }
  // goes though the interval node + 1 until the closest child in the topological ordering,
  // as these values can be potentially switched with the node without destroying
  // topological ordering.
  for (auto i = indexNode + 1; i < indexClosestChild; ++i) {
    bool valid = true;
    // checks if the node can really be switched, as there can be parent
    // of i between i and the node.
    for (const auto parent : graph.getParents(topologicalOrdering[i])) {
      if (nodeToTopologicalOrdering[parent] > nodeToTopologicalOrdering[node]) {
        valid = false;
      }
    }
    // If there is no parent, we try to swap the nodes and see if we
    // get smaller ordering. If yes, we save the index, then we swap
    // the nodes again, so that we can check other nodes in the interval
    if (valid) {
      topologicalOrdering[indexNode] = topologicalOrdering[i];
      topologicalOrdering[i] = node;
      auto result = calculateSizeTopologicalOrdering(graph, topologicalOrdering);
      if (result < bestSize) {
        bestSize = result;
        bestIndex = i;
      }
      topologicalOrdering[i] = topologicalOrdering[indexNode];
      topologicalOrdering[indexNode] = node;
    }
  }
  // If there is node to swap, we swap the best node.
  if (bestSize < size) {
    nodeToTopologicalOrdering[topologicalOrdering[bestIndex]] = indexNode;
    nodeToTopologicalOrdering[node] = bestIndex;
    topologicalOrdering[indexNode] = topologicalOrdering[bestIndex];
    topologicalOrdering[bestIndex] = node;
  }
  return bestSize;
}

std::stack<int> TopologicalOrderingFinder::getTopologicalOrderingStack(const std::vector<Graph::NodeIndex> &ordering) {
  std::stack<int> stackOrdering;
  for (const auto item : ordering) {
    stackOrdering.push((int &&) item);
  }
  return stackOrdering;
}

void TopologicalOrderingFinder::initialize(Graph const &graph, AlgorithmData &data) {
  data.clear();
  for (int index = 0; index < graph.getNumberOfNodes(); ++index) {
    if (graph.getParents(index).empty()) {
      data.mPossibleNodes.push_back(index);
    }
    data.mParentsWaiting.push_back(0);
    data.mParentsWaitingRecursive.push_back(0);
    data.mIsClosed.push_back(false);
    data.mIsVisited.push_back(false);
  }
  data.mNumberUnfinishedParents = graph.getCountParentsAllNodes();
  data.mNumberUnfinishedChildren = graph.getCountChildrenAllNodes();
  data.mCompareFunctionsVector = getCompareFunctionsVector(data.mCompareMethod);
  calculateComplexity(graph, data);
}

std::vector<int (*)(const Graph &graph, Graph::NodeIndex, Graph::NodeIndex,
                    TopologicalOrderingFinder::AlgorithmData &data)> TopologicalOrderingFinder::getCompareFunctionsVector(int compareMethod) {
  std::vector<int (*)(const Graph &graph, Graph::NodeIndex, Graph::NodeIndex,
                      AlgorithmData &data)> compareFunctions;
  switch (compareMethod) {
    case 0 :
      compareFunctions.push_back(compareVisited);
      compareFunctions.push_back(compareParentsWaiting);
      compareFunctions.push_back(compareParentsWaitingRecursive);
      compareFunctions.push_back(compareComplexity);
      break;
    default:
      break;
  }
  return compareFunctions;
}

void TopologicalOrderingFinder::calculateComplexity(Graph const& graph, AlgorithmData& data) {
  std::vector<Graph::NodeIndex> possibleNodes;
  std::vector<Graph::NodeCount> numberUnfinishedParents = graph.getCountParentsAllNodes();
  for (int i = 0; i < graph.getNumberOfNodes(); ++i) {
    if (graph.getParents(i).empty()) {
      possibleNodes.push_back(i);
    }
    data.mNodeComplexityMax.push_back(1);
  }
  for (int j = 0; j < graph.getNumberOfNodes(); ++j) {
    if (graph.getChildren(j).empty()) {
      auto current = j;
      while (1 == graph.getParents(current).size()) {
        if (graph.getChildren(graph.getParents(current).front()).size() == 1) {
          data.mNodeComplexityMax[current] -= data.mBonusForLeafNodesComplexity;
          current = graph.getParents(current).front();
        } else {
          break;
        }
      }
      data.mNodeComplexityMax[current] -= data.mBonusForLeafNodesComplexity;
    }
    if (graph.getParents(j).empty()) {
      data.mNodeComplexityMax[j] += data.mPenaltyForSourceNodesComplexity;
    }
  }
  while (!possibleNodes.empty()) {
    auto node = possibleNodes.back();
    possibleNodes.pop_back();
    for (const auto parent : graph.getParents(node)) {
      data.mNodeComplexityMax[node] += data.mNodeComplexityMax[parent];
    }
    for (const auto child : graph.getChildren(node)) {
      if (0 == --numberUnfinishedParents[child]) {
        possibleNodes.push_back(child);
      }
    }
  }
  for (const auto complexity : data.mNodeComplexityMax) {
    data.mNodeComplexityCurrent.push_back(complexity);
  }
}

void
TopologicalOrderingFinder::runAlgorithm(const Graph &graph, std::vector<Graph::NodeIndex> &ordering, AlgorithmData data,
                                        Graph::NodeIndex startingNode) {
  addNode(graph, ordering, data, startingNode);
  while (!data.mPossibleNodes.empty()) {
    addNode(graph, ordering, data, findNextNode(graph, data));
  }
}

void
TopologicalOrderingFinder::addNode(const Graph &graph, std::vector<Graph::NodeIndex> &ordering, AlgorithmData &data,
                                   Graph::NodeIndex node) {
  ordering.push_back(node);
  data.mIsClosed[node] = true;
  data.mPossibleNodes.erase(std::remove(data.mPossibleNodes.begin(), data.mPossibleNodes.end(), node),
                            data.mPossibleNodes.end());
  for (const auto parent : graph.getParents(node)) {
    if (0 == --data.mNumberUnfinishedChildren[parent]) {
      data.mOpenSet.erase(std::remove(data.mOpenSet.begin(), data.mOpenSet.end(), parent),
                          data.mOpenSet.end());
    }
  }
  if (!graph.getChildren(node).empty()) {
    for (const auto child : graph.getChildren(node)) {
      if (0 == --data.mNumberUnfinishedParents[child]) {
        data.mPossibleNodes.push_back(child);
      }
      data.mNodeComplexityCurrent[child] -= data.mNodeComplexityMax[node];
      propagateIsVisited(graph, child, data);
    }
    data.mOpenSet.push_back(node);
  }
  recalculateParentsWaiting(graph, data);
}

void TopologicalOrderingFinder::recalculateParentsWaiting(const Graph &graph, AlgorithmData &data) {
  std::unordered_set<Graph::NodeIndex> parents;
  std::unordered_set<Graph::NodeIndex> children;
  bool valid;
  std::fill(data.mParentsWaiting.begin(), data.mParentsWaiting.end(), 0);
  // we only care about nodes in open set, as those values we want to remove
  for (const auto node : data.mOpenSet) {
    parents.clear();
    children.clear();
    valid = true;
    for (const auto child : graph.getChildren(node)) {
      if (!data.mIsClosed[child]) {
        // if child has no unfinished parent, then it is in possible set, and we can
        // consider it for mParentsWaiting, thus into parents we add all parents as
        // they are all in the open set, and into children we add child.
        if (data.mNumberUnfinishedParents[child] == 0) {
          for (const auto parent : graph.getParents(child)) {
            parents.insert(parent);
          }
          children.insert(child);
          data.mParentsWaitingRecursive[child] += (1.0 / ((double) data.mNumberUnfinishedChildren[node]));
        } else {
          for (const auto parent : graph.getParents(child)) {
            if (!data.mIsClosed[parent]) {
              propagateParentsWaiting(graph, parent, data, (1.0 / (double) data.mNumberUnfinishedChildren[node]) *
                                                           (1.0 / ((double) data.mNumberUnfinishedParents[child])),
                                      data.mMaxNumberOfParentPropagations);
            }
          }
          data.mParentsWaitingRecursive[child] += (1.0 / (double) data.mNumberUnfinishedChildren[node]) *
                                                  (1.0 / ((double) data.mNumberUnfinishedParents[child]));
          valid = false;
        }
      }

    }
    // all non-closed children of the current node have all their parents finished, thus they are in the possible set.
    // Additionally, all their parents are in the open set as they have some unfinished child. Therefore,
    // if we add all children to the topological ordering, we remove all parents from the open set.
    if (valid) {
      if (parents.size() >= children.size()) {
        for (const auto child : children) {
          // this value will be added for each of the parents, so if there are two parents and
          // two children 1 will be result in both, while if there are two parents and
          // one child then the result will be 2.
          data.mParentsWaiting[child] += (1 / (double) children.size());
        }
      }
    }
  }
  // sets bonuses and penalties for the source and leaf nodes.
  for (const auto possibleNode : data.mPossibleNodes) {
    if (graph.getParents(possibleNode).empty()) {
      data.mParentsWaiting[possibleNode] -= data.mPenaltyForSourceNodes;
      data.mParentsWaitingRecursive[possibleNode] -= data.mPenaltyForSourceNodesRecursive;
    }
  }
  for (int i = 0; i < graph.getNumberOfNodes(); ++i) {
    if (!data.mIsClosed[i] && graph.getChildren(i).empty()) {
      data.mParentsWaiting[i] += data.mBonusForLeafNodes;
      auto parent = i;
      while (data.mNumberUnfinishedParents[parent] == 1) {
        for (const auto par : graph.getParents(parent)) {
          if (!data.mIsClosed[par]) {
            parent = par;
            break;
          }
        }
        if (graph.getChildren(parent).size() == 1) {
          data.mParentsWaiting[parent] += data.mBonusForLeafNodes;
        } else {
          break;
        }
      }
      data.mParentsWaitingRecursive[i] += data.mBonusForLeafNodesRecursive;
    }
  }
}

void TopologicalOrderingFinder::propagateParentsWaiting(const Graph &graph, Graph::NodeIndex node, AlgorithmData &data,
                                                        double value, int N) {
  if (--N) {
    if (data.mNumberUnfinishedParents[node]) {
      for (const auto parent : graph.getParents(node)) {
        if (!data.mIsClosed[parent]) {
          propagateParentsWaiting(graph, parent, data, (value / ((double) data.mNumberUnfinishedParents[node])),
                                  N);
        }
      }
    }
    data.mParentsWaitingRecursive[node] += value;
  }
}

void TopologicalOrderingFinder::propagateIsVisited(const Graph &graph, Graph::NodeIndex node, AlgorithmData &data) {
  data.mIsVisited[node] = true;
  for (const auto parent : graph.getParents(node)) {
    if (!data.mIsVisited[parent]) {
      propagateIsVisited(graph, parent, data);
    }
  }
}

Graph::NodeIndex
TopologicalOrderingFinder::findNextNode(const Graph &graph, TopologicalOrderingFinder::AlgorithmData &data) {
  std::vector<Graph::NodeIndex> sameNodes;
  Graph::NodeIndex current = data.mPossibleNodes.front();
  int result;
  for (const auto possibleNode : data.mPossibleNodes) {
    if (possibleNode == data.mPossibleNodes.front()) {
      continue;
    }
    result = compare(graph, possibleNode, current, data);
    switch (result) {
      case -1 :
        current = possibleNode;
        sameNodes.clear();
        break;
      case 0 :
        sameNodes.push_back(possibleNode);
        break;
      case 1 :
        break;
      default:
        std::cout << "Error occurred when comparing node: " << graph.getNameOfNode(possibleNode) << " with node: "
                  << graph.getNameOfNode(current) << "\n";
        break;
    }
  }
  if (data.cAllowRandomness) {
    if (!sameNodes.empty()) {
      sameNodes.push_back(current);
      current = sameNodes[pickRandomIndex(data, sameNodes.size() - 1)];
    }
  }
  return current;
}

int TopologicalOrderingFinder::compare(const Graph &graph, Graph::NodeIndex first, Graph::NodeIndex second,
                                       AlgorithmData &data) {
  int result = 0;
  for (const auto &compareFunction : data.mCompareFunctionsVector) {
    result = compareFunction(graph, first, second, data);
    if (result) {
      return result;
    }
  }
  return result;
}

size_t TopologicalOrderingFinder::pickRandomIndex(AlgorithmData &data, size_t maxIndex) {
  std::uniform_int_distribution<int> dist(0, static_cast<int>(maxIndex));
  return dist(data.mGenerator);
}


int TopologicalOrderingFinder::compareVisited(const Graph &graph, Graph::NodeIndex first, Graph::NodeIndex second,
                                              AlgorithmData &data) {
  if (data.mIsVisited[first] == data.mIsVisited[second]) {
    return 0;
  } else if (data.mIsVisited[first]) {
    return -1;
  }
  return 1;
}

int
TopologicalOrderingFinder::compareParentsWaiting(const Graph &graph, Graph::NodeIndex first, Graph::NodeIndex second,
                                                 AlgorithmData &data) {
  if ((std::fabs(data.mParentsWaiting[first] - data.mParentsWaiting[second])) <=
      data.mMinDifferenceParents + std::numeric_limits<double>::epsilon()) {
    return 0;
  } else if (data.mParentsWaiting[first] > data.mParentsWaiting[second]) {
    return -1;
  }
  return 1;
}

int
TopologicalOrderingFinder::compareParentsWaitingRecursive(const Graph &graph, Graph::NodeIndex first,
                                                          Graph::NodeIndex second,
                                                          AlgorithmData &data) {
  if ((std::fabs(data.mParentsWaitingRecursive[first] - data.mParentsWaitingRecursive[second])) <=
      data.mMinDifferenceParentsRecursive + std::numeric_limits<double>::epsilon()) {
    return 0;
  } else if (data.mParentsWaitingRecursive[first] > data.mParentsWaitingRecursive[second]) {
    return -1;
  }
  return 1;
}

int TopologicalOrderingFinder::compareComplexity(const Graph& graph, Graph::NodeIndex first, Graph::NodeIndex second,
                                                 AlgorithmData& data) {
  auto firstComplexity = calculateCurrentComplexityAverage(graph, first, data);
  auto secondComplexity = calculateCurrentComplexityAverage(graph, second, data);
  if (std::labs(firstComplexity - secondComplexity) <= data.mMinDifferenceComplexity) {
    return 0;
  }
  if (firstComplexity < secondComplexity) {
    return -1;
  }
  return 1;
}

/*
 * Calculates the current complexity of removing the node from the open set after it is added to the topological
 * ordering.
 */
long int
TopologicalOrderingFinder::calculateCurrentComplexityAverage(const Graph& graph, Graph::NodeIndex node,
                                                             AlgorithmData& data) {
  if (graph.getChildren(node).empty()) {
    return 0;
  }
  double sum = 0.0;
  for (const auto child : graph.getChildren(node)) {
      sum += (double) (data.mNodeComplexityCurrent[child] - data.mNodeComplexityMax[node]);
  }
  return (long) std::round(sum / graph.getChildren(node).size());
}

TopologicalOrderingFinder::OrderingSizeType
TopologicalOrderingFinder::calculateSizeTopologicalOrdering(const Graph &graph,
                                                            std::vector<Graph::NodeIndex> &ordering) {
  std::vector<Graph::NodeIndex> openSet;
  std::vector<unsigned long> numOfChildren;
  numOfChildren.reserve(graph.getNumberOfNodes());
  TopologicalOrderingFinder::OrderingSizeType countSum = 1;
  TopologicalOrderingFinder::OrderingSizeType countCurrent;
  for (int i = 0; i < graph.getNumberOfNodes(); ++i) {
    numOfChildren.push_back(graph.getChildren(i).size());
  }
  for (const auto node : ordering) {
    for (const auto parent : graph.getParents(node)) {
      if (0 == --numOfChildren[parent]) {
        openSet.erase(std::remove(openSet.begin(), openSet.end(), parent), openSet.end());
      }
    }
    countCurrent = graph.getNumberOfValues(node);
    for (const auto openNode : openSet) {
      if (graph.getNumberOfValues(openNode) >
          std::numeric_limits<TopologicalOrderingFinder::OrderingSizeType>::max() / countCurrent) {
        return std::numeric_limits<TopologicalOrderingFinder::OrderingSizeType>::max();
      }
      countCurrent *= graph.getNumberOfValues(openNode);
    }
    if (countCurrent > std::numeric_limits<TopologicalOrderingFinder::OrderingSizeType>::max() - countSum) {
      return std::numeric_limits<TopologicalOrderingFinder::OrderingSizeType>::max();
    }
    countSum += countCurrent;
    if (0 < numOfChildren[node]) {
      openSet.push_back(node);
    }
  }
  return countSum;
}


TopologicalOrderingFinder::OrderingSizeType
TopologicalOrderingFinder::calculateSizeTopologicalOrderingBinary(const Graph &graph,
                                                                  std::vector<Graph::NodeIndex> &ordering) {
  std::vector<Graph::NodeIndex> openSet;
  std::vector<unsigned long> numOfChildren;
  numOfChildren.reserve(graph.getNumberOfNodes());
  TopologicalOrderingFinder::OrderingSizeType countSum = 1;
  TopologicalOrderingFinder::OrderingSizeType countCurrent;
  for (int i = 0; i < graph.getNumberOfNodes(); ++i) {
    numOfChildren.push_back(graph.getChildren(i).size());
  }
  for (const auto node : ordering) {
    for (const auto parent : graph.getParents(node)) {
      if (0 == --numOfChildren[parent]) {
        openSet.erase(std::remove(openSet.begin(), openSet.end(), parent), openSet.end());
      }
    }
    // TODO this can be done as 2 pow openSet.size() or so
    countCurrent = 2;
    for (const auto openNode : openSet) {
      if (2 > std::numeric_limits<TopologicalOrderingFinder::OrderingSizeType>::max() / countCurrent) {
        return std::numeric_limits<TopologicalOrderingFinder::OrderingSizeType>::max();
      }
      countCurrent *= 2;
    }
    if (countCurrent > std::numeric_limits<TopologicalOrderingFinder::OrderingSizeType>::max() - countSum) {
      return std::numeric_limits<TopologicalOrderingFinder::OrderingSizeType>::max();
    }
    countSum += countCurrent;
    if (0 < numOfChildren[node]) {
      openSet.push_back(node);
    }
  }
  return countSum + 1;
}