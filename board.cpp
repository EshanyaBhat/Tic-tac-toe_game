#include "board.h"

Board::Board() {
    reset();
}

void Board::reset() {
    grid.resize(size, std::vector<char>(size, ' '));
}

bool Board::makeMove(int row, int col, char symbol) {
    if (row < 0 || row >= size || col < 0 || col >= size || grid[row][col] != ' ') {
        return false;
    }
    
    grid[row][col] = symbol;
    return true;
}

bool Board::isGameOver() const {
    return checkWinner() != ' ' || isFull();
}

char Board::checkWinner() const {
    // Check rows
    for (int i = 0; i < size; i++) {
        if (grid[i][0] != ' ' && grid[i][0] == grid[i][1] && grid[i][1] == grid[i][2]) {
            return grid[i][0];
        }
    }
    
    // Check columns
    for (int i = 0; i < size; i++) {
        if (grid[0][i] != ' ' && grid[0][i] == grid[1][i] && grid[1][i] == grid[2][i]) {
            return grid[0][i];
        }
    }
    
    // Check diagonals
    if (grid[0][0] != ' ' && grid[0][0] == grid[1][1] && grid[1][1] == grid[2][2]) {
        return grid[0][0];
    }
    
    if (grid[0][2] != ' ' && grid[0][2] == grid[1][1] && grid[1][1] == grid[2][0]) {
        return grid[0][2];
    }
    
    return ' ';
}

bool Board::isFull() const {
    for (int i = 0; i < size; i++) {
        for (int j = 0; j < size; j++) {
            if (grid[i][j] == ' ') {
                return false;
            }
        }
    }
    return true;
}

std::string Board::serialize() const {
    std::string result;
    for (int i = 0; i < size; i++) {
        for (int j = 0; j < size; j++) {
            result += grid[i][j];
        }
    }
    return result;
}

void Board::deserialize(const std::string& boardStr) {
    if (boardStr.length() != size * size) {
        return;
    }
    
    int index = 0;
    for (int i = 0; i < size; i++) {
        for (int j = 0; j < size; j++) {
            grid[i][j] = boardStr[index++];
        }
    }
}

void Board::display() const {
    std::cout << "\n";
    for (int i = 0; i < size; i++) {
        std::cout << " ";
        for (int j = 0; j < size; j++) {
            std::cout << grid[i][j];
            if (j < size - 1) {
                std::cout << " | ";
            }
        }
        std::cout << "\n";
        if (i < size - 1) {
            std::cout << "-----------\n";
        }
    }
    std::cout << "\n";
}