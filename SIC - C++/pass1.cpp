# include "utility.cpp"

ifstream sourceFile;
ofstream intermediateFile, errorFile;

void handleStart(const string &, int addr = 0);

void handleInstruction(vector<string>);

void handleSymbol(vector<string> inp);

bool handleAssemblyDirective(vector<string> inp);

void handleEnd(int inp);

int locctr;
int startAddr;

void pass1(vector<string> inp) {
    if (OPTAB.find(inp[0]) != OPTAB.end()) {
        // OPCODE without Label
        handleInstruction(inp);
    } else {
        // Handle Start without Program Name
        if (inp[0] == "START") {
            if (inp.size() == 2) {
                handleStart("", stoi(inp[1]));
            } else {
                handleStart("");
            }
        } else if (inp.size() > 1 && inp[1] == "START") { // Handle Start with Program Name
            if (inp.size() == 3) {
                handleStart(inp[0], stoi(inp[2]));
            } else {
                handleStart(inp[0]);
            }
        } else {
            if (inp[0] == "END") {
                inp.size() > 1 ? handleEnd(stoi(inp[1])) : handleEnd(startAddr);
            } else if (inp.size() > 1) {
                handleSymbol(inp);
            } else {
                errorFile << "ERROR: " << locctr << ": Invalid Instruction " << inp[0] << endl;
            }
        }
    }
}

void handleEnd(int inp) {
    intermediateFile << locctr << "\tEND\t" << inp;
}

void handleSymbol(vector<string> inp) {
    if (symbolTable.find(inp[0]) == symbolTable.end()) {
        symbolTable[inp[0]] = locctr;
        if (OPTAB.find(inp[1]) != OPTAB.end()) {
            vector<string> instr;
            instr.push_back(inp[1]);
            if (inp.size() > 2) {
                instr.push_back(inp[2]);
            }
            handleInstruction(instr);
        } else if (!(handleAssemblyDirective(inp))) {
            errorFile << "ERROR: " << locctr << ": Invalid Keyword " << inp[0] << " " << inp[1] << endl;
        }
    } else {
        errorFile << "ERROR: " << locctr << ": Symbol " << inp[0] << " Already Declared at " << symbolTable[inp[0]]
                  << endl;
    }
}

bool handleAssemblyDirective(vector<string> inp) {
    if (inp[1] == "WORD") {
        intermediateFile << locctr << "\tWORD\t" << inp[2] << endl;
        locctr += 3;
        return true;
    } else if (inp[1] == "BYTE") {
        intermediateFile << locctr << "\tBYTE\t" << inp[2] << endl;
        locctr += (inp[2][0] == 'C') ? ((int) inp[2].length() - 3) : (ceil(((float) inp[2].length() - 3) / 2));
        return true;
    } else if (inp[1] == "RESW") {
        intermediateFile << locctr << "\tRESW\t" << inp[2] << endl;
        locctr += (stoi(inp[2]) * 3);
        return true;
    } else if (inp[1] == "RESB") {
        intermediateFile << locctr << "\tRESB\t" << inp[2] << endl;
        locctr += stoi(inp[2]);
        return true;
    } else if (inp[1] == "END") {
        inp.size() > 2 ? handleEnd(stoi(inp[2])) : handleEnd(startAddr);
        return true;
    }
    return false;
}

void handleInstruction(vector<string> instr) {
    intermediateFile << locctr << "\t" << instr[0];
    if (instr.size() > 1) {
        intermediateFile << "\t" << instr[1];
    }
    intermediateFile << endl;
    locctr += 3;
}

void handleStart(const string &programName, int addr) {
    locctr = addr;
    startAddr = locctr;
    intermediateFile << locctr << "\tSTART\t" << addr << endl;
    // TODO Store in Symbol Table
}
