#include <iostream>
#include <string>

namespace Base64{
    std::string table = "ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz0123456789+/=";

    uint8_t charToIndex(char c){
        
        if(c >= 'A' && c <= 'Z'){
            return c - 'A';
        }
        if(c >= 'a' && c <= 'z'){
            return c - 'a' + 26;
        }
        if(c >= '0' && c <= '9'){
            return c - '0' + 52;
        }
        if(c == '+'){
            return 62;
        }
        if(c == '/'){
            return 63;
        }
        return 0;
    }

    char indexToChar(uint8_t index){
        return (index >= table.length())? '\0' : table.at(index);
    }

    std::string encodeBlock(uint8_t a, uint8_t b, uint8_t c, int padding){
        std::string base64;
        uint32_t tmp = (a << 16) | (b << 8) | c;
        base64.push_back(indexToChar(tmp >> 18 & 0x3F));
        base64.push_back(indexToChar(tmp >> 12 & 0x3F));
        base64.push_back((padding < 2)? indexToChar(tmp >> 6 & 0x3F) : '=');
        base64.push_back((padding < 1)? indexToChar(tmp & 0x3F) : '=');
        return base64;
    }

    size_t decodeBlock(std::string base64String, uint8_t* output){

        while(base64String.length() < 4){
            base64String.push_back('=');  
        }
        char a = base64String.at(0);
        char b = base64String.at(1);
        char c = base64String.at(2);
        char d = base64String.at(3);
        
        uint32_t tmp = ((charToIndex(a) & 0x3F) << 18) | ((charToIndex(b) & 0x3F) << 12) | ((charToIndex(c) & 0x3F) << 6) | (charToIndex(d) & 0x3F);

        output[0] = tmp >> 16 & 0xFF;
        output[1] = tmp >> 8 & 0xFF;
        output[2] = tmp & 0xFF;

        int bytesWritten = 1;

        if(c != '='){
            bytesWritten++;
        }
        if(d != '='){
            bytesWritten++;
        }
        return bytesWritten;
    }

    std::string encode(uint8_t* input, size_t inputSize){
        std::string base64;

        for(int i = 0; i < inputSize; i += 3){
            int padding = 0;
            uint8_t a = input[i];
            uint8_t b = 0;
            uint8_t c = 0;
            (i + 1 < inputSize)? b = input[i + 1] : padding++;
            (i + 2 < inputSize)? c = input[i + 2] : padding++;
            base64.append(encodeBlock(a, b, c, padding));
        }
        return base64;
    }

    std::string encode(std::istream& stream){
        std::string base64;
        
        while(stream.good()){
            uint8_t* bytes = new uint8_t[3];
            stream.read((char*)bytes, 3);      
            int padding = 3 - stream.gcount();
            base64.append(encodeBlock(bytes[0], bytes[1], bytes[2], padding));
        }
        return base64;
    }

    size_t decode(uint8_t* output, size_t outputSize, std::string base64String){
        size_t writePosition = 0;

        while(base64String.length() % 4 != 0){
            base64String.push_back('=');   
        }
        for(int i = 0; i < base64String.length(); i += 4){
            if(writePosition >= outputSize){
                return writePosition - 4;
            }
            writePosition += decodeBlock(base64String.substr(i, 4), output + writePosition);
        }
        return writePosition;
    }
}

const int BUFFER_SIZE = 128;

int main(){
    uint8_t* buffer = new uint8_t[BUFFER_SIZE];
    size_t decodedSize = Base64::decode(buffer, BUFFER_SIZE, "SGVsbG8sIHRoZSBwcm9ncmFtIHdvcmtzIHByb3Blcmx5IQ=="); //Hello, the program works properly!

    for(int i = 0; i < decodedSize; i++){
        std::cout << buffer[i];
    }
    std::cout << "\nEnter text to encode: (finish with crtl + D)" << Base64::encode(std::cin);
    delete buffer;
    return 0;
}