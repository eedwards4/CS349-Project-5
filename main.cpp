#include <iostream>
#include <fstream>
#include <vector>
#include <tuple>
#include <algorithm>
#include "Spaceship.h"

using namespace std;

const int MAX_DURATION = 1000000;

void initFiles(string& infileName, string& outfileName, ifstream& infile, ofstream& outfile, int argc, char* argv []);
int templateMatcher(const vector<Spaceship>& templates, char k);
int escapeDuration(const vector<vector<Spaceship>>& ships, tuple<int, int> currentXY);
bool withinPerimeter(const vector<vector<Spaceship>>& ships, tuple<int, int> currentXY);

int main(int argc, char** argv) {
    ifstream infile;
    ofstream outfile;
    string ifName, ofName, line;
    char kClass, kShip;
    int cases, numShips, kValue, x, y;
    vector<Spaceship> templates = { Spaceship(0, 'E') }; //The enterprise template always exists
    vector<vector<Spaceship>> ships;
    tuple<int, int> startXY;

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
        for (int u = 0; u < y; u++){
            ships.emplace_back();
            for (int v = 0; v < x; v++){
                infile >> kShip;
                if (kShip == 'E') startXY = make_tuple(u, v);
                ships.at(u).emplace_back(templateMatcher(templates, kShip), kShip);
            }
            infile.ignore(); //skip newline
        }

        //calculate and output duration
        outfile << escapeDuration(ships, startXY) << endl;
    }

    infile.close();
    outfile.close();
    return 0;
}

// Converts chars to ints based on templates stored in "templates"
int templateMatcher(const vector<Spaceship>& templates, char k){
    for (Spaceship s : templates) {
        if (k == s.klass) return s.value;
    }
    return -1;
}

bool withinPerimeter(const vector<vector<Spaceship>>& ships, tuple<int, int> currentXY) {
    return get<0>(currentXY) > 0
        && get<0>(currentXY) < ships.size() - 1
        && get<1>(currentXY) > 0
        && get<1>(currentXY) < ships[0].size();
}

int escapeDuration(const vector<vector<Spaceship>>& ships, tuple<int, int> currentXY) {
    vector<tuple<int, int>> visited = { currentXY };
    tuple<int, int> chosen = currentXY;
    int sum = 0, least = MAX_DURATION;

    while (withinPerimeter(ships, currentXY)) {
        //following for loop inspects adjacent coordinates of ships matrix
        for (int x = get<0>(currentXY) - 1; x <= get<0>(currentXY) + 1; x++) { //step through x coordinates
            for (int y = get<1>(currentXY) - 1; y <= get<1>(currentXY) + 1; y++) { //step through y coordinates
                //do if this coordinate is not within visited
                if (find(visited.begin(), visited.end(), make_tuple(x, y)) == visited.end())
                    if (ships[x][y].value < least) {
                        chosen = make_tuple(x, y);
                        least = ships[x][y].value;
                    }
            }
        }
        if (chosen == currentXY) return -1; //debug line, code failed.
        visited.push_back(chosen);
        currentXY = chosen;
        sum += least;
        least = MAX_DURATION;
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
