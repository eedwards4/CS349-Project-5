#include <iostream>
#include <fstream>
#include <vector>
#include <tuple>
#include <algorithm>
#include <queue>
#include "Spaceship.h"

using namespace std;

const int MAX_DURATION = 100000;

int templateMatcher(const vector<Spaceship>& templates, char k);
int weightMatcher(int yBound, int xBound, int yPos, int xPos);
int fastestRoute(const vector<vector<Spaceship>>& ships, pair<int, int> currentYX);
int pathEval(const vector<vector<Spaceship>>& ships, pair<int, int> currentYX, const vector<pair<int, int>>& currentPath);
pair<int, int> chooseNeighbor(pair<int, int> currentYX, const vector<pair<int, int>>& visited, const vector<vector<Spaceship>>& ships);
vector<pair<int, int>> getCandidates(pair<int, int> currentYX, const vector<pair<int, int>>& visited);
void initFiles(ifstream& infile, ofstream& outfile, int argc, char* argv[]);

int main(int argc, char** argv) {
    ifstream infile;
    ofstream outfile;
    string line;
    char kClass, kShip;
    int cases, numShips, kValue, xBound, yBound;
    vector<Spaceship> templates = { Spaceship(0, 'E') }; //The enterprise template always exists
    vector<vector<Spaceship>> ships;
    pair<int, int> startYX;

    initFiles(infile, outfile, argc, argv);

    if (infile.is_open() && outfile.is_open()) {
        getline(infile, line);
        cases = stoi(line);
        for (int i = 0; i < cases; i++){
            getline(infile, line);
            numShips = stoi(line.substr(0, line.find(' ')));
            line.erase(0, line.find(' ') + 1);
            xBound = stoi(line.substr(0, line.find(' '))); // split xBound
            line.erase(0, line.find(' ') + 1);
            yBound = stoi(line.substr(0, line.find(' '))); // split yBound
            for (int j = 0; j < numShips; j++) {
                getline(infile, line);
                kClass = line.substr(0, line.find(' '))[0];
                line.erase(0, line.find(' ') + 1);
                kValue = stoi(line.substr(0, line.size()));
                templates.emplace_back(kValue, kClass);
            }
            for (int yPos = 0; yPos < yBound; yPos++){ //lines are read into ship by y value,
                                                       //then by x value; coordinates paired in pairs of
                                                       //{y, x} from here on.
                ships.emplace_back();
                for (int xPos = 0; xPos < xBound; xPos++){
                    infile >> kShip;
                    if (kShip == 'E') startYX = make_pair(yPos, xPos);
                    ships.at(yPos).emplace_back(templateMatcher(templates, kShip), //coord value from class
                                                weightMatcher(yBound, xBound, yPos, xPos), //coord weight
                                                kShip); //coord class
                }
                infile.ignore(); //skip newline
            }

            //calculate and output duration
            outfile << fastestRoute(ships, startYX) << endl;

            templates.clear();
            ships.clear();
            templates.emplace_back(0, 'E'); //Add the Enterprise Spaceship template
        }
    }

    infile.close();
    outfile.close();
    return 0;
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
    return -1;
}

/*
 * This function calculates the perimeter weight of a matrix element
 * given its coordinates within the bounds of the matrix.
 *
 *      yPos, xPos:     Receives the current coordinates of a matrix element.
 *      yBound, xBound: Receives the bounds of the matrix.
 *
 * Returns the perimeter weight of a matrix element
 */
int weightMatcher(int yBound, int xBound, int yPos, int xPos) {
    int yWeight, xWeight;
    if (yPos >= yBound / 2) {
        yWeight = yBound - yPos; //count descending from middle to end
    }
    else {
        yWeight = yPos + 1; //count ascending from start to middle
    }
    if (xPos >= xBound / 2) { //count descending from middle to end
        xWeight = xBound - xPos;
    }
    else {
        xWeight = xPos + 1;  //count ascending from start to middle
    }
    return min(yWeight, xWeight); //weight is for the nearest boundary
}

/*
 *
 */
int fastestRoute(const vector<vector<Spaceship>>& ships, pair<int, int> currentYX) {
    struct Path {
        vector<pair<int, int>> path;
        int cost;

        Path(const vector<pair<int, int>>& p, int c) {
            path = p;
            cost = c;
        }

        struct compare {
            bool operator()(const Path& a, const Path& b) {
                return a.cost > b.cost;
            }
        };
    };

    priority_queue<Path, vector<Path>, Path::compare> pathHistory; //priorityqueue with custom compare function to
                                                                   //store elements in non-decreasing order; improved
                                                                   //performance on grabbing the smallest element, logn
                                                                   //performance on pushing/popping elements
    Path thisRoute({currentYX}, pathEval(ships, currentYX, {currentYX}));

    while (thisRoute.cost != -1) { //do while path is not a dead end
        //found an exit path, add it to the history
        pathHistory.push(thisRoute);

        //find fork options
        

        //evaluate a new path
        thisRoute.cost = pathEval(ships, currentYX, thisRoute.path);
    }
    //manage behavior when thisRoute is a dead end path
}

