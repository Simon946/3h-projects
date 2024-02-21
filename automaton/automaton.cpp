#include <iostream>
#include <vector>
#include <string>
#include <queue>
#include <stack>
#include <set>
#include <assert.h>

using namespace std;

const char LAMBDA = 0x0;
const string REGEX_EMPTY = "0";
const string REGEX_LAMBDA = "L";

class State;
vector<State*> inputStates();
void inputTransitions(vector<State*> states);
void printStates(vector<State*> states);

class Transition{
    public:
        Transition(State* from, char label, State* to);
        Transition(){};
        char label = LAMBDA;
        string regex;
        State* from = nullptr;
        State* to = nullptr;
};

Transition::Transition(State* from, char label, State* to){
    this->from = from;
    this->label = label;
    this->regex = (label == LAMBDA)? REGEX_LAMBDA : string(1, label);
    this->to = to;
}


class State{
    public:
        State(bool accepting, string name);
        bool isAccepted(string s, queue<Transition>& workQueue, stack<Transition>& transitionSteps);
        string name;
        string getLoopRegex();

        vector<Transition>transitions;
        bool accepting = false;
};

State::State(bool accepting, string name){
    this->accepting = accepting;
    this->name = name;
}

bool State::isAccepted(string s, queue<Transition>& workQueue, stack<Transition>& transitionSteps){

    if(s.empty()){
        return this->accepting;
    }

    for(Transition t : transitions){
        if(t.label == s.front() || t.label == 0x0){//empty labels allowed for lambda transitions.
            workQueue.push(t);
        }
    }

    while(!workQueue.empty()){
        Transition t = workQueue.back();
        workQueue.pop();
       
        s.erase(0,1);
        transitionSteps.push(t);
        if(t.to->isAccepted(s, workQueue, transitionSteps)){
            return true;
        }
    }
    return false;
}

string State::getLoopRegex(){
    string regex = "";

    for(Transition t : this->transitions){

        if(t.from == t.to && t.from == this){
            if(regex.empty()){
                regex = t.regex;
            }
            else{
                regex = "(" + regex + "+" + t.regex + ")";
            }
        }
    }
    return (regex.empty())? REGEX_EMPTY : regex;
}

class Automaton{
    public:
        Automaton();
        bool isDeterministic();
        bool isAccepted(string s, queue<Transition>& workQueue, stack<Transition>& transitionSteps);
        string toRegex();
        void mergeTransitions();
        vector<Transition> incomingStates(State* state);
        vector<Transition> outgoingStates(State* state);
        void addTransition(Transition t);
        void removeState(State* s);
    private:
        vector<State*> states;//also includes the startingstate
        State* startingState = nullptr;
};

Automaton::Automaton(){
    this->states = inputStates();

    cout << "enter a starting state name:";
    string stateName;
    cin >> stateName;
    
    for(State* s : this->states){
        if(s->name == stateName){
            cerr << "state :" << s << " is already entered" << endl;
            break;
        }
    }
    cout << "Is the state accepting (aka a final state) (y/n)?";
    char c;
    cin >> c;
    this->startingState = (c == 'y' || c =='Y')? new State(true, stateName) : new State(false, stateName);
    this->states.push_back(startingState);
    cout << "all the states are:" << endl;
    printStates(this->states); 
    inputTransitions(this->states);
}

bool Automaton::isDeterministic(){
    if(this->states.empty()){
        return true;
    }
    int transitionCount = states.front()->transitions.size();

    set<char> labels;

    for(Transition t : states.front()->transitions){
        char label = t.label;

        if(labels.count(label)){
            return false;
        }
        labels.insert(label);
    }

    for(State* s : states){
        
        if(s->transitions.size() != transitionCount){
            return false;
        }

        for(Transition t : s->transitions){
            
            if(labels.count(t.label) == 0 || t.label == 0x0){
                return false;
            }
        }
    }
    return true;

}

