//
// Created by Joe Coon and Ethan Edwards on 3/9/2023.
//

// Includes
#include <iostream>
#include <fstream>
#include <climits>
#include <vector>
#include <thread>
#include <queue>
#include "Spaceship.h"

using namespace std;

// Structs
struct Node {
    int x, y, weight;
    bool operator>(const Node& other) const {
        return weight > other.weight;
    }
};

// Globals
bool DEBUG = false; // THIS WILL SLOW THINGS DOWN
bool DEBUG_GRIDPRINT = false; // THIS WILL SLOW THINGS DOWN EVEN MORE
bool DEBUG_CONF_MULTITHREAD = false; // THIS WILL MAKE EVERYTHING GO HYPERSPEED BUT THE OUTPUTS WILL BE OUT OF ORDER YOU HAVE BEEN WARNED
fstream DEBUG_MULTITHREAD_OUTFILE; // SIDENOTE: IF YOU TURN THIS ON WHILE YOU'RE EVALUATING OUR CODE PLEASE COME TALK TO ME FIRST I WANT TO BE THERE FOR IT

// Functions
void getInfo(vector<Spaceship>& defs, vector<vector<int>>& grid, vector<vector<int>>& weightGrid, ifstream& in, pair<int, int>& src);
int getPath(vector<vector<int>> grid, int srcX, int srcY, bool isThread);
void initFiles(ifstream& infile, ofstream& outfile, int argc, char* argv[]);
int weightGen(int currX, int currY, int maxX, int maxY);
int charToInt(vector<Spaceship>& defs, char in);

// Main
int main(int argc, char** argv){
    // Read in file
    // Vars for file read
    ifstream infile; ofstream outfile; string line; int cases = 0; pair<int, int> src;
    vector<Spaceship> defs;
    vector<vector<int>> grid, weightGrid;

    // Initialize the files
    initFiles(infile, outfile, argc, argv);

    getline(infile, line);
    cases = stoi(line);
    // VERY VERY EXPERIMENTAL ZONE
    if (DEBUG_CONF_MULTITHREAD){ // Multithreading wooo!
        cout << "SPOOLING THREADS\n";
        vector<thread> threadHandler; // Vector of threads
        DEBUG_MULTITHREAD_OUTFILE.open("multi_outfile.txt");
        if (!DEBUG_MULTITHREAD_OUTFILE.is_open()){exit(EXIT_FAILURE);}
        for (int i = 0; i < cases; i++){ // Create threads and add them to vector of threads
            // cout << "SPOOLING THREAD #" << i << endl;
            getInfo(defs, grid, weightGrid, infile, src);
            threadHandler.push_back(thread(getPath, grid, src.first, src.second, true));
            if (i + 1 != cases){threadHandler.at(i).detach();}
            else{threadHandler.at(i).join();} // Join only the last thread
            defs.clear(), grid.clear(), weightGrid.clear();
        }
        /*
         * STUFF TO DO TO MAKE THIS FUNCTIONAL:
         * > Figure out why the output isn't ordered correctly (likely threads finishing out of order)
         * > Incorporate multithreading more deeply (set up a way for each thread to return to the main program w/ a thread # and it's output for output ordering??)
         * > Sleep for 72 straight hours lmao
         */
        DEBUG_MULTITHREAD_OUTFILE.close();
        return 0;
    }
    // END VERY VERY EXPERIMENTAL ZONE
    for (int i = 0; i < cases; cases--){ // Loop through all given cases. Using cases-- to track number remaining
        if (DEBUG){cout << cases << " cases remaining.\n";}
        getInfo(defs, grid, weightGrid, infile, src);
        if (DEBUG_GRIDPRINT) { // Output the grid for debugging purposes. DO NOT ENABLE THIS IF YOU WANT THE PROGRAM TO RUN FAST
            for (const vector<int> &j: grid) {
                for (int k: j) { cout << k << " "; }
                cout << "\n";
            }
            for (const vector<int> &j: weightGrid) {
                for (int k: j) { cout << k << " "; }
                cout << "\n";
            }
        }
        if (DEBUG){cout << getPath(grid, src.first, src.second, false) + 6969 << "\n";}
        outfile << getPath(grid, src.first, src.second, false) + 6969 << "\n";
        defs.clear(); grid.clear(); weightGrid.clear();
    }

    return 0;
}

