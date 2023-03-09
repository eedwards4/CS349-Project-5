//
// Created by Joe Coon and Ethan Edwards on 3/9/2023.
//

// Includes
#include <iostream>
#include <fstream>
#include <vector>
#include <thread>
#include <queue>
#include <pthread.h>
#include "Spaceship.h"

// Structs
struct Node {
    int x, y, weight, cost;
    bool operator>(const Node& other) const {
        return weight > other.weight;
    }
};

// Globals
bool DEBUG = false;
bool DEBUG_GRIDPRINT = false;
int NUM_THREADS = 4;
// Functions
void getInfo(std::vector<Spaceship>& defs, std::vector<std::vector<int>>& grid, std::vector<std::vector<int>>& weightGrid, std::ifstream& in, std::pair<int, int>& src);
int getPath(std::vector<std::vector<int>> grid, std::vector<std::vector<int>> weights, int srcX, int srcY);
void initFiles(std::ifstream& infile, std::ofstream& outfile, int argc, char* argv[]);
int weightGen(int currX, int currY, int maxX, int maxY);
int charToInt(std::vector<Spaceship>& defs, char in);

// Main
int main(int argc, char** argv){
    // Read in file
    // Vars for file read
    std::ifstream infile; std::ofstream outfile; std::string line; int cases = 0; std::pair<int, int> src;
    std::vector<Spaceship> defs;
    std::vector<std::vector<int>> grid, weightGrid;

    // Initialize the files
    initFiles(infile, outfile, argc, argv);

    std::getline(infile, line);
    cases = stoi(line);
    for (int i = 0; i < cases; cases--){ // Loop through all given cases. Using cases-- to track number remaining
        if (DEBUG){std::cout << cases << " cases remaining.\n";}
        getInfo(defs, grid, weightGrid, infile, src);
        if (DEBUG_GRIDPRINT) { // Output the grid for debugging purposes. DO NOT ENABLE THIS IF YOU WANT THE PROGRAM TO RUN FAST
            for (const std::vector<int> &j: grid) {
                for (int k: j) { std::cout << k << " "; }
                std::cout << "\n";
            }
            for (const std::vector<int> &j: weightGrid) {
                for (int k: j) { std::cout << k << " "; }
                std::cout << "\n";
            }
        }
        if (DEBUG){std::cout << getPath(grid, weightGrid, src.first, src.second) + 6969 << "\n";}
        outfile << getPath(grid, weightGrid, src.first, src.second) + 6969 << "\n";
        defs.clear(); grid.clear(); weightGrid.clear();
    }
    return 0;
}

