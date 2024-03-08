#include <iostream>
#include <string>
#include <vector>

using namespace std;

class State;
const string EMPTY_TAPE = "#";

enum Direction {
    LEFT,
    RIGHT
};

class Transition{
    public:
        Direction dir;
        State* toState;
        char readChar;
        char writeChar;
};

class State{
    public:
        State(bool accepting, string stateName);
        vector<Transition> transitions;
        bool accepting = false;
        string name;
};

State::State(bool accepting, string stateName){
    this->accepting = accepting;
    this->name = stateName;
}

class TuringMachine{//works deterministic
    public:
        TuringMachine(State* startingState);
        void print();
        bool isAccepting(string inputWord);
        State* executeStep(State* currentState);//returns the next state or nullptr if none is found, executes one transition.

    private:
        string tape = EMPTY_TAPE;
        int headPosition = 0;
        State* startingState = nullptr;
};

TuringMachine::TuringMachine(State* startingState){
    this->startingState = startingState;
}

void TuringMachine::print(){
    for(int i = 0; i < this->tape.length(); i++){

        if(i == this->headPosition){
            cout << '[';
        }
        cout << this->tape.at(i);

        if(i == this->headPosition){
            cout << ']';
        }
    }
    cout << endl;
}

State* TuringMachine::executeStep(State* currentState){
    
    if(currentState == nullptr){
        cerr << "current state is null" << endl;
        return nullptr;
    }
    for(Transition t : currentState->transitions){
            
        if(t.readChar == this->tape.at(this->headPosition)){
            this->tape.at(headPosition) = t.writeChar;
            headPosition += (t.dir == LEFT)? -1 : 1;

            if(this->headPosition >= this->tape.length() || this->headPosition < 0){
                cerr << "head is outside the tape!" << endl;
                return nullptr;
            }
            return t.toState;
        }
    }
    cerr << "no transitions found, the turing machine halts" << endl;
    return nullptr;
}

bool TuringMachine::isAccepting(string inputWord){
    this->tape = EMPTY_TAPE + inputWord + EMPTY_TAPE;
    this->headPosition = EMPTY_TAPE.length();//start reading at the start of the word
    State* currentState = this->startingState;

    while(currentState != nullptr && !currentState->accepting){
        cout << "State: \"" << currentState->name << "\" ";
        this->print();
        currentState = this->executeStep(currentState);
    }
    return currentState != nullptr;
}

void inputTransitions(vector<State*> states){
    char c;

    if(states.empty()){
        cerr << "there are no states" << endl;
        return;
    }
    do{
        if(cin.fail()){
            return;
        }
        cout << "enter a transition rule in the form d(q0 , a) = (q1 , b , R) (meaning from state q0 to state q1 with readlabel a, write label b, direction: right)";
        
        char d, openBracket, comma, readChar, closeBracket, equals, writeChar, direction;
        string fromStateName, toStateName;
        cin >> d >> openBracket >> fromStateName >> comma >> readChar >> closeBracket >> equals >> openBracket >> toStateName >> comma >> writeChar >> comma >> direction >> closeBracket;

        if(d != 'd' || openBracket != '(' || closeBracket != ')' || equals != '=' || comma != ','){
            cerr << "expected example transition: d(q0 , a) = (q1 , a , R) note the spaces" << endl;
            continue;
        }
        Transition t;
        State* fromState = nullptr;

        for(State* s : states){

            if(s->name == fromStateName){
                fromState = s;
                break;
            }
        }
        if(fromState == nullptr){
            cerr << "unknown fromstate name: \"" << fromStateName << "\"" << endl;
            continue;
        }
        for(State* s : states){
            if(s->name == toStateName){
                t.toState = s;
                break;
            }
        }
        if(t.toState == nullptr){
            cerr << "unknown tostate name: \"" << toStateName << "\"" << endl;
            continue;
        }
        t.readChar = readChar;
        t.writeChar = writeChar;
        
        if(direction == 'R' || direction == 'r'){
            t.dir = RIGHT;
        }
        else{
            t.dir = LEFT;
        }
        fromState->transitions.push_back(t);
        cout << "Do you want to enter more transitions? (y/n)" << endl;
        cin >> c;
    }while(c != 'n' && c != 'N');
}


vector<State*> inputStates(){
    char c;
    vector<State*> result;

    do{
        cout << "Enter a state name:";
        string stateName;
        cin >> stateName;
        
        for(State* s : result){
            if(s->name == stateName){
                cerr << "State :\"" << s << "\" is already entered" << endl;
                break;
            }
        }
        cout << "Is the state accepting (aka a final state) (y/n)?";
        cin >> c;
        result.push_back(new State((c == 'y' || c == 'Y'), stateName));
        cout << "Do you want to enter more states? (y/n)" << endl;
        cin >> c;
    }while(c == 'y' || c == 'Y');
    return result;
}

int main(){
    vector<State*> states = inputStates();
    inputTransitions(states);

    cout << "Enter a starting state:";
    string stateName;
    cin >> stateName;
    State* startingState = nullptr;

    for(State* s : states){

        if(s->name == stateName){
            startingState = s;
            break;
        }
    }
    if(startingState == nullptr){
        cerr << "The startingstate: \"" << startingState <<  "\" does not exist" << endl;
        return -1;
    }
    TuringMachine tm(startingState);
    char c = 'y';

    do{
        cout << "Enter a word:" << std::endl;
        string inputWord;
        cin >> inputWord;

        cout << ((tm.isAccepting(inputWord))? "Yes, accepted" : "No, rejected") << endl;
        cout << "Do you want to enter another word? (y/n)";
        cin >> c;
    }while(c == 'y' || c == 'Y');
    
    for(State* s : states){
        delete s;
    }
    return 0;
}