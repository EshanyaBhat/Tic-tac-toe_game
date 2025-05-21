#ifndef BOARD_H
#define BOARD_H

#include <iostream>
#include <vector>
#include <string>

class Board {
private:
    std::vector<std::vector<char>> grid;
    const int size = 3;

public:
    // Constructor
    Board();
    
    // Reset the board
    void reset();
    
    // Make a move on the board
    bool makeMove(int row, int col, char symbol);
    
    // Check if the game is over
    bool isGameOver() const;
    
    // Check if there's a winner
    char checkWinner() const;
    
    // Check if the board is full
    bool isFull() const;
    
    // Get the board state as a string for network transfer
    std::string serialize() const;
    
    // Set the board state from a string
    void deserialize(const std::string& boardStr);
    
    // Display the board
    void display() const;
};

#endif