#ifndef NETWORK_H
#define NETWORK_H

#include <string>
#include <functional>
#include <memory>

// Platform-specific socket headers
#ifdef _WIN32
    #include <winsock2.h>
    #include <ws2tcpip.h>
    #pragma comment(lib, "ws2_32.lib")
    typedef SOCKET SocketType;
#else
    #include <sys/socket.h>
    #include <netinet/in.h>
    #include <arpa/inet.h>
    #include <unistd.h>
    #include <fcntl.h>
    typedef int SocketType;
    #define INVALID_SOCKET -1
    #define SOCKET_ERROR -1
    #define closesocket close
#endif

// Network class that handles TCP communication
class Network {
private:
    SocketType socket;
    bool isServer;
    
    // Initialize socket library (Windows only)
    static bool initSockets();
    
    // Cleanup socket library (Windows only)
    static void cleanupSockets();

public:
    // Constructor
    Network(bool serverMode = false);
    
    // Destructor
    ~Network();
    
    // Initialize server
    bool startServer(int port);
    
    // Accept client connection
    bool acceptClient(Network& clientNetwork);
    
    // Connect to server
    bool connectToServer(const std::string& address, int port);
    
    // Send message
    bool sendMessage(const std::string& message);
    
    // Receive message (non-blocking)
    std::string receiveMessage();
    
    // Check if connected
    bool isConnected() const;
    
    // Close connection
    void close();
};

#endif