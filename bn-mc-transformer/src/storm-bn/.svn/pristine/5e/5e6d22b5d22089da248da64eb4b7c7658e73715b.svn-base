//
// Created by Bahare Salmani on 2019-05-22.
//

#ifndef BNPARSER_DAG_H
#define BNPARSER_DAG_H


// A C++ program to print topological sorting of a DAG
#include<iostream>
#include <list>
#include <stack>
using namespace std;

// Class to represent a graph
class DAG
{
    int V;    // No. of vertices'

    // Pointer to an array containing adjacency listsList
    list<int> *adj;

    // A function used by topologicalSort
    void topologicalSortUtil(int v, bool visited[], stack<int> &Stack);
public:
    DAG(){};
    DAG(int V);   // Constructor

    // function to add an edge to graph
    void addEdge(int v, int w);

    // prints a Topological Sort of the complete graph
    stack<int> topologicalSort();
};

#endif //BNPARSER_DAG_H