// Get the pre-run information for a set (xy, ship defs, etc)
void getInfo(std::vector<Spaceship>& defs, std::vector<std::vector<int>>& grid, std::vector<std::vector<int>>& weightGrid, std::ifstream& in, std::pair<int, int>& src){
    if (DEBUG){std::cout << "Entered getInfo.\n";}
    std::string line; int numDefs, xBound, yBound, kNum; char kClass;
    // Get the number of defs and the xy of the grid
    std::getline(in, line);
    numDefs = stoi(line.substr(0, line.find(' ')));
    line.erase(0, line.find(' ') + 1);
    xBound = stoi(line.substr(0, line.find(' ')));
    line.erase(0, line.find(' ') + 1);
    yBound = stoi(line.substr(0, line.size()));

    // Get the definitions
    for (int i = 0; i < numDefs; i++){
        std::getline(in, line);
        kClass = line.substr(0, line.find(' '))[0];
        line.erase(0, line.find(' ') + 1);
        kNum = stoi(line.substr(0, line.size()));
        defs.emplace_back(kNum, kClass);
    }
    // Get the grid (as chars) and convert to ints
    for (int i = 0; i < yBound; i++){
        grid.emplace_back(); // Create empty subvector
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
int charToInt(std::vector<Spaceship>& defs, char in){
    for (Spaceship i : defs){
        if (in == 'E'){return -6969;}
        if (in == i.shipClass){
            return i.value;
        }
    }
    exit(EXIT_FAILURE);
}

// Get the lowest of 4 numbers
Node getLowest(std::vector<Node> src){
    if (DEBUG){std::cout << "SECOND\n";}
    Node lowest = src.at(0);
    for (int i = 1; i < 4; i++){
        if (DEBUG){std::cout << "COMPARING " << src.at(i).cost << " AND " << src.at(i - 1).cost << "\n";}
        if (src.at(i).cost < src.at(i - 1).cost){lowest = src.at(i);}
        else if (src.at(i).cost == src.at(i - 1).cost){
            if (src.at(i).weight < src.at(i - 1).cost){lowest = src.at(i);}
            else{(lowest = src.at(i - 1));}
        }
    }
    return lowest;
}

#include <climits>

// Get the shortest path to the exit
int getPath(std::vector<std::vector<int>> grid, std::vector<std::vector<int>> weights, int srcX, int srcY){
    int n = grid.size();
    int m = grid[0].size();
    std::vector<std::vector<int>> dist(n, std::vector<int>(m, INT_MAX));
    std::vector<std::vector<bool>> visited(n, std::vector<bool>(m, false));

    std::priority_queue<Node, std::vector<Node>, std::greater<Node>> pq;

    pq.push({srcY, srcX, grid[srcY][srcX]});
    dist[srcY][srcX] = grid[srcY][srcX];

    int dx[] = {-1, 0, 1, 0};
    int dy[] = {0, 1, 0, -1};

    while (!pq.empty()) {
        Node curr = pq.top();
        pq.pop();

        if (visited[curr.x][curr.y]) {
            continue;
        }
        visited[curr.x][curr.y] = true;

        if (curr.x == 0 || curr.x == n - 1 || curr.y == 0 || curr.y == m - 1) {
            return dist[curr.x][curr.y];
        }

        for (int i = 0; i < 4; i++) {
            int nx = curr.x + dx[i];
            int ny = curr.y + dy[i];

            if (nx >= 0 && nx < n && ny >= 0 && ny < m && !visited[nx][ny]) {
                int newDist = dist[curr.x][curr.y] + grid[nx][ny];

                if (newDist < dist[nx][ny]) {
                    dist[nx][ny] = newDist;
                    pq.push({nx, ny, newDist});
                }
            }
        }
    }

    return -1;
}

// Get the weight of point n
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
    return std::min(yWeight, xWeight);
}

/*
 *  This function receives user arguments and assigns them to the ifstream and ofstream objects which manage I/O
 *  for the program. If the user supplies no arguments, this function tries to resolve the issue.
 *
 *  If the user has no input file available to pass, then they must enter ## when prompted to exit the program.
 */
void initFiles(std::ifstream& infile, std::ofstream& outfile, int argc, char* argv[]){
    char confirm = 'n';
    std::string fname;
    // Check for inputs
    if (argc == 3){
        infile.open(argv[1]);
        outfile.open(argv[2]);
    }
    else if (argc == 2) {
        fname = argv[1];
        std::cout << "Found " << fname << " for input file. Continue using " << fname << " as input? (y/n) ";
        std::cin >> confirm;
        if (confirm == 'n') {
            std::cout << "Enter the new input filename: \n";
            std::cin >> fname;
        }
        infile.open(fname);
    }
    else {
        std::cout << "No input or output filename entered. Please run the program as ./a.out <input file> <output file>\n"; // TODO: REPLACE ./a.out WITH COMPILED PROGRAM NAME
        exit( EXIT_FAILURE);
    }

    // Confirm that the files are actually open & ask for reentry if not valid
    while (!outfile.is_open()){
        std::cout << "Please provide an existing filename to overwrite, or a new filename to create: ";
        std::cin >> fname;
        outfile = std::ofstream(fname, std::ios::out);
    }
    while (!infile.is_open() && fname != "##"){
        std::cout << "No input file found. Please provide a relative path and filename for an input file or enter ## to close the program: ";
        std::cin >> fname;
        infile.open(fname);
    }
}