#include <fstream>
#include <iostream>
#include <sstream>
#include <stack>
#include <map>
#include <stdint.h>
#include <string.h>
#include <assert.h>

using namespace std;

int main(int argc, char* argv[]){
    std::string fileName;

    if(argc < 2){
        cout << "enter a file name:";
        cin >> fileName;
    }
    else{
        fileName = string(argv[1]);
    }
    ifstream file(fileName, ios::in);
    const size_t TAPE_SIZE = 256 * 1024 * 1024;
    
    uint8_t* tape = new uint8_t[TAPE_SIZE];
    memset(tape, 0, TAPE_SIZE);
    stringstream code;
    code << file.rdbuf(); 

    uint32_t tapePtr = 0;
    char opCode;
    stack<streampos> callStack;
    map<streampos, streampos> brackets;

    while(code >> opCode){

        switch(opCode){
            case '>':
                tapePtr++;
                tapePtr %= TAPE_SIZE;
                break;
            case '<':
                tapePtr--;
                tapePtr %= TAPE_SIZE;
                break;
            case '+':
                tape[tapePtr]++;
                break;
            case '-':
                tape[tapePtr]--;
                break;
            case ',':{
                char input = 0;
                cin >> input;
                tape[tapePtr] = (uint8_t)input;
                break;
            }
            case '.':
                cout << (char)tape[tapePtr];
                break;
            case ']':{
                if(callStack.empty()){
                    cerr << "Unexpected \"]\" at character: " << code.tellg() - (streampos)1 << endl;
                    return -1;
                }
                brackets.insert({callStack.top(), code.tellg()});

                if(tape[tapePtr] == 0){
                    callStack.pop();
                    break;
                }
                code.seekg(callStack.top(), std::ios_base::beg);
                break;
            }
            case '[':{
                if(tape[tapePtr] != 0){
                    callStack.push(code.tellg());
                    break;
                }
                if(brackets.count(code.tellg()) > 0){
                    code.seekg(brackets[code.tellg()], std::ios_base::beg);
                    break;
                }        

                unsigned int nestCount = 1;
                do{
                    code >> opCode;

                    if(opCode == ']'){
                        nestCount--;
                    }
                    else if(opCode == '['){
                        nestCount++;
                    }
                }while(opCode != ']' || nestCount > 0);
            }
            default:
                break;
        }
    }
    if(!callStack.empty()){
        cerr << "Missing \"]\" for \"[\" at character: " << callStack.top() - (streampos)1 << endl;
    }
    delete[] tape;
    file.close();
    return 0;
}
