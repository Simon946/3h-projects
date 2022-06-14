#include <iostream>
#include <cstring>

char changeUpperLower(char a){
    if(a >= 65 && a <= 90) return a + 32; //uppercase to lowercase ASCII 65 = A. 90 = Z
    if(a >= 97 && a <= 122) return a - 32; //lowercase to uppercase ASCII 97 = a. 122 = z
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