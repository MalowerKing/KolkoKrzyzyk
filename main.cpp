#include <SDL3/SDL.h>
#include <cstdio>
#include <iostream>
#include <cstdlib>
#include <string>

#include <unistd.h>
#include <fcntl.h>
#include <sys/select.h>

enum GameState {
    MENU,
    PLAYING,
    GAME_OVER
};

// Forward declaration
class Renderer;

// Game Logic Class - handles all game state and logic
class GameLogic {
private:
    GameState currentState;
    int boardSize;
    int winLength;
    
    char** board;
    int cursorX, cursorY;
    char currentPlayer;
    char gameResult;
    
    // Menu variables
    int tempBoardSize;
    int tempWinLength;
    bool editingBoardSize;

public:
    GameLogic() : currentState(MENU), boardSize(3), winLength(3),
                  board(nullptr), cursorX(0), cursorY(0), currentPlayer('X'), gameResult(' '),
                  tempBoardSize(3), tempWinLength(3), editingBoardSize(true) {}
    
    ~GameLogic() {
        cleanupBoard();
    }
    
    void initBoard(int size) {
        if (board) cleanupBoard();
        
        board = new char*[size];
        for (int i = 0; i < size; ++i) {
            board[i] = new char[size];
            for (int j = 0; j < size; ++j) {
                board[i][j] = ' ';
            }
        }
    }
    
    void cleanupBoard() {
        if (board) {
            for (int i = 0; i < boardSize; ++i) {
                delete[] board[i];
            }
            delete[] board;
            board = nullptr;
        }
    }
    
    char checkWin(int size) {
        // Check rows
        for (int i = 0; i < size; ++i) {
            for (int j = 0; j <= size - winLength; ++j) {
                if (board[i][j] != ' ') {
                    bool win = true;
                    for (int k = 1; k < winLength; ++k) {
                        if (board[i][j + k] != board[i][j]) {
                            win = false;
                            break;
                        }
                    }
                    if (win) return board[i][j];
                }
            }
        }
        
        // Check columns
        for (int j = 0; j < size; ++j) {
            for (int i = 0; i <= size - winLength; ++i) {
                if (board[i][j] != ' ') {
                    bool win = true;
                    for (int k = 1; k < winLength; ++k) {
                        if (board[i + k][j] != board[i][j]) {
                            win = false;
                            break;
                        }
                    }
                    if (win) return board[i][j];
                }
            }
        }
        
        // Check diagonals (top-left to bottom-right)
        for (int i = 0; i <= size - winLength; ++i) {
            for (int j = 0; j <= size - winLength; ++j) {
                if (board[i][j] != ' ') {
                    bool win = true;
                    for (int k = 1; k < winLength; ++k) {
                        if (board[i + k][j + k] != board[i][j]) {
                            win = false;
                            break;
                        }
                    }
                    if (win) return board[i][j];
                }
            }
        }
        
        // Check diagonals (top-right to bottom-left)
        for (int i = 0; i <= size - winLength; ++i) {
            for (int j = winLength - 1; j < size; ++j) {
                if (board[i][j] != ' ') {
                    bool win = true;
                    for (int k = 1; k < winLength; ++k) {
                        if (board[i + k][j - k] != board[i][j]) {
                            win = false;
                            break;
                        }
                    }
                    if (win) return board[i][j];
                }
            }
        }
        
        // Check for draw
        bool full = true;
        for (int i = 0; i < size; ++i) {
            for (int j = 0; j < size; ++j) {
                if (board[i][j] == ' ') {
                    full = false;
                    break;
                }
            }
            if (!full) break;
        }
        
        return full ? 'D' : ' ';
    }
    
 
    
    void handleGameInput(SDL_Event& e) {
        if (e.type == SDL_EVENT_KEY_DOWN) {
            switch (e.key.key) {
                case SDLK_W:
                case SDLK_UP:
                    if (cursorY > 0) cursorY--;
                    break;
                case SDLK_S:
                case SDLK_DOWN:
                    if (cursorY < boardSize - 1) cursorY++;
                    break;
                case SDLK_A:
                case SDLK_LEFT:
                    if (cursorX > 0) cursorX--;
                    break;
                case SDLK_D:
                case SDLK_RIGHT:
                    if (cursorX < boardSize - 1) cursorX++;
                    break;
                case SDLK_RETURN:
                case SDLK_SPACE:
                    if (board[cursorY][cursorX] == ' ') {
                        board[cursorY][cursorX] = currentPlayer;
                        currentPlayer = (currentPlayer == 'X') ? 'O' : 'X';
                    }
                    break;
                case SDLK_ESCAPE:
                    currentState = MENU;
                    break;
            }
        }
    }
    
   
    void startNewGame() {
        initBoard(boardSize);
        cursorX = cursorY = 0;
        currentPlayer = 'X';
        gameResult = ' ';
        currentState = PLAYING;
    }
    
