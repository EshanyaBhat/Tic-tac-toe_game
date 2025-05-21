#include <iostream>
#include <winsock2.h>
#include <ws2tcpip.h>
#include <string>

#pragma comment(lib, "ws2_32.lib")

// Simple 3x3 board
char board[3][3] = {
    {' ', ' ', ' '},
    {' ', ' ', ' '},
    {' ', ' ', ' '}
};

// Print the board
void printBoard() {
    std::cout << "\n";
    for (int i = 0; i < 3; i++) {
        std::cout << " " << board[i][0] << " | " << board[i][1] << " | " << board[i][2] << std::endl;
        if (i < 2) std::cout << "-----------\n";
    }
    std::cout << "\n";
}

int main() {
    // Initialize Winsock
    WSADATA wsaData;
    if (WSAStartup(MAKEWORD(2, 2), &wsaData) != 0) {
        std::cerr << "WSAStartup failed" << std::endl;
        return 1;
    }
    
    // Create socket
    SOCKET clientSocket = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
    if (clientSocket == INVALID_SOCKET) {
        std::cerr << "Socket creation failed" << std::endl;
        WSACleanup();
        return 1;
    }
    
    // Set up server address
    sockaddr_in serverAddr;
    serverAddr.sin_family = AF_INET;
    serverAddr.sin_port = htons(8888);
    
    // Get server IP
    std::string ipAddress;
    std::cout << "Enter server IP address: ";
    std::cin >> ipAddress;
    
    // Convert IP address from string to binary
    serverAddr.sin_addr.s_addr = inet_addr(ipAddress.c_str());
    if (serverAddr.sin_addr.s_addr == INADDR_NONE) {
        std::cerr << "Invalid address" << std::endl;
        closesocket(clientSocket);
        WSACleanup();
        return 1;
    }
    
    // Connect to server
    if (connect(clientSocket, (struct sockaddr*)&serverAddr, sizeof(serverAddr)) == SOCKET_ERROR) {
        std::cerr << "Connection failed" << std::endl;
        closesocket(clientSocket);
        WSACleanup();
        return 1;
    }
    
    std::cout << "Connected to server! You are O, server is X." << std::endl;
    
    char buffer[1024];
    bool gameOver = false;
    
    while (!gameOver) {
        // Receive server's move
        std::cout << "Waiting for server's move..." << std::endl;
        int bytesReceived = recv(clientSocket, buffer, sizeof(buffer), 0);
        if (bytesReceived <= 0) {
            std::cout << "Server disconnected" << std::endl;
            break;
        }
        
        buffer[bytesReceived] = '\0';
        std::string message(buffer);
        
        // Check if game is over
        if (message.substr(0, 10) == "GAME_OVER_") {
            char result = message[10];
            printBoard();
            if (result == 'X') {
                std::cout << "Server wins!" << std::endl;
            } else if (result == 'O') {
                std::cout << "You win!" << std::endl;
            } else {
                std::cout << "It's a draw!" << std::endl;
            }
            gameOver = true;
            break;
        }
        
        // Process server's move
        int row = buffer[0] - '0';
        int col = buffer[1] - '0';
        board[row][col] = 'X';
        
        std::cout << "Server moved at (" << row << ", " << col << ")" << std::endl;
        printBoard();
        
        // Client's turn
        int clientRow, clientCol;
        bool validMove = false;
        
        while (!validMove) {
            std::cout << "Your turn (O). Enter row and column (0-2): ";
            std::cin >> clientRow >> clientCol;
            
            // Validate move
            if (clientRow < 0 || clientRow > 2 || clientCol < 0 || clientCol > 2 || board[clientRow][clientCol] != ' ') {
                std::cout << "Invalid move! Try again." << std::endl;
            } else {
                validMove = true;
            }
        }
        
        // Make move
        board[clientRow][clientCol] = 'O';
        
        // Send move to server
        std::string move = std::to_string(clientRow) + std::to_string(clientCol);
        send(clientSocket, move.c_str(), move.length(), 0);
    }
    
    // Cleanup
    closesocket(clientSocket);
    WSACleanup();
    
    std::cout << "Press Enter to exit..." << std::endl;
    std::cin.ignore();
    std::cin.get();
    
    return 0;
}