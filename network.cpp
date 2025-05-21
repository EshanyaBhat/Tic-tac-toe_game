#include "network.h"
#include <iostream>
#include <cstring>

bool Network::initSockets() {
#ifdef _WIN32
    WSADATA wsaData;
    return WSAStartup(MAKEWORD(2, 2), &wsaData) == 0;
#else
    return true;
#endif
}

void Network::cleanupSockets() {
#ifdef _WIN32
    WSACleanup();
#endif
}

Network::Network(bool serverMode) : isServer(serverMode), socket(INVALID_SOCKET) {
    initSockets();
}

Network::~Network() {
    close();
    if (isServer) {
        cleanupSockets();
    }
}

bool Network::startServer(int port) {
    // Create socket
    socket = ::socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
    if (socket == INVALID_SOCKET) {
        std::cerr << "Error creating socket" << std::endl;
        return false;
    }
    
    // Set up server address
    sockaddr_in serverAddr;
    memset(&serverAddr, 0, sizeof(serverAddr));
    serverAddr.sin_family = AF_INET;
    serverAddr.sin_addr.s_addr = INADDR_ANY;
    serverAddr.sin_port = htons(port);
    
    // Bind socket
    if (bind(socket, (struct sockaddr*)&serverAddr, sizeof(serverAddr)) == SOCKET_ERROR) {
        std::cerr << "Bind failed" << std::endl;
        closesocket(socket);
        socket = INVALID_SOCKET;
        return false;
    }
    
    // Listen for connections
    if (listen(socket, 1) == SOCKET_ERROR) {
        std::cerr << "Listen failed" << std::endl;
        closesocket(socket);
        socket = INVALID_SOCKET;
        return false;
    }
    
    std::cout << "Server started on port " << port << std::endl;
    return true;
}

bool Network::acceptClient(Network& clientNetwork) {
    // Accept connection
    sockaddr_in clientAddr;
    socklen_t clientAddrSize = sizeof(clientAddr);
    
    SocketType clientSocket = accept(socket, (struct sockaddr*)&clientAddr, &clientAddrSize);
    if (clientSocket == INVALID_SOCKET) {
        std::cerr << "Accept failed" << std::endl;
        return false;
    }
    
    clientNetwork.socket = clientSocket;
    
    std::cout << "Client connected: " << inet_ntoa(clientAddr.sin_addr) << std::endl;
    return true;
}

bool Network::connectToServer(const std::string& address, int port) {
    // Create socket
    socket = ::socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
    if (socket == INVALID_SOCKET) {
        std::cerr << "Error creating socket" << std::endl;
        return false;
    }
    
    // Set up server address
    sockaddr_in serverAddr;
    memset(&serverAddr, 0, sizeof(serverAddr));
    serverAddr.sin_family = AF_INET;
    serverAddr.sin_port = htons(port);
    
    // Convert IP address from string to binary
    serverAddr.sin_addr.s_addr = inet_addr(address.c_str());
    if (serverAddr.sin_addr.s_addr == INADDR_NONE) {
        std::cerr << "Invalid address" << std::endl;
        closesocket(socket);
        socket = INVALID_SOCKET;
        return false;
    }
    
    // Connect to server
    if (connect(socket, (struct sockaddr*)&serverAddr, sizeof(serverAddr)) == SOCKET_ERROR) {
        std::cerr << "Connect failed" << std::endl;
        closesocket(socket);
        socket = INVALID_SOCKET;
        return false;
    }
    
    std::cout << "Connected to server: " << address << ":" << port << std::endl;
    return true;
}

bool Network::sendMessage(const std::string& message) {
    if (socket == INVALID_SOCKET) {
        return false;
    }
    
    // Send message length first
    uint32_t length = htonl(static_cast<uint32_t>(message.length()));
    if (send(socket, reinterpret_cast<char*>(&length), sizeof(length), 0) != sizeof(length)) {
        return false;
    }
    
    // Send message
    int sent = send(socket, message.c_str(), message.length(), 0);
    return sent == static_cast<int>(message.length());
}

std::string Network::receiveMessage() {
    if (socket == INVALID_SOCKET) {
        return "";
    }
    
    // Check if data is available
    fd_set readSet;
    FD_ZERO(&readSet);
    FD_SET(socket, &readSet);
    
    // Set timeout to 10ms
    timeval timeout;
    timeout.tv_sec = 0;
    timeout.tv_usec = 10000;
    
    int selectResult = select(socket + 1, &readSet, NULL, NULL, &timeout);
    if (selectResult <= 0) {
        return ""; // No data available or error
    }
    
    // Receive message length first
    uint32_t length = 0;
    int recvResult = recv(socket, reinterpret_cast<char*>(&length), sizeof(length), 0);
    if (recvResult != sizeof(length)) {
        return "";
    }
    length = ntohl(length);
    
    // Receive message
    std::string message;
    message.resize(length);
    
    size_t totalReceived = 0;
    while (totalReceived < length) {
        int received = recv(socket, &message[totalReceived], length - totalReceived, 0);
        if (received <= 0) {
            return "";
        }
        totalReceived += received;
    }
    
    return message;
}

bool Network::isConnected() const {
    return socket != INVALID_SOCKET;
}

void Network::close() {
    if (socket != INVALID_SOCKET) {
        closesocket(socket);
        socket = INVALID_SOCKET;
    }
}