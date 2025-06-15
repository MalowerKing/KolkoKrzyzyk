#include <SDL3/SDL.h>
#include <cstdio>
#include <iostream>
#include <cstdlib>
#include <string>
#include <utility>
#include <vector>

#include <unistd.h>
#include <fcntl.h>
#include <sys/select.h>

#include "ai.h"

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

    char player;
    char currentPlayer;
    char gameResult;
    int gameRounds = 0;
    
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
    
// Call this right after you place board[r][c] = player;
// player is either 'X' or 'O'.
char checkFrom(int r, int c) {
    char p = board[r][c];
    gameRounds++;
    if (p == ' ') return ' ';          // sanity

    // Four direction‐pairs: {dr,dc} are the “forward” deltas,
    // and we’ll implicitly handle the backwards by negating.
    const std::vector<std::pair<int,int>> dirs = {
        {0,1},   // horizontal
        {1,0},   // vertical
        {1,1},   // diag down-right
        {1,-1}   // anti-diag down-left
    };

    auto inb = [&](int rr, int cc){
        return rr>=0 && rr<boardSize && cc>=0 && cc<boardSize;
    };

    for (auto [dr,dc] : dirs) {
        int count = 1;  // start with the current cell

        // walk forward
        for (int step = 1; step < winLength; ++step) {
            int nr = r + dr*step;
            int nc = c + dc*step;
            if (!inb(nr,nc) || board[nr][nc] != p) break;
            ++count;
        }
        // walk backward
        for (int step = 1; step < winLength; ++step) {
            int nr = r - dr*step;
            int nc = c - dc*step;
            if (!inb(nr,nc) || board[nr][nc] != p) break;
            ++count;
        }

        if (count >= winLength) 
            return p;   // immediate exit on win
    }

    // No win found on those four lines.  You can still check draw
    // with a separate "full" flag if you like, or maintain a move
    // counter that hits size*size.

    if(gameRounds == boardSize*boardSize) return 'D';
    return ' ';
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
                                gameResult = checkFrom(cursorY, cursorX);

                                update();

                                currentPlayer = (currentPlayer == 'X') ? 'O' : 'X'; 
                    }
                    break;
                case SDLK_ESCAPE:
                    currentState = MENU;
                    break;
            }
        }
    }

        void handleAIMove() {
                AI_Player bot(board, boardSize ,winLength);


                std::cout << currentPlayer << std::endl;
                std::pair<int, int> result = bot.move(currentPlayer);

                board[result.first][result.second] = currentPlayer;
                gameResult = checkFrom(result.first, result.second);

                currentPlayer = (currentPlayer == 'X') ? 'O' : 'X'; 
                update();
        }
    
   
    void startNewGame(int size) {
        initBoard(boardSize);
        cursorX = cursorY = 0;
        currentPlayer = 'X';
        gameResult = ' ';
        currentState = PLAYING;
    }
    
    void update() {
        if (currentState == PLAYING) {
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
        char getWhosPlayer() const { return player;}

        void set_winLength(int lenght) {
                winLength = lenght;
        };

        void set_boardSize(int size) {
                boardSize = size;
        };

        void set_currentState(GameState state) {
                currentState = state;
        }

        void set_whosPlayer(char letter){ player = letter;}
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

        std::cout << "Choose Player (X or O): \n";
        char choice;
        while(1){
        std::cin >> choice;
        switch (choice)
        {
            case 'x': case 'X':
                        std::cout << "Choosen Player X: \n";
                        gameLogic.set_whosPlayer('X'); 
                        gameLogic.startNewGame(gameLogic.getBoardSize());
                        gameLogic.set_currentState(PLAYING);
                        return;
            case 'o': case 'O':
                        std::cout << "Choosen Player O: \n";
                        gameLogic.set_whosPlayer('O'); 
                        gameLogic.startNewGame(gameLogic.getBoardSize());
                        gameLogic.set_currentState(PLAYING);
                        return;
            default:
                std::cout << "Invalid input. Please press X or O: \n";
                // loop again
        }
        }

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


        std::cout << "Enter\n";
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
    GameRenderer() : window(nullptr), renderer(nullptr), cellSize(0) {}
    
    ~GameRenderer() {
        cleanup();
    }
    
    bool init(int size) {
        windowSize = size;

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
                                         (float)(cellSize - 2*margin ), (float)(cellSize - 2*margin - 20)};
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

        
        bool running = true;
        SDL_Event e;
        
        if (!gameRenderer.init(1000)) return;

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
    gameRenderer.renderGame(gameLogic); // Initial render if needed

    if (gameLogic.getWhosPlayer() == 'X') {
        // Wait for player input
        while (SDL_WaitEvent(&e)) {
            if (e.type == SDL_EVENT_QUIT) {
                running = false;
                break;
            }

            if(gameLogic.getWhosPlayer() == gameLogic.getCurrentPlayer()){
                gameLogic.handleGameInput(e); 
                gameRenderer.renderGame(gameLogic);
                break; // Exit input wait after full turn
            }
            else {
                gameLogic.handleAIMove();
                gameRenderer.renderGame(gameLogic);
            }
        }
    } else {
        while (SDL_WaitEvent(&e)) {
            if (e.type == SDL_EVENT_QUIT) {
                running = false;
                break;
            }
        if('X' == gameLogic.getCurrentPlayer()){
            gameLogic.handleAIMove();
            gameRenderer.renderGame(gameLogic);
        }
        else {
            gameLogic.handleGameInput(e);
            gameRenderer.renderGame(gameLogic);
        }
        }
    }

    SDL_Delay(16); // Cap frame rate
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
