//
// Created by rdrah on 5/25/2020.
//

#ifndef TRANSFORMATION_GRAPH_H
#define TRANSFORMATION_GRAPH_H

#include <memory>
#include <string>
#include <utility>
#include <vector>
#include <unordered_set>
#include <algorithm>

class Graph {
public:
  using NodeIndex = int32_t;
  using NodeCount = int32_t;

  Graph() { mNumberOfVertices = 0; };

  /*!
   * Adds node corresponding to random variable to graph.
   * @param name Name of node.
   * @param numberOfValues Number of possible values of the random variable.
   * @return Index of created node.
   */
  NodeIndex addNode(const std::string &name, NodeIndex numberOfValues);

  /*!
   * Adds an edge to graph. If either the parent or child do not exists, nothing is added.
   * @param indexParent Index of the source node.
   * @param indexChild Index of the target node.
   */
  void addEdge(NodeIndex indexChild, NodeIndex indexParent);

  NodeCount getNumberOfNodes() const {
    return mNumberOfVertices;
  }

  const std::vector<NodeIndex>& getChildren(NodeIndex indexOfVertex) const {
    return mNodesVector[indexOfVertex].mChildrenVector;
  }

  const std::vector<NodeIndex>& getParents(NodeIndex indexOfVertex) const {
    return mNodesVector[indexOfVertex].mParentsVector;
  }

  const std::string& getNameOfNode(NodeIndex index) const {
    return mNodesVector[index].mName;
  }

  /*!
   * Finds index of the first node with given name.
   * @param nodeName Name of the node.
   * @return Index of the node. If there is no node with
   * given name, current number of nodes in graph is returned.
   */
  NodeIndex getNodeIndex(const std::string &nodeName) const;

  NodeCount getNumberOfValues(NodeIndex index) const {
    return mNodesVector[index].mNumberOfValues;
  }

  /*!
   * Gets vector that for each node contains count of its
   * children.
   * @return Vector of children counts.
   */
  std::vector<NodeCount> getCountChildrenAllNodes() const;

  /*!
   * Gets vector that for each node contains count of its
   * parents.
   * @return Vector of parents counts.
   */
  std::vector<NodeCount> getCountParentsAllNodes() const;

private:

  struct Node {
    /*
     * Creates new node.
     * @param index Index of the new node.
     * @param name Name of the new node.
     * @param numberOfValues Number of possible values of the new node.
     */
    Node(NodeIndex index, std::string name, NodeIndex numberOfValues) : mIndex(index), mName(std::move(name)),
                                                                        mNumberOfValues(numberOfValues) {};
    NodeIndex mIndex;
    std::string mName;
    /*
     * Number of possible values of random variable corresponding to the node.
     */
    NodeIndex mNumberOfValues;
    std::vector<NodeIndex> mParentsVector;
    std::vector<NodeIndex> mChildrenVector;
    /*
     * Adds node to childVector if it is not already there.
     */
    void addChild(NodeIndex index);
    /*
     * Adds node to parentVector if it is not already there.
     */
    void addParent(NodeIndex index);
    void addIfUnique(NodeIndex index, std::vector<Graph::NodeIndex> &vector);
  };

private:
  NodeCount mNumberOfVertices;
  std::vector<Node> mNodesVector;

};


#endif //TRANSFORMATION_GRAPH_H
