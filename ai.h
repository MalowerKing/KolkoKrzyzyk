#include <algorithm>
#include <climits>
#include <cstdint>
#include <cstdlib>
#include <ostream>
#include <vector>
#include <utility>
#include <iostream>
#include <chrono>
using namespace std;

class AI_Player {
    public:
        AI_Player(char* boardGiven[], int size, int win) {
                board = new char*[size];
                for(int i = 0; i < size; i++){
                        board[i] = new char[size];
                }

                for(int i = 0; i < size; i++) {
                        for(int j = 0; j < size; j++){
                                board[i][j] = boardGiven[i][j];
                        }
                }

                winCondition = win;
                boardSize = size;
                centreOfBoard = (size -1)/2;
        };
        
        pair<int, int> move(char player) {
                int evaluation = 0;
                pair<int, pair<int, int>> bestValue;

 
                int maxDepth = 3;

                if( player == 'X') bestValue.first = INT_MIN;
                else bestValue.first = INT_MAX;

                int promoteCentre = 0;

                auto start = std::chrono::high_resolution_clock::now();
                while (1) {
                int alpha = INT_MIN;
                int beta = INT_MAX;

                //Generate possible moves
               for (int i = 0; i < boardSize; i++) {
                        for(int j = 0; j < boardSize; j++) {
                                if(board[i][j] == ' ') {
                                        board[i][j] = player;
                                        if(player == 'X') evaluation = minMax(0, 'O', alpha, beta, maxDepth);
                                        else evaluation = minMax(0, 'X', alpha, beta, maxDepth);

                                        if (player=='X') alpha = max(alpha, evaluation);
                                        else             beta  = min(beta, evaluation);


                                        board[i][j] = ' ';

                                        if( player == 'X') {
                                            if(bestValue.first < evaluation - promoteCentre) bestValue = {evaluation - promoteCentre, {i, j}};
                                        }
                                        else {
                                            if(bestValue.first > evaluation + promoteCentre) bestValue = {evaluation + promoteCentre, {i, j}};
                                        }
                                };
                        };
                }

                auto now = std::chrono::high_resolution_clock::now();
                auto elapsed = std::chrono::duration_cast<std::chrono::seconds>(now - start).count();

                if (elapsed >= 30 || maxDepth > boardSize * boardSize) {
                        break;
                }
                
                cout << maxDepth << " | " << bestValue.first << endl;
                maxDepth++;

                }

               return  bestValue.second;

        };

        int minMax(int depth, char player, int alpha, int beta, int maxDepth) {
                int evaluation = evaluate();
                if(evaluation == 500 || evaluation == -500 || evaluation == -1 || depth >= maxDepth){
                    if(evaluation > 0) return evaluation - depth;
                    else if(evaluation < 0) return evaluation + depth;
                    else return 0;
                }

                int bestValue = 0;

                if(player == 'X') bestValue = -1000;
                else bestValue = 1000;

                //Generate possible moves
               for (int i = 0; i<boardSize; i++) {
                        for(int j = 0; j < boardSize; j++) {
                                if(board[i][j] == ' ') {
                                    board[i][j] = player;

                                    if(player == 'X') evaluation = minMax(depth+1, 'O', alpha, beta, maxDepth);
                                    else evaluation = minMax(depth+1, 'X', alpha, beta, maxDepth);

                                    if (player == 'X') {
                                        bestValue = max(bestValue, evaluation);
                                        alpha = max(alpha, bestValue);
                                    } else {
                                        bestValue = min(bestValue, evaluation);
                                        beta = min(beta, bestValue);
                                    }
                                    
                                    board[i][j] = ' ';
                                    if(beta <= alpha) break;
                                };
                        };
                        if(beta <= alpha) break;
                } 
                return bestValue;
        };
        
~AI_Player() {
    for (int i = 0; i < boardSize; ++i)
        delete[] board[i];
    delete[] board;
}



    private:
        char** board;
        int boardSize;
        int winCondition;
        int centreOfBoard;

void printBoard() {
        for(int i = 0; i < boardSize; i++) {
                for(int j = 0; j <  boardSize; j++) cout << board[i][j];
                cout << endl;
        }
        };

int evaluate() {
    bool draw = true;
    pair<int, char> maxCount = {0, ' '};
    // For every cell, if it’s X or O, try scanning forward in each dir
    for (int i = 0; i < boardSize; ++i) {
        for (int j = 0; j < boardSize; ++j) {

            char start = board[i][j];
            if (start == ' ')  {draw = false; continue;}

            int count = 0;


            int x = 0;
            int y = 0;

            //Checking forward in X
            for(int step = 0; step < winCondition; step++) {
                x = i+step;

                if(x >= boardSize) break;
                if(board[x][j] == start) count++;
                else break;
            }
            
            if (count >= winCondition) {
                return (start == 'X') ? +500 : -500;
            }
            if(count >= maxCount.first) maxCount = {count, start};

            count = 0;

            //Checking downward in Y
            for(int step = 0; step < winCondition; step++) {
                y = j+step;

                if(y >= boardSize) break;
                if(board[i][y] == start) count++;
                else break;
            }

            if (count >= winCondition) {
                return (start == 'X') ? +500 : -500;
            }
            if(count >= maxCount.first) maxCount = {count, start};



            count = 0;

            //Checking Horizontal in X Y
            for(int step = 0; step < winCondition; step++) {
                x = i+step;
                y = j+step;

                if(y >= boardSize || x >= boardSize) break;
                if(board[x][y] == start) count++;
                else break;
            }

            if (count >= winCondition) {
                return (start == 'X') ? +500 : -500;
            }
            if(count >= maxCount.first) maxCount = {count, start};

            count = 0;
           //Checking Horizontal in -X Y
            for(int step = 0; step < winCondition; step++) {
                x = i+step;
                y = j-step;

                if(y < 0 || x >= boardSize) break;
                if(board[x][y] == start) count++;
                else break;
            }

            if (count >= winCondition) {
                return (start == 'X') ? +500 : -500;
            }
            if(count >= maxCount.first) maxCount = {count, start};
            }
        }
        if(draw) return -1;
        else {
            if(winCondition - 1 == maxCount.first) return (maxCount.second == 'X') ? +10 : -10;
            else return 0;
        }
    }


};
