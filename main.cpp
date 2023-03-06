#include <iostream>
#include <fstream>
#include <vector>
#include <tuple>
#include <algorithm>
#include "Spaceship.h"

using namespace std;

const int MAX_DURATION = 100000;

void initFiles(ifstream& infile, ofstream& outfile, int argc, char* argv[]);
int templateMatcher(const vector<Spaceship>& templates, char k);
int weightMatcher(int colSize, int rowSize, int colPos, int rowPos);
int escapeDuration(const vector<vector<Spaceship>>& ships, pair<int, int> currentYX);
pair<int, int> chooseNeighbor(pair<int, int> currentYX, const vector<pair<int, int>>& visited, const vector<vector<Spaceship>>& ships);
bool withinPerimeter(const vector<vector<Spaceship>>& ships, pair<int, int> currentYX);
vector<pair<int, int>> getCandidates(pair<int, int> currentYX, const vector<pair<int, int>>& visited, const vector<vector<Spaceship>>& ships);

int main(int argc, char** argv) {
    ifstream infile;
    ofstream outfile;
    string line;
    char kClass, kShip;
    int cases, numShips, kValue, x, y;
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
            x = stoi(line.substr(0, line.find(' '))); // split x
            line.erase(0, line.find(' ') + 1);
            y = stoi(line.substr(0, line.find(' '))); // split y
            for (int j = 0; j < numShips; j++) {
                getline(infile, line);
                kClass = line.substr(0, line.find(' '))[0];
                line.erase(0, line.find(' ') + 1);
                kValue = stoi(line.substr(0, line.size()));
                templates.emplace_back(kValue, kClass);
            }
            for (int v = 0; v < y; v++){ //lines are read into ship by y value, then by x value; coordinates paired in
                                         //pairs of {y, x} from here on.
                ships.emplace_back();
                for (int u = 0; u < x; u++){
                    infile >> kShip;
                    if (kShip == 'E') startYX = make_pair(v, u);
                    ships.at(v).emplace_back(templateMatcher(templates, kShip), //coord value from class
                                                weightMatcher(y, x, v, u), //coord weight
                                                kShip); //coord class
                }
                infile.ignore(); //skip newline
            }

            //calculate and output duration
            outfile << escapeDuration(ships, startYX) << endl;

            templates.clear();
            ships.clear();
            templates.emplace_back(0, 'E'); //Add the Enterprise Spaceship template
        }
    }

    infile.close();
    outfile.close();
    return 0;
}

// Converts chars to ints based on templates stored in "templates"
int templateMatcher(const vector<Spaceship>& templates, char k){
    for (Spaceship s : templates) {
        if (k == s.shipClass) return s.value;
    }
    return -1;
}

int weightMatcher(int colSize, int rowSize, int colPos, int rowPos) {
    int colWeight, rowWeight;
    if (colPos >= colSize / 2) {
        colWeight = colSize - colPos;
    }
    else {
        colWeight = colPos + 1;
    }
    if (rowPos >= rowSize / 2) {
        rowWeight = rowSize - rowPos;
    }
    else {
        rowWeight = rowPos + 1;
    }
    return min(colWeight, rowWeight);
}

vector<pair<int, int>> getCandidates(pair<int, int> currentYX, const vector<pair<int, int>>& visited, const vector<vector<Spaceship>>& ships) {
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
    return candidates.size() > 0 ? candidates : vector<pair<int, int>>({currentYX}); //if the enterprise got trapped,
                                                                                       //return its last location.
                                                                                       //otherwise, return candidates.
}

pair<int, int> chooseNeighbor(pair<int, int> currentYX, const vector<pair<int, int>>& visited, const vector<vector<Spaceship>>& ships) {
    int minVal = MAX_DURATION, minWeight = INT_MAX;
    int finalY, finalX;

    for (pair p : getCandidates(currentYX, visited, ships)) {
        int candidateVal = ships[p.first][p.second].value;
        int candidateWeight = ships[p.first][p.second].perimeterWeight;
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

int escapeDuration(const vector<vector<Spaceship>>& ships, pair<int, int> currentYX) {
    vector<pair<int, int>> visited = { currentYX }; //visited coordinates
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