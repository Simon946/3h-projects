#include <iostream>
#include <set>
#include <string>
#include "colorize.cpp"

using namespace std;

enum ExprType {
    UNION,
    INTERSECT,
    COMPLEMENT,//applies to lhs only
    DIFFERENCE,
    CONSTANT//applies to value only
};

void ignoreSpaces(){
    char c;

    while(cin.peek() == ' '){
        cin.ignore(1);
    }
}


string setToString(set<string> set);
set<string> inputSet();


class SetExpr{
    public:
        SetExpr(SetExpr* lhs, ExprType operand, SetExpr* rhs);
        SetExpr(set<string> value);
        ~SetExpr();
        SetExpr* calculate();
        string toString();
        SetExpr* lhs = nullptr;
        ExprType operand = CONSTANT;
        SetExpr* rhs = nullptr;        
        SetExpr* complement = nullptr;//only valid when type = COMPLEMENT
        set<string> value; //only valid when type == CONSTANT
};

SetExpr* calculateUnion(SetExpr* lhs, SetExpr* rhs);
SetExpr* calculateIntersect(SetExpr* lhs, SetExpr* rhs);
SetExpr* calculateDifference(SetExpr* lhs, SetExpr* rhs);

SetExpr::SetExpr(SetExpr* lhs, ExprType operand, SetExpr* rhs){
    this->lhs = lhs;
    this->operand = operand;
    this->rhs = rhs;

    if(operand == COMPLEMENT){
        if(lhs != rhs){
            cerr << "lhs must be rhs when the SetExpr is complement";
            
        }
        this->complement = lhs;
    }
    if(operand == CONSTANT){
        cerr << "cannot create new constatn like this";
    }
}

SetExpr::SetExpr(set<string> constant){
    this->operand = CONSTANT;
    this->value = constant;
}

SetExpr* load(){
    ignoreSpaces();
    char c = cin.peek();

    if(c == 'C'){
        cin.ignore(1);
        SetExpr* set = load();
        return new SetExpr(set, COMPLEMENT, set);   
    }

    switch (c){
        case '{':
            return new SetExpr(inputSet());
        case '(':{
            cin >> c;

            SetExpr* lhs = load();
            char operand;

            ignoreSpaces();
            cin >> operand;
            ExprType type;

            if(operand == 'U'){
                type = UNION;
            }
            else if(operand == 'I'){
                type = INTERSECT;
            }
            else if(operand == 'D'){
                type = DIFFERENCE;
            }
            SetExpr* rhs = load();
            ignoreSpaces();
            
            cin >> c;
            if(c != ')'){
                cerr << "unbalanced brackets" << endl;
            }
            return new SetExpr(lhs, type, rhs); 
        }
        default:
            cerr << "unexpected character: " << c << endl;
            break;
    }
    return nullptr;
}

SetExpr::~SetExpr(){
    delete lhs;
    delete rhs;
}

SetExpr* calculateUnion(SetExpr* lhs, SetExpr* rhs){

    if(lhs->operand != COMPLEMENT && rhs->operand != COMPLEMENT){
        set<string> unionSet = lhs->value;

        for(string s : rhs->value){
            unionSet.insert(s);
        }
        return new SetExpr(unionSet); 
    }

    if(lhs->operand == COMPLEMENT && rhs->operand == COMPLEMENT){
        SetExpr* inverseUnion = calculateUnion(lhs->complement, rhs->complement);
        return new SetExpr(inverseUnion, COMPLEMENT, inverseUnion);
    }
    SetExpr* complement = lhs;
    SetExpr* other = rhs;

    if(rhs->operand == COMPLEMENT){
        swap(complement, other);
    }
    return calculateDifference(complement->complement, other);
}

