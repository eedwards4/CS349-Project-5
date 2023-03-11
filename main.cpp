//
// Created by Joe Coon and Ethan Edwards on 3/9/2023.
//

// Includes
#include <iostream>
#include <fstream>
#include <vector>
#include <thread>
#include <future>
#include <queue>
#include "Spaceship.h"

using namespace std;

// Structs
struct PathNode {
    int x, y, cost;
    bool operator>(const PathNode& other) const {
        return cost > other.cost;
    }
};

// Globals
bool DEBUG = false; // THIS WILL SLOW THINGS DOWN

// Functions
void getInfo(vector<Spaceship>& defs, vector<vector<int>>& grid, ifstream& in, pair<int, int>& startXY);
int getPath(vector<vector<int>> grid, int startX, int startY);
int templateMatcher(const vector<Spaceship>& templates, char k);
void initFiles(ifstream& infile, ofstream& outfile, int argc, char* argv[]);

// Main
int main(int argc, char** argv){
    // Read in file
    // Vars for file read
    ifstream infile; ofstream outfile; string line; int cases = 0; pair<int, int> startXY;
    vector<Spaceship> defs;
    vector<vector<int>> grid;

    // Initialize the files
    initFiles(infile, outfile, argc, argv);

    getline(infile, line);
    cases = stoi(line);
    if (cases < 10) {
        for (int i = 0; i < cases; i++) {
            getInfo(defs, grid, infile, startXY);
            outfile << getPath(grid, startXY.first, startXY.second) << '\n';
            defs.clear();
            grid.clear();
        }
    }
    else {
        vector<future<int>> threadHandler;
        for (int i = 0; i < cases; i++) {
            getInfo(defs, grid, infile, startXY);
            threadHandler.push_back(async(getPath, grid, startXY.first, startXY.second));
            defs.clear();
            grid.clear();
        }
        for (auto &output : threadHandler)
            outfile << output.get() << '\n';
    }

    return 0;
}

// Get the pre-run information for a set (xy, ship defs, etc)
void getInfo(vector<Spaceship>& defs, vector<vector<int>>& grid, ifstream& in, pair<int, int>& startXY) {
    string line;
    int numDefs, xBound, yBound, kNum;
    char kClass;
    // Get the number of defs and the xy of the grid
    getline(in, line);
    numDefs = stoi(line.substr(0, line.find(' ')));
    line.erase(0, line.find(' ') + 1);
    xBound = stoi(line.substr(0, line.find(' ')));
    line.erase(0, line.find(' ') + 1);
    yBound = stoi(line.substr(0, line.size()));

    // Get the definitions
    for (int i = 0; i < numDefs; i++){
        getline(in, line);
        kClass = line.substr(0, line.find(' '))[0];
        line.erase(0, line.find(' ') + 1);
        kNum = stoi(line.substr(0, line.size()));
        defs.emplace_back(kNum, kClass);
    }
    // Get the grid (as chars) and convert to ints
    for (int i = 0; i < yBound; i++){
        grid.emplace_back(); // Create empty subvectors
        for (int j = 0; j < xBound; j++){
            in >> kClass;
            if (kClass == 'E') startXY.first = j, startXY.second = i;
            grid.at(i).emplace_back(templateMatcher(defs, kClass));
        }
        in.ignore(); // Skip newline
    }
}

/*
 * This function returns the value of a Spaceship class given a
 * Spaceship class:value template composed of Spaceship objects
 *
 * k is the shipClass character to match with a template value.
 * templates is the list of Spaceship templates holding class:value pairs
 */
int templateMatcher(const vector<Spaceship>& templates, char k){
    for (Spaceship s : templates) {
        if (k == s.shipClass) return s.value;
    }
    return 0;
}

