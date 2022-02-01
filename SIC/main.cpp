
# include "pass2.cpp"

using namespace std;

int main() {
    OPTAB = loadOPTAB(); // Load OP Table
    string fileline;
    vector<string> lineArray;
    string fileName = "test.asm";
    sourceFile.open(fileName);
    if (!sourceFile) {
        cout << "Unable to open file: " << fileName << endl;
        exit(1);
    }
    intermediateFile.open("intermediate" + fileName);
    if (!intermediateFile) {
        cout << "Unable to open file: intermediate" << fileName << endl;
        exit(1);
    }
    errorFile.open("error" + fileName);
    if (!errorFile) {
        cout << "Unable to open file: error" << fileName << endl;
        exit(1);
    }
    while (getline(sourceFile, fileline)) {
        lineArray = tokenize(fileline);
        pass1(lineArray);
    }
    sourceFile.close();
    intermediateFile.close();
    errorFile.close();
    // TODO STORE SYMBOL TABLE
    pass2(fileName);
    return 0;
}