#include "player.h"

Player::Player(const std::string& playerName, char playerSymbol) 
    : name(playerName), symbol(playerSymbol) {}

std::string Player::getName() const {
    return name;
}

char Player::getSymbol() const {
    return symbol;
}

void Player::setName(const std::string& playerName) {
    name = playerName;
}

void Player::setSymbol(char playerSymbol) {
    symbol = playerSymbol;
}