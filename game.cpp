#include "game.h"
#include <iostream>
#include <sstream>
#include <cstdlib> // For system()

#ifdef _WIN32
    #include <windows.h> // For Sleep
#else
    #include <unistd.h> // For usleep
#endif

Game::Game(GameMode gameMode) 
    : state(GameState::WAITING_FOR_PLAYER), 
      mode(gameMode),
      isMyTurn(false) {
    
    // Initialize players based on game mode
    if (mode == GameMode::SERVER) {
        localPlayer = Player("Server", 'X');
        remotePlayer = Player("Client", 'O');
        isMyTurn = true;  // Server (X) goes first
    } else {
        localPlayer = Player("Client", 'O');
        remotePlayer = Player("Server", 'X');
        isMyTurn = false;  // Client (O) goes second
    }
    
    // Create network instance
    network = std::make_unique<Network>(mode == GameMode::SERVER);
}

bool Game::initialize(const std::string& address, int port) {
    bool result = false;
    
    if (mode == GameMode::SERVER) {
        // Start server
        result = network->startServer(port);
        if (result) {
            std::cout << "Waiting for opponent to connect..." << std::endl;
            Network clientNetwork;
            result = network->acceptClient(clientNetwork);
            if (result) {
                network = std::make_unique<Network>();
                *network = std::move(clientNetwork);
                state = GameState::PLAYER_X_TURN;
            }
        }
    } else {
        // Connect to server
        result = network->connectToServer(address, port);
        if (result) {
            state = GameState::PLAYER_X_TURN;
        }
    }
    
    return result;
}

void Game::processMessage(const std::string& message) {
    if (message.length() < 1) {
        return;
    }
    
    char command = message[0];
    std::string payload = message.substr(1);
    
    switch (command) {
        case 'M': {  // Move
            if (payload.length() >= 2) {
                int row = payload[0] - '0';
                int col = payload[1] - '0';
                
                // Update board
                board.makeMove(row, col, remotePlayer.getSymbol());
                
                // Switch turns
                isMyTurn = true;
                
                // Update game state
                if (board.checkWinner() != ' ') {
                    state = GameState::GAME_OVER;
                } else if (board.isFull()) {
                    state = GameState::GAME_OVER;
                } else {
                    state = isMyTurn ? 
                        (localPlayer.getSymbol() == 'X' ? GameState::PLAYER_X_TURN : GameState::PLAYER_O_TURN) :
                        (remotePlayer.getSymbol() == 'X' ? GameState::PLAYER_X_TURN : GameState::PLAYER_O_TURN);
                }
            }
            break;
        }
        case 'B': {  // Board state
            board.deserialize(payload);
            break;
        }
        case 'R': {  // Reset game
            board.reset();
            isMyTurn = mode == GameMode::SERVER;  // Server (X) always starts
            state = GameState::PLAYER_X_TURN;
            break;
        }
    }
}

bool Game::makeMove(int row, int col) {
    if (!isMyTurn || state == GameState::GAME_OVER) {
        return false;
    }
    
    if (!board.makeMove(row, col, localPlayer.getSymbol())) {
        return false;
    }
    
    // Send move to opponent
    std::string moveMsg = "M" + std::string(1, '0' + row) + std::string(1, '0' + col);
    network->sendMessage(moveMsg);
    
    // Check for game over
    if (board.checkWinner() != ' ') {
        state = GameState::GAME_OVER;
    } else if (board.isFull()) {
        state = GameState::GAME_OVER;
    } else {
        // Switch turns
        isMyTurn = false;
        state = remotePlayer.getSymbol() == 'X' ? GameState::PLAYER_X_TURN : GameState::PLAYER_O_TURN;
    }
    
    return true;
}

GameState Game::getState() const {
    return state;
}

bool Game::isLocalPlayerTurn() const {
    return isMyTurn;
}

void Game::displayBoard() const {
    board.display();
}

Player Game::getLocalPlayer() const {
    return localPlayer;
}

Player Game::getRemotePlayer() const {
    return remotePlayer;
}

void Game::runGameLoop() {
    // Initial board display
    system("cls");  // Clear screen (Windows)
    displayBoard();
    if (isMyTurn) {
        std::cout << "Your turn (" << localPlayer.getSymbol() << ")" << std::endl;
    } else {
        std::cout << "Opponent's turn (" << remotePlayer.getSymbol() << ")" << std::endl;
    }
    
    // Main game loop
    while (network->isConnected()) {
        // Check for incoming messages
        std::string message = network->receiveMessage();
        if (!message.empty()) {
            processMessage(message);
            
            // Display updated board
            system("cls");  // Clear screen (Windows)
            displayBoard();
            
            // Display game status
            if (state == GameState::GAME_OVER) {
                char winner = board.checkWinner();
                if (winner == localPlayer.getSymbol()) {
                    std::cout << "You win!" << std::endl;
                } else if (winner == remotePlayer.getSymbol()) {
                    std::cout << "You lose!" << std::endl;
                } else {
                    std::cout << "It's a draw!" << std::endl;
                }
            } else {
                if (isMyTurn) {
                    std::cout << "Your turn (" << localPlayer.getSymbol() << ")" << std::endl;
                } else {
                    std::cout << "Opponent's turn (" << remotePlayer.getSymbol() << ")" << std::endl;
                }
            }
        }
        
        // Always show board periodically (helps with display issues)
        static int displayCounter = 0;
        if (++displayCounter > 50) {
            displayCounter = 0;
            system("cls");
            displayBoard();
            if (isMyTurn) {
                std::cout << "Your turn (" << localPlayer.getSymbol() << ")" << std::endl;
            } else {
                std::cout << "Opponent's turn (" << remotePlayer.getSymbol() << ")" << std::endl;
            }
        }
        
        if (isMyTurn && state != GameState::GAME_OVER) {
            int row, col;
            std::cout << "Enter your move (row [0-2] column [0-2]): ";
            std::cin >> row >> col;
            
            if (row >= 0 && row < 3 && col >= 0 && col < 3) {
                if (!makeMove(row, col)) {
                    std::cout << "Invalid move! Try again." << std::endl;
                } else {
                    // Display updated board after our move
                    system("cls");
                    displayBoard();
                    std::cout << "Opponent's turn (" << remotePlayer.getSymbol() << ")" << std::endl;
                }
            } else {
                std::cout << "Invalid input! Row and column must be between 0 and 2." << std::endl;
            }
        } else if (state == GameState::GAME_OVER) {
            char playAgain;
            std::cout << "Play again? (y/n): ";
            std::cin >> playAgain;
            
            if (playAgain == 'y' || playAgain == 'Y') {
                board.reset();
                isMyTurn = mode == GameMode::SERVER;  // Server (X) always starts
                state = GameState::PLAYER_X_TURN;
                
                // Send reset command
                network->sendMessage("R");
                
                // Display fresh board
                system("cls");
                displayBoard();
                if (isMyTurn) {
                    std::cout << "Your turn (" << localPlayer.getSymbol() << ")" << std::endl;
                } else {
                    std::cout << "Opponent's turn (" << remotePlayer.getSymbol() << ")" << std::endl;
                }
            } else {
                break;
            }
        }
        
        // Small delay to prevent CPU hogging
        #ifdef _WIN32
            Sleep(100); // Windows sleep function (milliseconds)
        #else
            usleep(100000); // Unix sleep function (microseconds)
        #endif
    }
}