#include <iostream>
#include <set>
#include <string>
#include <assert.h>
#include "colorize.cpp"

using namespace std;

enum ExprType {
    UNION,     //all elements in lhs or rhs
    INTERSECT, //all elements in lhs and rhs
    COMPLEMENT, //all elements, except those in this->complement. This does not apply to this->lhs or this->rhs but to this->complement
    DIFFERENCE, //all elements in lhs but not in rhs and vice versa 
    CONSTANT, //the expression is constant aka a single set. no need to calculate further.
    MULTIPLY, //concatenate all elements in rhs to lhs.
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
        SetExpr(const SetExpr& other);
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
SetExpr* calculateMultiply(SetExpr* lhs, SetExpr* rhs);

SetExpr::SetExpr(SetExpr* lhs, ExprType operand, SetExpr* rhs){
    this->lhs = lhs;
    this->operand = operand;
    this->rhs = rhs;

    if(operand == COMPLEMENT){
        if(lhs != rhs){
            cerr << "lhs must be rhs when the SetExpr is complement" << endl;   
        }
        this->complement = lhs;
    }
    if(operand == CONSTANT){
        cerr << "cannot create new constant like this" << endl;
    }
}

SetExpr::SetExpr(set<string> constant){
    this->operand = CONSTANT;
    this->value = constant;
}

SetExpr::SetExpr(const SetExpr& other){

    this->lhs = nullptr;
    this->rhs = nullptr;
    this->complement = nullptr;

    switch (other.operand){
        case UNION:
        case INTERSECT:
        case DIFFERENCE:{
            this->lhs = new SetExpr(*other.lhs);
            this->rhs = new SetExpr(*other.rhs);
            break;
        }
        case CONSTANT:{
            break;
        }
        case COMPLEMENT:{
            this->complement = new SetExpr(*other.complement);
            break;
        }       
        default:{
            cerr << "unknown operand while copying SetExpr" << endl;
            break;
        }
    }
    this->value = other.value;
    this->operand = other.operand;
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
            ignoreSpaces();
            
            char operand;
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
            else if(operand == ')'){
                return lhs;
            }
            else if(operand == '*'){
                type = MULTIPLY;
            }
            else if(operand == '^'){
                int power = 1;
                cin >> power;
                SetExpr* base = lhs;

                while(power > 1){
                    base = new SetExpr(base, MULTIPLY, new SetExpr(*lhs));
                    power--;
                }
                ignoreSpaces();
                cin.ignore(1);
                return base;
            }
            else{
                cerr << "unexpected operand: " << operand << endl;
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
            cerr << "unexpected character: " << c <<  " expected ( or {" << endl;
            break;
    }
    return nullptr;
}

SetExpr::~SetExpr(){
    
    if(this->operand == COMPLEMENT){
        delete complement;
    }
    else{
        delete lhs;
        delete rhs;
    }
    
}

SetExpr* calculateUnion(SetExpr* lhs, SetExpr* rhs){

    if(lhs->operand == CONSTANT && rhs->operand == CONSTANT){
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
    SetExpr* complement = new SetExpr(*lhs);
    SetExpr* other = new SetExpr(*rhs);

    if(rhs->operand == COMPLEMENT){
        swap(complement, other);
    }
    SetExpr* inverseIntersect = calculateIntersect(complement->complement, other);
    SetExpr* inverseDifference = calculateDifference(complement->complement, inverseIntersect);
    return new SetExpr(inverseDifference, COMPLEMENT, inverseDifference);
}

SetExpr* calculateIntersect(SetExpr* lhs, SetExpr* rhs){

    if(lhs->operand == CONSTANT && rhs->operand == CONSTANT){
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
    SetExpr* complement = new SetExpr(*lhs);
    SetExpr* other = new SetExpr(*rhs);

    if(rhs->operand == COMPLEMENT){
        swap(complement, other);
    }

    return calculateDifference(complement->complement, other);
}

SetExpr* calculateDifference(SetExpr* lhs, SetExpr* rhs){

    if(lhs->operand == CONSTANT && rhs->operand == CONSTANT){
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
    SetExpr* complement = new SetExpr(*lhs);
    SetExpr* other = new SetExpr(*rhs);

    if(rhs->operand == COMPLEMENT){
        swap(complement, other);
    }
    SetExpr* inverseDifference = calculateUnion(complement->complement, other);
    return new SetExpr(inverseDifference, COMPLEMENT, inverseDifference);   
}

SetExpr* calculateMultiply(SetExpr* lhs, SetExpr* rhs){
    assert(lhs->operand == CONSTANT && rhs->operand == CONSTANT);

    set<string> result;

    for(string start : lhs->value){

        for(string end : rhs->value){
            result.insert(start + end);
        }
    }
    return new SetExpr(result);
}

SetExpr* filterLength(SetExpr* exp, int maxLength){
    assert(exp->operand == CONSTANT);

    set<string>result;

    for(string s : exp->value){
        if(s.length() <= maxLength){
            result.insert(s);
        }
    }
    return new SetExpr(result);
}

SetExpr* SetExpr::calculate(){
    
    if(this->operand == CONSTANT){
        return new SetExpr(this->value);
    }
    if(this->operand == COMPLEMENT){
        if(this->complement->operand == COMPLEMENT){
            return new SetExpr(*this->complement->complement);//double complement
        }
        return new SetExpr(*this);//best I can do
    }

    SetExpr* lhs = this->lhs->calculate();
    SetExpr* rhs = this->rhs->calculate();

    SetExpr* result = nullptr;

    switch (this->operand){
        case UNION:{
            result = calculateUnion(lhs, rhs);
            break;
        }
        case INTERSECT:{
            result = calculateIntersect(lhs, rhs);
            break;
        }
        case DIFFERENCE:{
            result = calculateDifference(lhs, rhs);
            break;
        }
        case MULTIPLY:{
            result = calculateMultiply(lhs, rhs);
            break;
        }
    default:
        cerr << "unknown operand: " << this->operand;
        break;
    }
    if(result == nullptr){
        return new SetExpr(*this);
    }
    delete lhs;
    delete rhs;
    SetExpr* tmp = result->calculate();
    delete result;
    return tmp;
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
        case MULTIPLY:{
            operand = "multiply";
            break;
        }
        default:{
            cerr << "unknown operand: " << this->operand;
            return "";
        }
    }
    str.append("(");
    str.append(this->lhs->toString());
    str.append(" ");
    str.append(operand);
    str.append(" ");
    str.append(this->rhs->toString());
    str.append(")");
    return str;
}

string setToString(set<string> set){
    string str = "{";  
    
    for(string s : set){
        if(s.empty()){
            str.append("lambda");
        }
        else{
            str.append(s);
        }
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
        if(s == "lambda"){
            set.insert("");
        }
        else{
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
    coloredPrint(exp->toString(), 0, 1);
    cout << endl;
    
    SetExpr* result = exp->calculate();
    SetExpr* trimmed = filterLength(result, 4);

    cout << "The result should be: ";
    coloredPrint(trimmed->toString(), 0, 1);
    cout << endl;
    delete exp;
    delete result;
    delete trimmed;
    return 0;
}
