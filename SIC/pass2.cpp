# include "pass1.cpp"

struct instruction {
    int addr;
    string opcode;
    string operand;
    bool isOPCode;
    bool isIndexedAddr;
};

void process_instruction(instruction *pInstruction, ofstream *pOfstream);

const string &dec_hex(int data);

string char_dec(const string &basicString);

void parseIntermediateFile(string fileName) {
    ifstream intermediateFile;
    ofstream tempFile;
    intermediateFile.open("intermediate" + fileName);
    if (!intermediateFile) {
        cout << "Unable to open file: intermediate" << fileName << endl;
        exit(1);
    }
    tempFile.open("temp" + fileName);
    if (!tempFile) {
        cout << "Unable to open file: intermediate" << fileName << endl;
        exit(1);
    }
    string fileline;
    vector<string> lineArray;
    while (getline(intermediateFile, fileline)) {
        lineArray = tokenize(fileline);
        instruction instr;
        instr.addr = stoi(lineArray[0]);
        instr.opcode = lineArray[1];
        instr.isOPCode = (OPTAB.find(instr.opcode) != OPTAB.end());
        if (lineArray.size() > 2) {
            instr.operand = lineArray[2];
            instr.isIndexedAddr = (instr.operand[instr.operand.length() - 2] == ',');
        } else {
            if (instr.isOPCode) {
                // RSUB Case
                instr.operand = "0000";
                instr.isIndexedAddr = false;
            }
        }
        process_instruction(&instr, &tempFile);
    }
    intermediateFile.close();
    tempFile.close();
}

void process_instruction(instruction *pInstruction, ofstream *pOfstream) {
    if (pInstruction->isOPCode) {
        if (pInstruction->opcode == "RSUB") {
            (*pOfstream) << pInstruction->addr << "\t" << OPTAB[pInstruction->opcode] << "0000" << endl;
            return;
        }
        int operand = symbolTable[pInstruction->operand]; // TODO Error
        if (pInstruction->isIndexedAddr) {
            operand += 8000;
        }
        string op = to_string(operand);
        if (op.length() != 4) {
            string a = string(4 - op.length(), '0');
            op = a + op;
        }
        (*pOfstream) << pInstruction->addr << "\t" << OPTAB[pInstruction->opcode] << op << endl;
    } else {
        if (pInstruction->opcode == "WORD") {
            int data = stoi(pInstruction->operand);
            (*pOfstream) << pInstruction->addr << "\t" << dec_hex(data) << endl;
        } else if (pInstruction->opcode == "BYTE") {
            if (pInstruction->operand[0] == 'C') {
                (*pOfstream) << pInstruction->addr << "\t" << char_dec(pInstruction->operand) << endl;
            } else {
                (*pOfstream) << pInstruction->addr << "\t"
                             << pInstruction->operand.substr(2, pInstruction->operand.length() - 3) << endl;
            }
        }
    }
}

string char_dec(const string &basicString) {
    string res;
    for (char c:basicString) {
        int d = (unsigned char) c;
        stringstream ss;
        ss << hex << d;
        res.append(ss.str());
    }
    return res;
}

const string &dec_hex(int data) {
    stringstream ss;
    ss << hex << data;
    auto *res = new string(ss.str());
    if (res->length() != 6) {
        string stuff(6 - res->length(), '0');
        *res = stuff + *res;
    }
    // TODO Handle Error
    return *res;
}

void pass2(string fileName) {
    OPTAB = loadOPTAB();
    parseIntermediateFile(fileName);
}