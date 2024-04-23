#include "Graph.h"

// Creating a TSP class
class tspProblem{

private:
    int numVertices;
    int finalStateIndexV;   // this is the base-10 representation of bitset V when
                            // all the vertices are included in the set
                            // i.e. the first (numVertices) indices of bitset<32> V are set starting index 0
                            // if the graph has set of vertices {1,2,3,4}, the final state would be the
                            // whole graph {1,2,3,4}, first 4 indices of V would be set to 1
                            // and the index of THIS state ({1,2,3,4}) would be 15
                            // Note: starting state is the empty set {} with index 0

                            // This is helpful in creating D and P matrices where the rows of these
                            // matrices would represent the vertices and
                            // columns would represent the respective STATES (subsets) of the universal set {1,2,3,4}
    vector<vector<int>> W;
    vector<vector<int>> D;
    vector<vector<int>> P;
    bitset<32> V;
    Graph *graph = nullptr; // create a Graph pointer

    // Compute the minimum tour cost using dynamic programming
    unsigned int computeMinTourCost(unsigned int i, bitset<32>& V) {
        unsigned int aCost;
        int best_j;
        bool found_at_least_one_vertex = false;

        if (V.none()) {
            return W[i][0];     //FIXME: or return D[i][0]?; not working tho
        }

        // this is the index of the current state represented by the bits in V
        unsigned long currStateIndex = V.to_ulong();
        if (D[i][currStateIndex] >= 0) {   // or stateindex
            return D[i][currStateIndex];   // or stateIndex
        }

        int bestCost = INT_MAX;
        best_j = -1;

        for (unsigned int j = 1; j < numVertices; j++) {        // j = 0 or j = 1?; both seems to work as of now
                                                                // however, j = 0 makes much sense bevause V[0] is the first index (cell)
                                                                // j = 1 would skip V[0]

                                                                // UPDATE: j = 1 because the vertex 0 is already excluded
                                                                // at the top level call, so j = 1 makes more sense
                                                                // j = 0 works because the condition if (V[j] == 1)
                                                                // fails for j = 0 since V[0] = 0 and
                                                                // the statements inside if() does not get executed at all
                                                                // for j = 0

            // NOTE: This basically is iterating over permutations of VERTICES (excluding 0), and NOT THE SUBSETS
            // For example, if universal vertices are 0,1,2,3
            // when j = 1, this loop finds out min distance from 1 to 0 with 2 and 3 as intermediate vertices
            // when j = 2, it finds out min distance from 2 to 0 with 1 and 3 as intermediate vertices
            // j = 3 works similarly
            // Note that j is excluded from the set of intermediate vertices each time
            if (V[j] == 1) {
                aCost = W[i][j] + computeMinTourCost(j, V.reset(j));
                V.set(j);       // need to set the bit V[j] back because it was reset in the recursive call above

                if (aCost < bestCost) {
                    found_at_least_one_vertex = true;
                    bestCost = aCost;
                    best_j = j;
                }
            }
        }

        if (found_at_least_one_vertex) {
            D[i][currStateIndex] = bestCost;
            P[i][currStateIndex] = best_j;
        }

        return bestCost;
    }

    void printMinTourCost(unsigned int numVertices, vector<vector<int>> P, bitset<32> V)
    {
        unsigned int v = 0;
        bitset<32> A = V;
        A.reset(v);

        //cout << v + 1; // print starting vertex: 0 + 1 = 1
        cout << this->graph->getVertexName(v);        // this prints the vertex label

        while (A.any())
        {
            cout << ",";
            unsigned long currStateIndex = A.to_ulong();
            v = P[v][currStateIndex];
            //cout << v + 1;      // print the current vertex
            cout << this->graph->getVertexName(v);

            A.reset(v);
        }
        //cout << "," << 1; // return back to the starting vertex
        cout << "," << this->graph->getVertexName(0);
    }

public:

    // constructor for the tspProblem instance
    tspProblem(Graph* graph) {
        setNumVertices(graph);
        setFinalStateIndex(graph);
        setW(graph);
        setD();
        setP();
        setV();
        this->graph = graph;        // &graph
    }

    void setNumVertices(Graph* graph) {
        this->numVertices = graph->getNumVertices();
    }
    void setFinalStateIndex(Graph* graph) {
        this->finalStateIndexV = (1 << graph->getNumVertices());
        //this->finalStateIndexV = V.to_ulong(); // this was giving errors
    }
    void setW(Graph* graph) {
        this->W = graph->getAdjMatrix();
    }
    void setD() {
        //this->D = vector<vector<int>>(this->numVertices, vector<int>(this->longVal, -1)); // Set initial values to -1
        D.assign(numVertices, vector<int>(finalStateIndexV , -1));
        // if numVertices is 4 and
        // if the last state index is 15, the function creates a matrix of dimension (4 * 15+1) i.e 4*16
        // where the row indices: 0 to 3 and column indices: 0 to 15
    }
    void setP(){
        //P = vector<vector<int>>(numVertices, vector<int>(this->longVal, -1));     // set initial values to -1
        P.assign(numVertices, vector<int>(finalStateIndexV , -1));
        // same logic as matrix D
    }
    void setV() {       // sets the V
        for (unsigned int i = 0; i < this->numVertices; i++) {
            V.set(i);
        }
    }

    void solve() {
        bitset<32> tempSet = V;
        unsigned int bestTourCost= computeMinTourCost(0, tempSet.reset(0));

        if (bestTourCost != INT_MAX) {
            cout << "Optimal Tour Cost = [" << bestTourCost << ", <";
            printMinTourCost(numVertices, P, V); // passing the Universal set with all vertices included
            cout << ">]" << endl;
        }
        else {
            cout << "No Hamiltonian Cycle" << endl;
        }
    }
};


int main() {

    Graph *graph = new Graph();   // pointer to a Graph object, allocating memory for the Graph object

    // read the .txt file to insert to the Graph object
    string filename = "";
    cout << "Enter filename: ";
    cin >> filename;
    ifstream file(filename);

    // check if the file can't be opened
    if (!file.is_open())
    {
        cerr << "Failed to open file: " << filename << endl;
        delete graph; // delete the graph object before exiting due to the error
        return 1;     // return an error code
    }

    // reads the file content once open
    graph->read(file);
    file.close();

    // create a tspProblem instance
    tspProblem problem(graph);

    // solve the problem instance
    problem.solve();

    // delete the allocated Graph memory
    delete graph;

    return 0;
}

