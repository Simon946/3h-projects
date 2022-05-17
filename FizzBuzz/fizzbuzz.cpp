#include <iostream>
#include <string>

std::string FizzBuzz(int max){
    std::string result = "";

    for(int i = 1; i <= max; i++){
        std::string value = "";
        if(i % 3 == 0){
            value += "Fizz";
        }
        if (i % 5 == 0){
            value += "Buzz";
        }
        if(value == ""){
            value = std::to_string(i);
        }
        result += value + " ";
    }
    return result;
}

int main(){
    int max;
    std::cout << "Enter a maximum value:";
    std::cin >> max;
    std::cout << FizzBuzz(max) << std::endl;
    return 0;
}