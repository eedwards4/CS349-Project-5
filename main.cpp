#include <iostream>
#include <fstream>
#include <vector>
#include <tuple>
#include <algorithm>
#include "Spaceship.h"

using namespace std;

const int MAX_DURATION = 100000;

void initFiles(string& infileName, string& outfileName, ifstream& infile, ofstream& outfile, int argc, char* argv []);
int templateMatcher(const vector<Spaceship>& templates, char k);
int escapeDuration(const vector<vector<Spaceship>>& ships, pair<int, int> currentYX);
pair<int, int> chooseNeighbor(pair<int, int> currentYX, const vector<pair<int, int>>& visited, const vector<vector<Spaceship>>& ships);
bool withinPerimeter(const vector<vector<Spaceship>>& ships, pair<int, int> currentYX);
vector<pair<int, int>> getCandidates(pair<int, int> currentYX, const vector<pair<int, int>>& visited, const vector<vector<Spaceship>>& ships);

int main(int argc, char** argv) {
    ifstream infile;
    ofstream outfile;
    string ifName, ofName, line;
    char kClass, kShip;
    int cases, numShips, kValue, x, y;
    vector<Spaceship> templates = { Spaceship(0, 'E') }; //The enterprise template always exists
    vector<vector<Spaceship>> ships;
    pair<int, int> startYX;

    initFiles(ifName, ofName, infile, outfile, argc, argv);

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
                ships.at(v).emplace_back(templateMatcher(templates, kShip), kShip);
                // TODO: THIS IS WHERE THE SHIPS SHOULD GET THEIR WEIGHTS, DEPENDANT ON X/Y COORDS
            }
            infile.ignore(); //skip newline
        }

        //calculate and output duration
        outfile << escapeDuration(ships, startYX) << endl;

        templates.clear();
        ships.clear();
        templates.emplace_back(0, 'E'); //Add the Enterprise Spaceship template
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

bool withinPerimeter(const vector<vector<Spaceship>>& ships, pair<int, int> currentYX) {
    return currentYX.first > 0
        && currentYX.first < ships.size() - 1
        && currentYX.second > 0
        && currentYX.second < ships[0].size() - 1;
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
    return candidates;
}

pair<int, int> chooseNeighbor(pair<int, int> currentYX, const vector<pair<int, int>>& visited, const vector<vector<Spaceship>>& ships) {
    int minVal = MAX_DURATION, minWeight = INT_MAX;
    int finalY, finalX;

    for (pair p : getCandidates(currentYX, visited, ships)) {
        int candidateVal = ships[p.first][p.second].value;
        int candidateWeight = ships[p.first][p.second].perimeterDist;
        if (candidateVal < minVal) {
            finalY = p.first;
            finalX = p.second;
            minVal = candidateVal;
            minWeight = candidateWeight;
        }
        else if (candidateVal == minVal) {
            //logic for favoring nodes that are closer to the perimeter in tiebreaker scenarios (HOPEFULLY)
            if (candidateWeight < minWeight){
                finalX = p.first;
                finalY = p.second;
                minVal = candidateVal;
            }
        }
    }

    return make_pair(finalY, finalX);
}

int escapeDuration(const vector<vector<Spaceship>>& ships, pair<int, int> currentYX) {
    vector<pair<int, int>> visited = { currentYX }; //visited coordinates
    pair<int, int> chosen; //coordinate of the next ship to traverse
    int sum = 0;

    while (withinPerimeter(ships, currentYX)) {
        chosen = chooseNeighbor(currentYX, visited, ships);
        if (chosen == currentYX) return -1; //debug line, code failed if a new coordinate isn't chosen
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
void initFiles(string& infileName, string& outfileName, ifstream& infile, ofstream& outfile, int argc, char* argv []) {
    //string dirPath = "inputOutputData/";
    //infileName = outfileName = dirPath;

    if (argc == 3) { //use passed arguments if user gave both input and output.
        infileName.append(argv[1]);
        outfileName.append(argv[2]);
        infile.open(argv[1]);
        outfile.open(argv[2]);
    }
    else if (argc == 2) { //user passed one argument, confirm they wish to use it as an input file
        char yesno = '\0';
        infileName = argv[1];
        cout << "Found " << infileName << " for input file. Continue using " << infileName << " for input? (y/n) ";
        cin >> yesno;
        if (yesno == 'n') {
            infileName.clear();
            cout << "Enter the new input filename: ";
            cin >> infileName;
        }
        infile.open(infileName);
    }
    while (!outfile.is_open()) { //request an output file from the user if there is none available
        outfileName.clear();
        cout << "Please provide an existing filename to overwrite, or a new filename to create: ";
        cin >> outfileName;
        outfile = ofstream(outfileName, ios::out); /*invoking the ofstream constructor will create a
                                                                       new file if one does not already exist.*/
    }
    while (!infile.is_open() && infileName != "##") { //request an input file from the user if there is none available
        infileName.clear();
        cout << "No input file was found. Please provide a relative path and filename for an input file "
             << "(enter ## to close the program): ";
        cin >> infileName;
        infile.open(infileName);
    }
}
