#ifndef PLAYER_H
#define PLAYER_H

#include <string>

class Player {
private:
    std::string name;
    char symbol;

public:
    // Constructor
    Player(const std::string& playerName = "Player", char playerSymbol = 'X');
    
    // Getters
    std::string getName() const;
    char getSymbol() const;
    
    // Setters
    void setName(const std::string& playerName);
    void setSymbol(char playerSymbol);
};

#endif