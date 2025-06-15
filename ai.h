#include <algorithm>
#include <climits>
#include <cstdint>
#include <cstdlib>
#include <ostream>
#include <vector>
#include <utility>
#include <iostream>
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
        };
        
        pair<int, int> move(char player) {
                int evaluation = 0;
                pair<int, pair<int, int>> bestValue;

                int alpha = INT_MIN;
                int beta = INT_MAX;

                if( player == 'X') bestValue.first = INT_MIN;
                else bestValue.first = INT_MAX;

                //Generate possible moves
               for (int i = 0; i < boardSize; i++) {
                        for(int j = 0; j < boardSize; j++) {
                                if(board[i][j] == ' ') {
                                        board[i][j] = player;
                                        printBoard();
                                        if(player == 'X') evaluation = minMax(0, 'O', alpha, beta);
                                        else evaluation = minMax(0, 'X', alpha, beta);

                                        if (player=='X') alpha = max(alpha, evaluation);
                                        else             beta  = min(beta, evaluation);

                                        cout << evaluation << endl;

                                        board[i][j] = ' ';

                                        if( player == 'X') {
                                            if(bestValue.first < evaluation) bestValue = {evaluation, {i, j}};
                                        }
                                        else {
                                            if(bestValue.first > evaluation) bestValue = {evaluation, {i, j}};
                                        }
                                };
                        };
                } 

                cout << "{" << bestValue.first << "} --- {" << bestValue.second.first << " | " << bestValue.second.second << "}" << endl;
               return  bestValue.second;

        };

        int minMax(int depth, char player, int alpha, int beta) {
                int evaluation = evaluate();
                if(evaluation == 100 || evaluation == -100 || evaluation == -1 || depth >= 9){
                    //cout << "GAME OVER: " << evaluation<< endl;
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

                                    if(player == 'X') evaluation = minMax(depth+1, 'O', alpha, beta);
                                    else evaluation = minMax(depth+1, 'X', alpha, beta);

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
                //cout << bestValue << ": for " << player << endl;
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
                return (start == 'X') ? +100 : -100;
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
                return (start == 'X') ? +100 : -100;
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
                return (start == 'X') ? +100 : -100;
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
                return (start == 'X') ? +100 : -100;
            }
            if(count >= maxCount.first) maxCount = {count, start};
            }
        }
        if(draw) return -1;
        else {
            if(winCondition - 1 == maxCount.first) return (maxCount.second == 'X') ? +20 : -20;
            else if(winCondition -2 == maxCount.first) return (maxCount.second == 'X') ? +10 : -10;
            else if(winCondition -3 == maxCount.first) return (maxCount.second == 'X') ? +5 : -5;
            else return 0;
        }
    }


};
