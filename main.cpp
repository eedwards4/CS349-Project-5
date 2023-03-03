#include <iostream>
#include <fstream>
#include <vector>
#include "Klingon.h"

using namespace std;

void initFiles(string& infileName, string& outfileName, ifstream& infile, ofstream& outfile, int argc, char* argv []);
int klingonKonverter(char k);

int main(int argc, char** argv) {
    ifstream infile;
    ofstream outfile;
    string ifName, ofName, line; char kClass, kShip;
    int cases, numShips, x, y;
    vector<Klingon> templates; vector<vector<Klingon>> ships;
    initFiles(ifName, ofName, infile, outfile, argc, argv);

    getline(infile, line);
    cases = stoi(line);
    for (int i = 0; i < cases; i++){
        getline(infile, line);
        numShips = stoi(line.substr(0, line.find(' ')));
        x = stoi(line.substr(0, line.find(' '))); // split x
        line.erase(0, line.find(' ') + 1);
        y = stoi(line.substr(0, line.find(' '))); // split y
        for (int j = 0; j < numShips; j++) {
            getline(infile, line);
            kClass = line.substr(0, line.find(' '))[0];
            line.erase(0, line.find(' ') + 1);
            templates.emplace_back(stoi(line.substr(0, line.size())), kClass);
        }
        for (int j = 0; j < y; j++){
            ships.emplace_back();
            for (int k = 0; k < x; k++){
                infile >> kShip;
                if (kShip == 'E'){k++;}
                else{ships.at(j).emplace_back(klingonKonverter(kShip), kShip);}
            }
        }
    }
}

// Converts chars to ints based on templates stored in "templates"
int klingonKonverter(char k){

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
