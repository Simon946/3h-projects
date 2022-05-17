#include <iostream>
#include <fstream>
#include <string>

std::string positionString(int position){
    switch(position % 10){
        case 1:{
            return std::to_string(position) + "st";
            break;
        }
        case 2:{
            return std::to_string(position) + "nd";
            break;
        }
        default:{
            return std::to_string(position) + "th";
        }
    }
    return "";
}



int main(){
    std::fstream textFile;
    textFile.open("secretwords.txt");

    if(!textFile){
        std::cerr << "Unable to open the file." << std::endl;
        return 1;
    }
    std::string secretWord = "";
    std::string guessedWord = "";

    while(textFile >> secretWord){
        int wordPosition = 1;

        while(guessedWord != secretWord){
            int guessedValue = 0;
            
            std::cout << "Guess the ASCII value of the " << positionString(wordPosition) << " letter in the secret word: ";
            std::cin >> guessedValue;
            if(guessedValue == secretWord.at(wordPosition -1)){
                std::cout << "That is the correct value! The letter is:" << secretWord.at(wordPosition -1) << std::endl;
                guessedWord.push_back(guessedValue);
                std::cout << "You have guessed these letters so far: " << guessedWord << std::endl;
                wordPosition++;
            }
            else{
                (guessedValue < secretWord.at(wordPosition -1))? std::cout << "Your guess is too low!\n" : std::cout << "Your guess is too high!\n";
            }
        }
        std::cout << "You have successfully guessed the secret word! Well done! Get ready for the nex word!\n";
        guessedWord = "";
    }
    std::cout << "You have guessed all of the secret words" << std::endl;
    return 0;
}