    void update() {
        if (currentState == PLAYING) {
            gameResult = checkWin(boardSize);
            if (gameResult != ' ') {
                currentState = GAME_OVER;
            }
        }
    }
    
    // Getters for renderer
    GameState getCurrentState() const { return currentState; }
    int getBoardSize() const { return boardSize; }
    int getWinLength() const { return winLength; }
    char** getBoard() const { return board; }
    int getCursorX() const { return cursorX; }
    int getCursorY() const { return cursorY; }
    char getCurrentPlayer() const { return currentPlayer; }
    char getGameResult() const { return gameResult; }
    int getTempBoardSize() const { return tempBoardSize; }
    int getTempWinLength() const { return tempWinLength; }
    bool isEditingBoardSize() const { return editingBoardSize; }

        void set_winLength(int lenght) {
                winLength = lenght;
        };

        void set_boardSize(int size) {
                boardSize = size;
        };

        void set_currentState(GameState state) {
                currentState = state;
        }

};

// Terminal Interface Class - handles terminal I/O
class TerminalInterface {
public:
    
        GameLogic gameSettings;

        int input() {
    int value;
    char c;

    while (true) {
        std::cin >> value;

        // Check if input is valid and no extra characters are left in the buffer
        if (std::cin && std::cin.get(c) && c == '\n') {
            if (value >= 3) {
                return value;
            } else {
                std::cout << "The number must be greater than 3.\n";
            }
        } else {
            std::cout << "Invalid input. Please enter a valid integer.\n";
            std::cin.clear();
            while (std::cin.get() != '\n'); // discard the rest of the input
        }
    }
}

    void showMenu(GameLogic& gameLogic) {
        
        std::cout << "========================================\n";
        std::cout << "      ENHANCED TIC-TAC-TOE GAME\n";
        std::cout << "========================================\n\n";
        
        std::cout << "Configure Game Settings:\n\n";
        
        std::cout << "Board Size: \n";
       gameLogic.set_boardSize(input());
        std::cout << "Length of the line to win: \n";
        gameLogic.set_winLength(input());
        
        gameLogic.initBoard(gameLogic.getBoardSize());
        gameLogic.set_currentState(PLAYING);
    }
    
    void showGameOver(GameLogic& gameLogic) {
        
        std::cout << "========================================\n";
        std::cout << "            GAME OVER!\n";
        std::cout << "========================================\n\n";
        
        char gameResult = gameLogic.getGameResult();
        
        if (gameResult == 'D') {
            std::cout << "           IT'S A DRAW!\n";
        } else {
            std::cout << "        PLAYER " << gameResult << " WINS!\n";
        }


        std::cout << "Click anything\n";
        std::getchar();

        std::cout << "Do you want to Continue???\n";
        std::cout << "Y : N ";
        char choice;

            while (true)
    {
        std::cin >> choice;
        switch (choice)
        {
            case 'y': case 'Y':
                // --- YES pressed ---
                std::cout << "You chose YES\n";
                gameLogic.set_currentState(MENU);
                return;

            case 'n': case 'N':
                // --- NO pressed ---
                std::cout << "You chose NO\n";
                exit(0);
                return;

            default:
                std::cout << "Invalid input. Please press Y or N: ";
                // loop again
        }
    }
    }
};

// SDL Renderer Class - handles only game board rendering
class GameRenderer {
private:
    SDL_Window* window;
    SDL_Renderer* renderer;
    int windowSize;
    int cellSize;

public:
    GameRenderer() : window(nullptr), renderer(nullptr), windowSize(600), cellSize(0) {}
    
    ~GameRenderer() {
        cleanup();
    }
    
    bool init() {
        if (!SDL_Init(SDL_INIT_VIDEO != 0)) {
            std::cerr << "SDL initialization failed: " << SDL_GetError() << std::endl;
            return false;
        }
        
        window = SDL_CreateWindow("Enhanced Tic-Tac-Toe - Game Board", 
                                 windowSize, windowSize, SDL_WINDOW_RESIZABLE);
        if (!window) {
            std::cerr << "Window creation failed: " << SDL_GetError() << std::endl;
            return false;
        }
        
        renderer = SDL_CreateRenderer(window, nullptr);
        if (!renderer) {
            std::cerr << "Renderer creation failed: " << SDL_GetError() << std::endl;
            return false;
        }
        
        return true;
    }
    
    void cleanup() {
        if (renderer) SDL_DestroyRenderer(renderer);
        if (window) SDL_DestroyWindow(window);
        SDL_Quit();
    }
    
