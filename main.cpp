#include <iostream>
#include <string>
#include "game.h"

int main() {
    std::cout << "=== Multiplayer Tic-Tac-Toe ===" << std::endl;
    std::cout << "1. Start Server" << std::endl;
    std::cout << "2. Connect as Client" << std::endl;
    std::cout << "Select an option: ";
    
    int option;
    std::cin >> option;
    
    switch (option) {
        case 1: {
            // Create game in server mode
            Game game(GameMode::SERVER);
            
            // Initialize server
            int port = 8888;
            std::cout << "Enter port to listen on (default 8888): ";
            std::cin >> port;
            
            if (!game.initialize("", port)) {
                std::cerr << "Failed to initialize server" << std::endl;
                return 1;
            }
            
            // Run game loop
            game.runGameLoop();
            break;
        }
        case 2: {
            // Create game in client mode
            Game game(GameMode::CLIENT);
            
            // Connect to server
            std::string address;
            int port = 8888;
            
            std::cout << "Enter server IP address: ";
            std::cin >> address;
            
            std::cout << "Enter server port (default 8888): ";
            std::cin >> port;
            
            if (!game.initialize(address, port)) {
                std::cerr << "Failed to connect to server" << std::endl;
                return 1;
            }
            
            // Run game loop
            game.runGameLoop();
            break;
        }
        default:
            std::cerr << "Invalid option" << std::endl;
            return 1;
    }
    
    return 0;
}