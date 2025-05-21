# XOX Game

A networked Tic-Tac-Toe game implementation in C++.

## Project Structure

- `board.cpp/h`: Game board implementation
- `game.cpp/h`: Game logic
- `player.cpp/h`: Player management
- `network.cpp/h`: Network communication
- `main.cpp`: Main program entry point
- `simple_client.cpp`: Client implementation
- `simple_server.cpp`: Server implementation
- `complie.bat`: Batch file for compilation

## Prerequisites

Before running this project, you need to have the following installed:

- C++ compiler (e.g., GCC, MinGW, or Visual Studio with C++ support)
- Basic networking libraries (included in standard C++ libraries)

## How to Run

### Option 1: Using provided executables

1. Run the server:
   ```
   ./simple_server.exe
   ```

2. Run the client in a separate terminal:
   ```
   ./simple_client.exe
   ```

3. Run the game:
   ```
   ./xox_game.exe
   ```

### Option 2: Compile and run from source

1. Compile the project:
   ```
   ./complie.bat
   ```
   
   Alternatively, you can compile manually:
   ```
   g++ -o xox_game.exe main.cpp board.cpp game.cpp player.cpp network.cpp
   g++ -o simple_server.exe simple_server.cpp network.cpp
   g++ -o simple_client.exe simple_client.cpp network.cpp
   ```

2. Follow the steps from Option 1 to run the executables.

## Network Setup

The game uses a client-server architecture:

1. Start the server first to listen for incoming connections
2. Run the client to connect to the server
3. Start the game to begin playing

By default, the server listens on localhost. If you want to play over a network, you may need to modify the network settings in the code.

## Gameplay Instructions

1. The game is a classic Tic-Tac-Toe implementation
2. Players take turns placing their marks (X or O) on a 3x3 grid
3. The first player to get three of their marks in a row (horizontally, vertically, or diagonally) wins
4. If all cells are filled and no player has won, the game ends in a draw

## Troubleshooting

- If you encounter connection issues, ensure that no firewall is blocking the connection.
- If compilation fails, ensure that all required dependencies are installed.
- For Windows users, you may need to allow the executable through Windows Defender Firewall.

## Author

Eshanya