bool Automaton::isAccepted(string s, queue<Transition>& workQueue, stack<Transition>& transitionSteps){
    if(s.empty()){
        return this->startingState->accepting;
    }

    for(Transition t : this->startingState->transitions){
        if(t.label == s.front() || t.label == LAMBDA){//empty labels allowed for lambda transitions.
            workQueue.push(t);
        }
    }

    while(!workQueue.empty()){
        Transition t = workQueue.back();
        workQueue.pop();
       
        s.erase(0,1);
        transitionSteps.push(t);
        if(t.to->isAccepted(s, workQueue, transitionSteps)){
            return true;
        }
    }
    return false;
}

string Automaton::toRegex(){
    std::vector<State*> finalStates;

    for(State* s : this->states){

        if(s->accepting){
            finalStates.push_back(s);
        }
    }
    assert(!finalStates.empty());
    State* finalState = finalStates.back();

    if(finalStates.size() > 1){
        State* newFinal = new State(false, "new final state");

        for(State* s : finalStates){
            s->transitions.push_back(Transition(s, LAMBDA, newFinal));
            s->accepting = false;
        }
        finalState = newFinal;
    }

    while(this->states.size() >= 2){
        printStates(this->states);
        cout << "now merging" << endl;
        mergeTransitions();
        State* stateToDelete = nullptr;

        for(State* s : this->states){

            if(!s->accepting && s != this->startingState){
                stateToDelete = s;
                break;
            }
        }
        if(stateToDelete == nullptr){
            break;
        }
        vector<Transition> incoming = incomingStates(stateToDelete);
        vector<Transition> outgoing = outgoingStates(stateToDelete);
        string loopRegex = stateToDelete->getLoopRegex();

        for(Transition in : incoming){

            for(Transition out : outgoing){
                Transition combined = Transition(in.from, LAMBDA, out.to);

                if(loopRegex == REGEX_EMPTY){
                    combined.regex = "(" + in.regex + out.regex + ")";
                }
                else{
                    combined.regex = "(" + in.regex + loopRegex + "*" + out.regex + ")";
                }               
                addTransition(combined);
            }
        }
        removeState(stateToDelete);
    }
    mergeTransitions();

    if(startingState == finalState){
        return "(" + startingState->getLoopRegex() + "*)";
    }

    string r1 = startingState->getLoopRegex();
    string r2 = REGEX_EMPTY;
    string r3 = REGEX_EMPTY;

    for(Transition t : startingState->transitions){

        if(t.to == finalState){
            r2 = t.regex;
            break;
        }
    }
    for(Transition t : finalState->transitions){
        
        if(t.to == startingState){
            r3 = t.regex;
            break;
        }
    }
    string r4 = finalState->getLoopRegex();

    cout << "regex 1 to 4:" << endl << r1 << endl << r2 << endl << r3 << endl << r4 << endl;

    return "(" + r1 + "*" + r2 + "(" + r4 + "+" + "(" + r3 + r1 + "*" + r2 + ")*))";
}

void Automaton::mergeTransitions(){
    
    for(State* s : this->states){

        for(int i = 0; i < s->transitions.size(); i++){
            
            for(int j = 0; j < s->transitions.size(); j++){

                if(i == j){
                    continue;
                }
                if(s->transitions.at(i).from == s->transitions.at(j).from && s->transitions.at(i).to == s->transitions.at(j).to){
                    string lhsRegex = s->transitions.at(i).regex;
                    string rhsRegex = s->transitions.at(j).regex;

                    if(lhsRegex.empty() || rhsRegex.empty()){
                        cout << "regex emptyyy";
                    }
                    s->transitions.at(i).regex = "(" + lhsRegex + "+" + rhsRegex + ")";
                    s->transitions.erase(s->transitions.begin() + j);
                    j--;
                }
            }   
        }
    }
}

vector<Transition> Automaton::incomingStates(State* state){
    vector<Transition> result;

    for(State* s : this->states){
        
        for(Transition t : s->transitions){
            
            if(t.to == state){
                result.push_back(t);
            }
        }
    }
    return result;
}

vector<Transition> Automaton::outgoingStates(State* state){
    assert(state != nullptr);
    vector<Transition> result;

    for(Transition t : state->transitions){
        assert(t.from == state);
        result.push_back(t);
    }
    return result;
}