/*
 * Function returns the cost to traverse the shortest path out of the matrix from the start point.
 * This function does not find the components of the shortest path and calculate its cost to traverse,
 * instead assigning a cost for traversal to every vertex within the matrix and returning the minimum value
 * of those vertices which lie on the matrix perimeter.
 *
 *      grid:       The 2D int matrix of each vertex's individual traversal cost.
 *
 *      startX:     The initial x coordinate of the enterprise
 *
 *      startY:     The initial y coordinate of the enterprise
 */
int getPath(vector<vector<int>> grid, int startX, int startY){
    int xPerimeter = grid.size(); //The index of the x perimeter wall
    int yPerimeter = grid[0].size(); //The index of the y perimeter floor
    vector<vector<int>> cost(xPerimeter, vector<int>(yPerimeter, INT_MAX)); //mapping of costs to traverse to each node
    vector<vector<bool>> visited(xPerimeter, vector<bool>(yPerimeter, false)); // Where have we been?

    priority_queue<PathNode, vector<PathNode>, greater<PathNode>> paths; //min_heap of all valid paths

    paths.push({startY, startX, grid[startY][startX]});
    cost[startY][startX] = grid[startY][startX];

    int surroundingX[] = {-1, 0, 1, 0}; // x+1 x-1
    int surroundingY[] = {0, 1, 0, -1}; // y+1 y-1

    while (!paths.empty()) {
        PathNode curr = paths.top(); // Access the path with the minimum cost, assign it as current path.
        paths.pop();

        if (!visited[curr.x][curr.y]) {
            visited[curr.x][curr.y] = true; // If not, flag this as a place we've been

            if (curr.x == 0 || curr.x == xPerimeter - 1 || curr.y == 0 || curr.y == yPerimeter - 1) {
                return cost[curr.x][curr.y]; // Return the current point if it falls on any of the boundaries
            }

            for (int i = 0; i < 4; i++) { // Check the 4 nodes around our current node
                int newX = curr.x + surroundingX[i];
                int newY = curr.y + surroundingY[i];

                if (newX >= 0 && newX < xPerimeter && newY >= 0 && newY < yPerimeter &&
                    !visited[newX][newY]) { //Calculate a new cost for unvisited nodes
                    int newCost = cost[curr.x][curr.y] + grid[newX][newY];

                    if (newCost <
                        cost[newX][newY]) { // If this is a better path, take it! (And place it in paths to be sorted)
                        cost[newX][newY] = newCost; // Update our total distance traveled
                        paths.push({newX, newY, newCost});
                    }
                }
            }
        }
    }

    return -1;
}

/*
 *  This function receives user arguments and assigns them to the ifstream and ofstream objects which manage I/O
 *  for the program. If the user supplies no arguments, this function tries to resolve the issue.
 *
 *  If the user has no input file available to pass, then they must enter ## when prompted to exit the program.
 */
void initFiles(ifstream& infile, ofstream& outfile, int argc, char* argv[]){
    char confirm = 'n';
    string fname;
    // Check for inputs
    if (argc == 3){
        infile.open(argv[1]);
        outfile.open(argv[2]);
    }
    else if (argc == 2) {
        fname = argv[1];
        cout << "Found " << fname << " for input file. Continue using " << fname << " as input? (y/n) ";
        cin >> confirm;
        if (confirm == 'n') {
            cout << "Enter the new input filename: " << endl;
            cin >> fname;
        }
        infile.open(fname);
    }
    else {
        cout << "No input or output filename entered. Please run the program as ./ShipPather.out <input file> <output file>" << endl;
        exit( EXIT_FAILURE);
    }

    // Confirm that the files are actually open & ask for reentry if not valid
    while (!outfile.is_open()){
        cout << "Please provide an existing filename to overwrite, or a new filename to create: ";
        cin >> fname;
        outfile = ofstream(fname, ios::out);
    }
    while (!infile.is_open() && fname != "##"){
        cout << "No input file found. Please provide a relative path and filename for an input file "
             << "or enter ## to close the program: ";
        cin >> fname;
        infile.open(fname);
    }
}