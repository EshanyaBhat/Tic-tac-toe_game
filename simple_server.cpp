#include <iostream>
#include <winsock2.h>
#include <ws2tcpip.h>
#include <string>
#include <vector>

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

// Check if there's a winner
char checkWinner() {
    // Check rows
    for (int i = 0; i < 3; i++) {
        if (board[i][0] != ' ' && board[i][0] == board[i][1] && board[i][1] == board[i][2]) {
            return board[i][0];
        }
    }
    
    // Check columns
    for (int i = 0; i < 3; i++) {
        if (board[0][i] != ' ' && board[0][i] == board[1][i] && board[1][i] == board[2][i]) {
            return board[0][i];
        }
    }
    
    // Check diagonals
    if (board[0][0] != ' ' && board[0][0] == board[1][1] && board[1][1] == board[2][2]) {
        return board[0][0];
    }
    
    if (board[0][2] != ' ' && board[0][2] == board[1][1] && board[1][1] == board[2][0]) {
        return board[0][2];
    }
    
    // Check if board is full (draw)
    bool isFull = true;
    for (int i = 0; i < 3; i++) {
        for (int j = 0; j < 3; j++) {
            if (board[i][j] == ' ') {
                isFull = false;
                break;
            }
        }
        if (!isFull) break;
    }
    
    if (isFull) return 'D'; // Draw
    
    return ' '; // No winner yet
}

int main() {
    // Initialize Winsock
    WSADATA wsaData;
    if (WSAStartup(MAKEWORD(2, 2), &wsaData) != 0) {
        std::cerr << "WSAStartup failed" << std::endl;
        return 1;
    }
    
    // Create socket
    SOCKET serverSocket = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
    if (serverSocket == INVALID_SOCKET) {
        std::cerr << "Socket creation failed" << std::endl;
        WSACleanup();
        return 1;
    }
    
    // Set up server address
    sockaddr_in serverAddr;
    serverAddr.sin_family = AF_INET;
    serverAddr.sin_addr.s_addr = INADDR_ANY;
    serverAddr.sin_port = htons(8888);
    
    // Bind socket
    if (bind(serverSocket, (struct sockaddr*)&serverAddr, sizeof(serverAddr)) == SOCKET_ERROR) {
        std::cerr << "Bind failed" << std::endl;
        closesocket(serverSocket);
        WSACleanup();
        return 1;
    }
    
    // Listen for connections
    if (listen(serverSocket, 1) == SOCKET_ERROR) {
        std::cerr << "Listen failed" << std::endl;
        closesocket(serverSocket);
        WSACleanup();
        return 1;
    }
    
    std::cout << "Server started. Waiting for client..." << std::endl;
    
    // Accept client connection
    sockaddr_in clientAddr;
    int clientAddrSize = sizeof(clientAddr);
    SOCKET clientSocket = accept(serverSocket, (struct sockaddr*)&clientAddr, &clientAddrSize);
    if (clientSocket == INVALID_SOCKET) {
        std::cerr << "Accept failed" << std::endl;
        closesocket(serverSocket);
        WSACleanup();
        return 1;
    }
    
    std::cout << "Client connected! You are X, client is O." << std::endl;
    
    char gameState = ' '; // No winner yet
    char buffer[1024];
    bool isServerTurn = true; // Server (X) goes first
    
    while (gameState == ' ') {
        if (isServerTurn) {
            // Server's turn
            printBoard();
            std::cout << "Your turn (X). Enter row and column (0-2): ";
            int row, col;
            std::cin >> row >> col;
            
            // Validate move
            if (row < 0 || row > 2 || col < 0 || col > 2 || board[row][col] != ' ') {
                std::cout << "Invalid move! Try again." << std::endl;
                continue;
            }
            
            // Make move
            board[row][col] = 'X';
            
            // Send move to client
            std::string move = std::to_string(row) + std::to_string(col);
            send(clientSocket, move.c_str(), move.length(), 0);
            
            // Check for winner
            gameState = checkWinner();
            if (gameState != ' ') break;
            
            isServerTurn = false;
        } else {
            // Client's turn
            std::cout << "Waiting for client's move..." << std::endl;
            
            // Receive move from client
            int bytesReceived = recv(clientSocket, buffer, sizeof(buffer), 0);
            if (bytesReceived <= 0) {
                std::cout << "Client disconnected" << std::endl;
                break;
            }
            
            buffer[bytesReceived] = '\0';
            int row = buffer[0] - '0';
            int col = buffer[1] - '0';
            
            // Make move
            board[row][col] = 'O';
            std::cout << "Client moved at (" << row << ", " << col << ")" << std::endl;
            
            // Check for winner
            gameState = checkWinner();
            if (gameState != ' ') break;
            
            isServerTurn = true;
        }
    }
    
    // Game over
    printBoard();
    if (gameState == 'X') {
        std::cout << "You win!" << std::endl;
    } else if (gameState == 'O') {
        std::cout << "Client wins!" << std::endl;
    } else {
        std::cout << "It's a draw!" << std::endl;
    }
    
    // Send game over message
    std::string gameOverMsg = "GAME_OVER_" + std::string(1, gameState);
    send(clientSocket, gameOverMsg.c_str(), gameOverMsg.length(), 0);
    
    // Cleanup
    closesocket(clientSocket);
    closesocket(serverSocket);
    WSACleanup();
    
    std::cout << "Press Enter to exit..." << std::endl;
    std::cin.ignore();
    std::cin.get();
    
    return 0;
}