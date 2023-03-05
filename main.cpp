//
// Created by Joe Coon and Ethan Edwards on 3/3/23.
//
#include <algorithm>
#include <iostream>
#include <fstream>
#include <vector>
#include <tuple>
#include "Klingon.h"

using namespace std;

void initFiles(ifstream& infile, ofstream& outfile, int argc, const char* argv[]);
int escapeDuration(const vector<vector<Klingon>>& ships, tuple<int, int> currentXY);
bool withinPerimeter(const vector<vector<Klingon>>& ships, tuple<int, int> currentCoords);
int templateMatcher(const vector<Klingon>& templates, char k);

const int MAX_DURATION = 1000;

int main(int argc, const char** argv){
    // File access
    ifstream infile; ofstream outfile;
    initFiles(infile, outfile, argc, argv);
    // Variables
    string line; char kClass;
    int cases, numClasses, gridx, gridy, kVal; tuple<int, int> startCoords;
    vector<Klingon> templates {Klingon(0, 'E')}; // The Enterprise template always exists
    vector<vector<Klingon>> ships;
    // asdasdas
    getline(infile, line); // Get number of cases
    cases = stoi(line);
    for (int i = 0; i < cases; i++){
        getline(infile, line); // Get number of ship classes, grid x, and grid y for case n
        numClasses = stoi(line.substr(0, line.find(' ')));
        line.erase(0, line.find(' ') + 1);
        gridx = stoi(line.substr(0, line.find(' ')));
        line.erase(0, line.find(' ') + 1);
        gridy = stoi(line.substr(0, line.size()));
        for (int j = 0; j < numClasses; j++){
            getline(infile, line); // Get the class identifier and the class int for class n
            kClass = line.substr(0, line.find(' '))[0];
            line.erase(0, line.find(' ') + 1);
            kVal = stoi(line.substr(0, line.size()));
            templates.emplace_back(kVal, kClass); // Add a Klingon instance to the list of templates
        }
        for (int j = 0; j < gridy; j++){
            ships.emplace_back();
            for (int k = 0; k < gridx; k++){
                infile >> kClass;
                if (kClass == 'E') startCoords = make_tuple(gridx, gridy);
                ships.at(j).emplace_back(templateMatcher(templates, kClass), kClass);
            }
            infile.ignore();
        }

        //calculate and output duration
        outfile << escapeDuration(ships, startCoords) << endl;

        templates.clear();
        ships.clear();
        templates.emplace_back(0, 'E');
    }

    infile.close();
    outfile.close();
    exit(EXIT_SUCCESS);
}

// Convert char to int based on template
int templateMatcher(const vector<Klingon>& templates, char k){
    for (Klingon i : templates){
        if (k == i.shipClass) return i.value;
    }
    return -1;
}

// Check
bool withinPerimeter(const vector<vector<Klingon>>& ships, tuple<int, int> currentCoords){
    return get<0>(currentCoords) > 0 && get<0>(currentCoords) < ships.size() - 1 && get<1>(currentCoords) > 0 && get<1>(currentCoords) < ships[0].size() - 1;
}

//
int escapeDuration(const vector<vector<Klingon>>& ships, tuple<int, int> currentXY) {
    vector<tuple<int, int>> visited = { currentXY }; //visited coordinates
    tuple<int, int> chosen = currentXY; //coordinate of the next ship to traverse
    int sum = 0, least = MAX_DURATION, x, y;

    while (withinPerimeter(ships, currentXY)) {
        x = get<0>(currentXY);
        y = get<1>(currentXY);

        x--;
        while (x <= get<0>(currentXY) + 1) { //step through x coordinates
            //do if this coordinate is not within visited
            if (find(visited.begin(), visited.end(), make_tuple(x, y)) == visited.end())
                if (ships[x][y].value <= least) {
                    chosen = make_tuple(x, y);
                    least = ships[x][y].value;
                }
            x += 2; //skip looking at current x coordinate
        }

        x = get<0>(currentXY);

        y--;
        while (y <= get<1>(currentXY) + 1) { //step through y coordinates
            //do if this coordinate is not within visited
            if (find(visited.begin(), visited.end(), make_tuple(x, y)) == visited.end())
                if (ships[x][y].value <= least) {
                    chosen = make_tuple(x, y);
                    least = ships[x][y].value;
                }
            y += 2; //skip looking at current y coordinate
        }

        if (chosen == currentXY) return -1; //debug line, code failed.
        visited.push_back(chosen);
        currentXY = chosen;
        sum += least;
        least = MAX_DURATION;
    }

    return sum;
}

// File handling logic
void initFiles(ifstream& infile, ofstream& outfile, int argc, const char* argv[]){
    char confirm = 'n'; string fname;
    // Check for inputs
    if (argc == 3){
        infile.open(argv[1]);
        outfile.open(argv[2]);
    } else if (argc == 2){
        fname = argv[1];
        cout << "Found " << fname << " for input file. Continue using " << fname << " as input? (y/n) ";
        cin >> confirm;
        if (confirm == 'n'){
            cout << "Enter the new input filename: " << endl;
            cin >> fname;
        }
        infile.open(fname);
    } else{
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