/*
 * This function uses Dijkstra's algorithm to construct paths through the list until
 * the path with the smallest traversal cost is found.
 *
 *     currentYX:   the coordinate that centers the adjacency search. If this is a member of candidates,
 *                  then the path is a dead end.
 *
 *     visited:     the coordinate list of elements already traversed on the current path.
 *
 *     ships:       the matrix of Spaceship elements to traverse through.
 *
 *     chosen:      the candidate element to traverse through that is selected with Dijkstra's algorithm.
 *
 *     sum:         the cost of traversing the current path.
 */
int pathEval(const vector<vector<Spaceship>>& ships, pair<int, int> currentYX, const vector<pair<int, int>>& currentPath) {
    vector<pair<int, int>> visited = currentPath; //visited coordinates
    pair<int, int> chosen; //coordinate of the next ship to traverse
    int sum = 0;

    while (ships[currentYX.first][currentYX.second].perimeterWeight > 1) { //do while within perimeter of ships matrix
        chosen = chooseNeighbor(currentYX, visited, ships);
        if (chosen == currentYX) return -1; //debug line, enterprise got trapped.
        visited.push_back(chosen);
        currentYX = chosen;
        sum += ships[chosen.first][chosen.second].value;
    }

    return sum;
}

/*
 * This function selects the next adjacent element to traverse on the current path and returns
 * that elements coordinate.
 *
 *     currentYX:   the coordinate that centers the adjacency search. If this is a member of candidates,
 *                  then the path is a dead end.
 *
 *     visited:     the coordinate list of elements already traversed on the current path.
 *
 *     ships:       the matrix of Spaceship elements to path through.
 */
pair<int, int> chooseNeighbor(pair<int, int> currentYX, const vector<pair<int, int>>& visited, const vector<vector<Spaceship>>& ships) {
    int minVal = MAX_DURATION, minWeight = INT_MAX; //smallest traversal value and perimeter weight inspected
    int finalY, finalX; //currently chosen candidate coordinates

    for (pair p : getCandidates(currentYX, visited)) {
        int candidateVal = ships[p.first][p.second].value; //value for the currently inspected element
        int candidateWeight = ships[p.first][p.second].perimeterWeight; //weight for the currently inspected element
        if (candidateVal < minVal) {
            finalY = p.first;
            finalX = p.second;
            minVal = candidateVal;
            minWeight = candidateWeight;
        }
        else if (candidateVal == minVal) {
            //logic for favoring nodes that are closer to the perimeter in tiebreaker scenarios (HOPEFULLY)
            if (candidateWeight < minWeight){
                finalY = p.first;
                finalX = p.second;
                minVal = candidateVal;
                minWeight = candidateWeight;
            }
        }
    }

    return make_pair(finalY, finalX);
}

/*
 * This function evaluates adjacent indices of the matrix relative to the parameter
 * currentYX. If an index is within the coordinate list parameter visited then it is
 * skipped, otherwise that index is added to the list variable candidates. This function
 * returns candidates when the process is finished, or a 1 element coordinate list containing
 * currentYX.
 *
 *     currentYX:   the coordinate that centers the adjacency search. If this is a member of candidates,
 *                  then the path is a dead end.
 *
 *     visited:     the coordinate list of elements already traversed on the current path.
 */
vector<pair<int, int>> getCandidates(pair<int, int> currentYX, const vector<pair<int, int>>& visited) {
    vector<pair<int, int>> candidates;
    for (int y = currentYX.first - 1; y <= currentYX.first + 1; y += 2) { //step through y coordinates, skipping current
        //do if this coordinate is not within visited
        if (find(visited.begin(), visited.end(), make_pair(y, currentYX.second)) == visited.end())
            candidates.emplace_back(y, currentYX.second);
    }
    for (int x = currentYX.second - 1; x <= currentYX.second + 1; x += 2) { //step through x coordinates, skipping current
        //do if this coordinate is not within visited
        if (find(visited.begin(), visited.end(), make_pair(currentYX.first, x)) == visited.end())
            candidates.emplace_back(currentYX.first, x);
    }
    return candidates.empty() ? vector<pair<int, int>>({currentYX}) : candidates; //if the enterprise got trapped,
                                                                                    //return its last location.
                                                                                    //otherwise, return candidates.
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
        cout << "No input or output filename entered. Please run the program as ./a.out <input file> <output file>" << endl; // TODO: REPLACE ./a.out WITH COMPILED PROGRAM NAME
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