void Automaton::addTransition(Transition t){
    assert(t.from != nullptr);

    for(State * s : this->states){
        
        if(t.from == s){
            s->transitions.push_back(t);
            return;
        }
    }
}

void Automaton::removeState(State* s){
    assert(s != this->startingState);
    cout << "now removing state: " << s->name << " from the automaton" << endl;
    vector<Transition> incoming = incomingStates(s);
    
    for(Transition t : incoming){
        State* fromState = t.from;
        assert(fromState);

        for(int i = 0; i < fromState->transitions.size(); i++){
            
            if(fromState->transitions.at(i).to == s){
                fromState->transitions.erase(fromState->transitions.begin() + i);
                i--;
            }
        }
    }
    for(int i = 0; i < this->states.size(); i++){
        if(this->states.at(i) == s){
            this->states.erase(this->states.begin() + i);
            return;
        }
    }
    assert(false);//the state was not found in the automaton
}

void inputTransitions(vector<State*> states){
    char c;

    do{
        cout << "enter a transition rule in the form A -b> C (meaning from state A to state C with label b), use A -> B for lambda transition: ";
        string stateName;
        cin >> stateName;
        cout << "read statename as: " << stateName << endl;
        Transition t;
        State* fromState = nullptr;

        for(State* s : states){
            if(s->name == stateName){
                fromState = s;
                t.from = fromState;
                break;
            }
        }
        if(fromState == nullptr){
            cerr << "unknown state name: " << stateName << endl;
            continue;
        }
        cin >> c;
        if(c != '-'){
            cerr << "expected a:-" << endl;
        }
        cin >> t.label;
        
        if(t.label == '>'){
            t.label = LAMBDA;
            t.regex = REGEX_LAMBDA;
        }
        else{
            t.regex += t.label;
            cin >> c;
        
            if(c != '>'){
                cerr << "expected a:>" << endl;
            }
        }

        cin >> stateName;
        
        for(State* s : states){
            if(s->name == stateName){
                t.to = s;
                break;
            }
        }
        if(t.to == nullptr){
            cerr << "unknown state name: " << stateName << endl;
            continue;
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
        cout << "enter a state name:";
        string stateName;
        cin >> stateName;
        
        for(State* s : result){
            if(s->name == stateName){
                cerr << "state :" << s << " is already entered" << endl;
                break;
            }
        }
        cout << "Is the state accepting (aka a final state) (y/n)?";
        cin >> c;
        State* newState = (c == 'y' || c =='Y')? new State(true, stateName) : new State(false, stateName);
        result.push_back(newState);
        cout << "Do you want to enter more states? (y/n)" << endl;
        cin >> c;

    }while(c == 'y' || c == 'Y');

    return result;
}

void printStates(vector<State*> states){
    
    for(State* s : states){
        cout << s->name;
        if(s->accepting){
            cout << " (accepting) ";
        }
        if(!s->transitions.empty()){
            cout << " can go to: ";

            for(Transition t : s->transitions){
                string label = (t.label == LAMBDA)? "lambda" : string(1, t.label);

                cout << "[-" << label << "> " << t.to->name << "] ";
            }
        }
        cout << endl;
    }
}

int main(){

    Automaton aut;

    cout << "the matching regex for the automaton is: "<< aut.toRegex() << endl;

    do{
        string testString;
        cout << "enter a string that the atomaton must check:";
        cin >> testString;

        if(testString.empty()){
            break;
        }

        queue<Transition> workQueue;
        stack<Transition> transitionSteps;
        cout << testString;
        if(aut.isAccepted(testString, workQueue, transitionSteps)){
            cout << " is accepted: ";
            Transition t;

            while(!transitionSteps.empty()){
                t = transitionSteps.top();
                transitionSteps.pop();
                cout << t.to->name << " <" << t.label << "- ";
            }
            cout << t.from->name << endl;

        }   
        else{
            cout << " is not accepted" << endl;
        }
    }while(true);
    

    return 0;
}
