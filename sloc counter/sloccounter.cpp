#include <iostream>
#include <fstream>
#include <string>

int main(int argc, char* argv[]){
    unsigned long long sloc = 0;
    if (argc < 2){
        std::cerr << "Usage: ./sloccounter .[file extention]\n Use ./sloccounter .* to process all files" << std::endl;
        return -1;
    }
    std::cout << "Processing files:" << argv[1] << std::endl;
    std::string fileName = argv[1];
    std::ifstream inputFile;
    inputFile.open(argv[1], std::ios::in);

    if(!inputFile){
        std::cerr << "Unable to open file:" << argv[1] << std::endl;
        return -1;
    }
    std::string line = "";
    while(getline(inputFile, line)){
        if (line.length() > 1){
            sloc++;
            line = "";
        }
    }
    std::cout << "Counted:" << sloc << " single lines of code" << std::endl;



    return 0;
}