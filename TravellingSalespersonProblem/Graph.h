#pragma once
#include <iostream>
#include <fstream>
#include <sstream>
#include <string>
#include <list>
#include <map>
#include <climits>
#include <vector>
#include <assert.h>
#include <bitset>

using namespace std;

class Graph
{
private:
    size_t numVertices;
    vector<string> vertexNames;
    vector<vector<int>> adjMatrix;

    struct Edge
    {
        unsigned int vDest;
        int cost;
        Edge(unsigned int v = 0, int c = -1)
        {
            vDest = v;
            cost = c;
        }
    };

public:
    Graph() : numVertices(0){};
    ~Graph(){}; // use default destructors.
    unsigned int getNumVertices() { return numVertices; }
    int getEdgeCost(unsigned int v1, unsigned int v2) { return adjMatrix[v1][v2]; } // nums adjusted for starting at 0 instead of 1
    string getVertexName(unsigned int v) { return vertexNames[v]; }
    void read(ifstream &infile)
    {
        vector<list<Edge>> adjList;
        map<string, int> vertexNameIndex;
        // vector<string> vertexNames;

        string line;
        numVertices = 0;
        while (getline(infile, line))
        {
            numVertices++;
            istringstream iss(line);
            int n;
            int c;
            int vLabel;
            string vertex_name;
            vector<int> newRow; // empty vector row.
            iss >> vLabel;      // read past the vertex number in file; indexed internally
            iss >> vertex_name; // this is the important label
            vertexNameIndex[vertex_name] = numVertices - 1;
            vertexNames.push_back(vertex_name); // keep track of label mapping for printing tour

            list<Edge> tempList; // build an adjacency list of a variable number of edges
            tempList.clear();
            while (iss >> n)
            {
                iss >> c;
                n--; // adjust vertex number from file (vertices in 1..numVertices in file)
                tempList.push_back(Edge(n, c));
            }
            adjList.push_back(tempList); // add to the adjacency list version of graph		}
        }

        /*Because the file has potentially missing edges an adjacency list is built above.
        An adjacency matrix is easier to utilize for this dynamic programming solution,
        so it is built from the adjacency list representation. INT_MAX cost is used for missing edges.
        */

        assert(numVertices == adjList.size()); // check condition and start build adjacency matrix
        adjMatrix.resize(numVertices);
        for (size_t i = 0; i < numVertices; i++)
            adjMatrix.at(i).resize(numVertices);

        for (size_t i = 0; i < numVertices; i++)
        {
            for (size_t j = 0; j < numVertices; j++)
            {
                adjMatrix[i][j] = INT_MAX;
            }
        }

        for (size_t i = 0; i < numVertices; i++)
            adjMatrix[i][i] = 0;

        for (size_t i = 0; i < adjList.size(); i++)
        {
            for (list<Edge>::iterator itr = adjList.at(i).begin(); itr != adjList[i].end(); itr++)
            {
                unsigned int j = itr->vDest;
                adjMatrix[i][j] = itr->cost;
            }
        }
    }
    void print(ostream &outfile = cout)
    {
        for (size_t i = 0; i < numVertices; i++)
        {
            for (size_t j = 0; j < numVertices; j++)
            {
                if (adjMatrix[i][j] < INT_MAX)
                {
                    outfile << adjMatrix[i][j] << " ";
                }
                else
                {
                    outfile << "INF ";
                }
            }
            outfile << endl;
        }
    }

    vector<vector<int>> getAdjMatrix() { return adjMatrix; }
};