// Get the pre-run information for a set (xy, ship defs, etc)
void getInfo(vector<Spaceship>& defs, vector<vector<int>>& grid, vector<vector<int>>& weightGrid, ifstream& in, pair<int, int>& src){
    if (DEBUG){cout << "Entered getInfo.\n";}
    string line; int numDefs, xBound, yBound, kNum; char kClass;
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
        weightGrid.emplace_back();
        for (int j = 0; j < xBound; j++){
            in >> kClass;
            if (kClass == 'E'){src.first = j, src.second = i;}
            grid.at(i).emplace_back(charToInt(defs, kClass));
            weightGrid.at(i).emplace_back(weightGen(j, i, xBound, yBound));
        }
        in.ignore(); // Skip newline
    }
}

// Convert templated chars to ints, if NULL then exit w/ error
int charToInt(vector<Spaceship>& defs, char in){
    for (Spaceship i : defs){
        if (in == 'E'){return -6969;}
        if (in == i.shipClass){
            return i.value;
        }
    }
    exit(EXIT_FAILURE);
}

// Get the shortest path to the exit
int getPath(vector<vector<int>> grid, int srcX, int srcY, bool isThread){
    int n = grid.size();
    int m = grid[0].size();
    vector<vector<int>> dist(n, vector<int>(m, INT_MAX));
    vector<vector<bool>> visited(n, vector<bool>(m, false)); // Where have we been?
    vector<vector<int>> myGridCopy = grid; // Make a copy of the grid (multithreading support)

    priority_queue<Node, vector<Node>, greater<Node>> pq;

    pq.push({srcY, srcX, myGridCopy[srcY][srcX]});
    dist[srcY][srcX] = myGridCopy[srcY][srcX];

    int dx[] = {-1, 0, 1, 0}; // x+1 x-1
    int dy[] = {0, 1, 0, -1}; // y+1 y-1

    while (!pq.empty()) {
        Node curr = pq.top(); // Where we are
        pq.pop();

        if (visited[curr.x][curr.y]) {
            continue; // Skip if we've already been here
        }
        visited[curr.x][curr.y] = true; // If not, flag this as a place we've been

        if (curr.x == 0 || curr.x == n - 1 || curr.y == 0 || curr.y == m - 1) {
            if (isThread){DEBUG_MULTITHREAD_OUTFILE << dist[curr.x][curr.y] + 6969 << endl; return 0;}
            return dist[curr.x][curr.y]; // Return the current point if it falls on any of the boundaries
        }

        for (int i = 0; i < 4; i++) { // Check the 4 tiles around our current tile
            int nx = curr.x + dx[i];
            int ny = curr.y + dy[i];

            if (nx >= 0 && nx < n && ny >= 0 && ny < m && !visited[nx][ny]) { // Have we visited any of these tiles already?
                int newDist = dist[curr.x][curr.y] + myGridCopy[nx][ny];

                if (newDist < dist[nx][ny]) { // If this is a better path, take it! (And place it in the pq to be sorted)
                    dist[nx][ny] = newDist;
                    pq.push({nx, ny, newDist});
                }
            }
        }
    }

    return -1;
}

// Get the weight of point n (DEPRECATED BUT STILL USEFUL CODE)
int weightGen(int currX, int currY, int maxX, int maxY){
    int xWeight, yWeight;
    if (currY >= maxY / 2){
        yWeight = maxY - currY;
    } else{
        yWeight = currY + 1;
    }
    if (currX >= maxX / 2){
        xWeight = maxX - currX;
    } else{
        xWeight = currX + 1;
    }
    return min(yWeight, xWeight);
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
            cout << "Enter the new input filename: \n";
            cin >> fname;
        }
        infile.open(fname);
    }
    else {
        cout << "No input or output filename entered. Please run the program as ./ShipPather.out <input file> <output file>\n";
        exit( EXIT_FAILURE);
    }

    // Confirm that the files are actually open & ask for reentry if not valid
    while (!outfile.is_open()){
        cout << "Please provide an existing filename to overwrite, or a new filename to create: ";
        cin >> fname;
        outfile = ofstream(fname, ios::out);
    }
    while (!infile.is_open() && fname != "##"){
        cout << "No input file found. Please provide a relative path and filename for an input file or enter ## to close the program: ";
        cin >> fname;
        infile.open(fname);
    }
}