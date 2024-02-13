#include <iostream>
#include <vector>
#include <string>
#include <queue>
#include <stack>
#include <set>

using namespace std;

class State;

class Transition{
    public:
        char label;
        State* from = nullptr;
        State* to = nullptr;
};

class State{
    public:
        State(bool accepting, string name);
        bool isAccepted(string s, queue<Transition>& workQueue, stack<Transition>& transitionSteps);
        string name;

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


bool isDeterministic(vector<State*> states){
    
    if(states.empty()){
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
            t.label = 0x0;
        }
        else{
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
                cout << "[-" << t.label << "> " << t.to->name << "] ";
            }
        }
        cout << endl;
    }
}

int main(){
    cout << "enter a starting state:";
    string stateName;
    cin >> stateName;
    cout << "Is the state accepting (aka a final state) (y/n)?";
    char c;
    cin >> c;

    State startingState = (c == 'y' || c =='Y')? State(true, stateName) : State(false, stateName);

    vector<State*> states = inputStates();
    states.push_back(&startingState);
    cout << "the states are:" << endl;
    printStates(states);
    inputTransitions(states);
    cout << "states and transitions: " << endl;
    printStates(states);

    if(isDeterministic(states)){
        cout << "the atomaton is deterministic" << endl;
    }
    else{
        cout << "the atomaton is non-deterministic" << endl;
    }

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
        if(startingState.isAccepted(testString, workQueue, transitionSteps)){
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
