#include <iostream>
#include <cstring>

char changeUpperLower(char a){
    if(a >= 'A' && a <= 'Z') return a + 32; //uppercase to lowercase
    if(a >= 'a' && a <= 'z') return a - 32; //lowercase to uppercase
    return a;
}

int main(int argc, char* argv[]){
    
    if(argc != 2) return 1;
    
    for(int i = 0; i < strlen(argv[1]); i++){
        std::cout << changeUpperLower(argv[1][i]);
    }
    std::cout << std::endl;
    return 0;
}