    void renderGame(const GameLogic& gameLogic) {
        int boardSize = gameLogic.getBoardSize();
        char** board = gameLogic.getBoard();
        cellSize = windowSize / boardSize;
        
        // Clear with white background
        SDL_SetRenderDrawColor(renderer, 255, 255, 255, 255);
        SDL_RenderClear(renderer);
        
        // Draw grid lines
        SDL_SetRenderDrawColor(renderer, 0, 0, 0, 255);
        for (int i = 1; i < boardSize; ++i) {
            SDL_RenderLine(renderer, i * cellSize, 0, i * cellSize, windowSize);
            SDL_RenderLine(renderer, 0, i * cellSize, windowSize, i * cellSize);
        }
        
        // Draw marks
        for (int y = 0; y < boardSize; ++y) {
            for (int x = 0; x < boardSize; ++x) {
                int px = x * cellSize;
                int py = y * cellSize;
                int margin = cellSize / 6;
                
                if (board[y][x] == 'X') {
                    SDL_SetRenderDrawColor(renderer, 255, 0, 0, 255);
                    // Draw thicker X
                    for (int i = 0; i < 3; ++i) {
                        SDL_RenderLine(renderer, px + margin + i, py + margin, 
                                     px + cellSize - margin + i, py + cellSize - margin);
                        SDL_RenderLine(renderer, px + cellSize - margin + i, py + margin, 
                                     px + margin + i, py + cellSize - margin);
                    }
                } else if (board[y][x] == 'O') {
                    SDL_SetRenderDrawColor(renderer, 0, 0, 255, 255);
                    // Draw circle (approximated with rectangles)
                    SDL_FRect outerRect = {(float)(px + margin), (float)(py + margin), 
                                         (float)(cellSize - 2*margin), (float)(cellSize - 2*margin)};
                    SDL_FRect innerRect = {(float)(px + margin + 10), (float)(py + margin + 10), 
                                         (float)(cellSize - 2*margin - 20), (float)(cellSize - 2*margin - 20)};
                    SDL_RenderRect(renderer, &outerRect);
                    SDL_SetRenderDrawColor(renderer, 255, 255, 255, 255);
                    SDL_RenderFillRect(renderer, &innerRect);
                }
            }
        }
        
        // Draw cursor
        int cursorX = gameLogic.getCursorX();
        int cursorY = gameLogic.getCursorY();
        SDL_FRect cursor = {(float)(cursorX * cellSize + 5), (float)(cursorY * cellSize + 5), 
                           (float)(cellSize - 10), (float)(cellSize - 10)};
        SDL_SetRenderDrawColor(renderer, 0, 255, 0, 255);
        SDL_RenderRect(renderer, &cursor);
        
        SDL_RenderPresent(renderer);
        
        // Also show current player in terminal
        std::cout << "\rCurrent Player: " << gameLogic.getCurrentPlayer() << " | Use WASD/Arrows to move, SPACE/ENTER to place, ESC for menu" << std::flush;
    }
    
    void hideWindow() {
        if (window) {
            SDL_HideWindow(window);
        }
    }
    
    void showWindow() {
        if (window) {
            SDL_ShowWindow(window);
        }
    }
};

// Main Game Class - coordinates between logic, terminal, and SDL rendering
class TicTacToe {
private:
    GameLogic gameLogic;
    TerminalInterface terminal;
    GameRenderer gameRenderer;

public:
    TicTacToe() {}
    
    void run() {
        if (!gameRenderer.init()) return;
        
        bool running = true;
        SDL_Event e;
        
        gameRenderer.hideWindow(); // Start with window hidden
        
        while (running) {
            switch (gameLogic.getCurrentState()) {
                case MENU: {
                    gameRenderer.hideWindow();
                    terminal.showMenu(gameLogic);
                    
                    SDL_Delay(50); // Small delay to prevent excessive CPU usage
                    break;
                }
                
                case PLAYING: {
                    gameRenderer.showWindow();
                    
                    // Handle SDL events for game
                    while (SDL_PollEvent(&e)) {
                        if (e.type == SDL_EVENT_QUIT) {
                            running = false;
                            continue;
                        }
                        gameLogic.handleGameInput(e);
                    }
                    
                    // Update game logic
                    gameLogic.update();
                    
                    // Render game
                    gameRenderer.renderGame(gameLogic);
                    SDL_Delay(16);
                    break;
                }
                
                case GAME_OVER: {
                    gameRenderer.hideWindow();
                    terminal.showGameOver(gameLogic);
                    
                    SDL_Delay(50); // Small delay to prevent excessive CPU usage
                    break;
                }
            }
        }
    }
};

int main(int argc, char* argv[]) {
    TicTacToe game;
    game.run();
    return 0;
}
