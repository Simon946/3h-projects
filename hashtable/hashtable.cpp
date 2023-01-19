#include <iostream>
#include <string>
#include <vector>

const int HASHTABLE_SIZE = 1024;//length of the hash table.

template <typename Type> class HashItem{
    public:
        HashItem(std::string newKey, Type newValue) : key(newKey), value(newValue) {};
        std::string key = "";
        Type value;
        HashItem<Type>* next = nullptr;
};

template <class Type> class HashTable{
    public:
        void insert(std::string key, Type value);
        void remove(std::string key);
        Type get(std::string key);
        Type defaultValue;
        void print();

    private:
        unsigned int hash(std::string string);
        std::vector<HashItem<Type>*> table = std::vector<HashItem<Type>*>(HASHTABLE_SIZE, nullptr);
};

template <class Type> void HashTable<Type>::insert(std::string key, Type value){
    HashItem<Type>* existingItem = table.at(hash(key) % HASHTABLE_SIZE);
    
    if(existingItem != nullptr){
        existingItem->value = value;
        return;
    }
    HashItem<Type>* currentItem = existingItem;

    if(currentItem == nullptr){
        table.at(hash(key) % HASHTABLE_SIZE) = new HashItem<Type>(key, value);
        return;
    }
    while(currentItem->next != nullptr){
        
        if(currentItem->key == key){
            currentItem->value = value;
            return;
        }
        currentItem = currentItem->next;
    }
    currentItem->next = new HashItem<Type>(key, value);
}

template <class Type> Type HashTable<Type>::get(std::string key){
    HashItem<Type>* currentItem = table.at(hash(key) % HASHTABLE_SIZE);

    if(currentItem == nullptr){
        return defaultValue;
    }
    if(currentItem->key == key){
        return currentItem->value;
    }
    while(currentItem->next != nullptr){
        
        if(currentItem->key == key){
            return currentItem->value;
        }
        currentItem = currentItem->next;
    }
    return defaultValue;
}

template <class Type> void HashTable<Type>::remove(std::string key){
    HashItem<Type>* previousItem = table.at(hash(key) % HASHTABLE_SIZE);

    if(previousItem == nullptr){
        return;
    }
    if(previousItem->key == key){

        if(previousItem->next != nullptr){
            table.at(hash(key) % HASHTABLE_SIZE) = previousItem->next; 
        }
        delete previousItem;
        return;
    }
    HashItem<Type>* currentItem = previousItem->next;

    if(currentItem == nullptr){
        return;
    }
    while(currentItem->key != key){
        previousItem = currentItem;
        currentItem = currentItem->next;
    }
    previousItem->next = currentItem->next;
    delete currentItem;
}

template <class Type> unsigned int HashTable<Type>::hash(std::string string){//Hash function inspired by http://www.cse.yorku.ca/~oz/hash.html
    unsigned int hash = 5381;

    for(size_t i = 0; i < string.length(); i++){
        hash = ((hash << 5) + hash) + static_cast<int>(string.at(i));
    }
    return hash;
}

template <class Type> void HashTable<Type>::print(){
    std::cout << "Printing table:" << std::endl;
    
    for(int i = 0; i < HASHTABLE_SIZE; i++){
        std::cout << i << ' ';

        if(table.at(i) == nullptr){
            std::cout << "[EMPTY]" << std::endl;
            continue;
        }
        HashItem<Type>* item = table.at(i);
        do{
            std::cout << '[' << item->key << ", " << item->value << "] ";
            item = item->next;

        }while(item != nullptr);
        std::cout << "[END]" << std::endl;
    }
}

int main(){
    HashTable<int> table;

    table.insert("coffee", 10);
    std::cout << table.get("coffee") << std::endl;
    table.print();

    std::string command = "";
    std::string key = "";
    int value = 0;
    table.defaultValue = -1;

    while(command != "exit"){
        
        if(command == "insert"){
            std::cin >> key;
            std::cin >> value;
            table.insert(key, value);
        }
        else if(command == "get"){
            std::cin >> key;
            std::cout << table.get(key) << std::endl;
        }
        else if(command == "remove"){
            std::cin >> key;
            table.remove(key);
        }
        else if(command == "print"){
            table.print();
        }
        std::cin >> command;
    }
    return 0;
}