SetExpr* calculateIntersect(SetExpr* lhs, SetExpr* rhs){

    if(lhs->operand != COMPLEMENT && rhs->operand != COMPLEMENT){
        set<string> intersectSet;
            
        for(string s : rhs->value){
            if(lhs->value.count(s)){
                intersectSet.insert(s);
            }
        }
        return new SetExpr(intersectSet); 
    }
    if(lhs->operand == COMPLEMENT && rhs->operand == COMPLEMENT){
        SetExpr* inverseUnion = calculateUnion(lhs->complement, rhs->complement);
        return new SetExpr(inverseUnion, COMPLEMENT, inverseUnion);
    }
    SetExpr* complement = lhs;
    SetExpr* other = rhs;

    if(rhs->operand == COMPLEMENT){
        swap(complement, other);
    }
    return calculateDifference(complement->complement, other);
}

SetExpr* calculateDifference(SetExpr* lhs, SetExpr* rhs){

    if(lhs->operand != COMPLEMENT && rhs->operand != COMPLEMENT){
        set<string> differenceSet;
            
        for(string s : rhs->value){
            if(!lhs->value.count(s)){
                differenceSet.insert(s);
            }
        }
        for(string s : lhs->value){
            if(!rhs->value.count(s)){
                differenceSet.insert(s);
            }
        }
        return new SetExpr(differenceSet); 
    }
    if(lhs->operand == COMPLEMENT && rhs->operand == COMPLEMENT){
        return calculateDifference(lhs->complement, rhs->complement);
    }
    SetExpr* complement = lhs;
    SetExpr* other = rhs;

    if(rhs->operand == COMPLEMENT){
        swap(complement, other);
    }
    SetExpr* inverseDifference = calculateUnion(complement->complement, other);
    return new SetExpr(inverseDifference, COMPLEMENT, inverseDifference);   
}

SetExpr* SetExpr::calculate(){
    
    if(this->operand == CONSTANT){
        return this;
    }

    SetExpr* lhs = this->lhs->calculate();
    SetExpr* rhs = this->rhs->calculate();

    switch (this->operand){
        case UNION:{
            return calculateUnion(lhs, rhs);
        }
        case INTERSECT:{
            return calculateIntersect(lhs, rhs);
        }
        case COMPLEMENT:{
            if(lhs->operand == COMPLEMENT){
                return lhs->complement;//double complement
            }
            return this;//best I can do
        }
        case DIFFERENCE:{
            return calculateDifference(lhs, rhs);
        }
    default:
        cerr << "unknown operand: " << this->operand;
        break;
    }
    return nullptr;
}

string SetExpr::toString(){
    string str;

    if(this->operand == CONSTANT){
        return setToString(this->value);
    }
    if(this->operand == COMPLEMENT){
        str = "complement( ";
        str.append(this->complement->toString());
        str.append(" )");
        return str;
    }
    string operand = "?";
    switch (this->operand){
        case UNION:{
            operand = "union";
            break;
        }
        case INTERSECT:{
            operand = "intersect";
            break;
        }
        case DIFFERENCE:{
            operand = "difference";
            break;
        }
        default:{
            cerr << "unknown operand: " << this->operand;
            return "";
        }
    }
    str = this->lhs->toString();
    str.append(" ");
    str.append(operand);
    str.append(" ");
    str.append(this->rhs->toString());
    return str;
}

string setToString(set<string> set){
    string str = "{";  
    
    for(string s : set){
        str.append(s);
        str.append(", "); 
    }
    if(!set.empty()){
        str.pop_back();
        str.pop_back();
    }
    str.push_back('}');
    return str;
}

set<string> inputSet(){
    
    char c;
    cin >> c;

    if(c != '{'){
        cerr << "The set has to start with {";
    }
    set<string> set;

    while(c != '}'){
        string s;
        cin >> c;

        while(c != ',' && c != '}'){

            if(isalnum(c)){
                s.push_back(c);
            }
            cin >> c;
        }
        if(!s.empty()){
           set.insert(s);
        }
    }
    return set;
}

int main(){
    SetExpr* exp = load();

    if(exp == nullptr){
        cerr << "failed to parse" << endl;
        return -1;
    }
    cout << "The input is parsed as: ";
    string str = exp->toString();

    coloredPrint(exp->toString(), 0, 1);

    cout << "\n the result should be: " << endl;
    coloredPrint(exp->calculate()->toString(), 0, 1);
    cout << endl;
}
