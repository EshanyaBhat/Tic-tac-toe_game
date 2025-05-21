#ifndef GAME_H
#define GAME_H

#include "board.h"
#include "player.h"
#include "network.h"
#include <memory>

enum class GameState {
    WAITING_FOR_PLAYER,
    PLAYER_X_TURN,
    PLAYER_O_TURN,
    GAME_OVER
};

enum class GameMode {
    SERVER,
    CLIENT
};

class Game {
private:
    Board board;
    Player localPlayer;
    Player remotePlayer;
    GameState state;
    GameMode mode;
    bool isMyTurn;
    std::unique_ptr<Network> network;

public:
    // Constructor
    Game(GameMode gameMode);
    
    // Initialize game
    bool initialize(const std::string& address = "", int port = 8888);
    
    // Process incoming network message
    void processMessage(const std::string& message);
    
    // Make a move
    bool makeMove(int row, int col);
    
    // Get current game state
    GameState getState() const;
    
    // Check if it's local player's turn
    bool isLocalPlayerTurn() const;
    
    // Display game board
    void displayBoard() const;
    
    // Get local player
    Player getLocalPlayer() const;
    
    // Get remote player
    Player getRemotePlayer() const;
    
    // Run game loop
    void runGameLoop();
};

#endif