@echo off
echo Compiling XOX Multiplayer Game...
g++ -o xox_game.exe board.cpp player.cpp network.cpp game.cpp main.cpp -lws2_32
if %ERRORLEVEL% == 0 (
  echo Compilation successful!
  echo.
  echo To run the game, type: xox_game.exe
  echo.
  echo For testing on the same computer:
  echo 1. Run the game twice in separate command windows
  echo 2. Select option 1 (Server) in one window
  echo 3. Select option 2 (Client) in the other window
  echo 4. For client, use 127.0.0.1 as server IP
  echo.
  echo Press any key to exit...
) else (
  echo Compilation failed. Please check error messages above.
  echo Press any key to exit...
)
pause