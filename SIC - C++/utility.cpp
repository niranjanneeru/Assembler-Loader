# include <unordered_map>
# include <iostream>
# include<fstream>
# include <sstream>
# include <vector>
# include <cmath>

using namespace std;

unordered_map<string, int> symbolTable;
unordered_map<string, string> OPTAB;

vector<string> tokenize(string fileLine) {
    stringstream ss(fileLine);
    string word;
    vector<string> lineArray;
    while (ss >> word) {
        lineArray.push_back(word);
    }
    return lineArray;
}

unordered_map<string, string> loadOPTAB() {
//    unordered_map<string, string> OPTAB;
    ifstream OPTABFILE;
    OPTABFILE.open("optab.txt");
    string fileline;
    vector<string> lineArray;
    while (getline(OPTABFILE, fileline)) {
        lineArray = tokenize(fileline);
        OPTAB[lineArray[0]] = lineArray[1];
    }
    return OPTAB;
}