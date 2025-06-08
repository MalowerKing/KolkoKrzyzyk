#include <algorithm>
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

        pair<int, int> minMax(int depth, char player) {
                vector<pair<int, int>> possibleMoves;
                pair<int, int> temp;
                vector<int> evaluations;
                int evaluation = 0;

                //Generate possible moves
               for (int i = 0; i<boardSize; i++) {
                        for(int j = 0; j < boardSize; j++) {
                                if(board[i][j] == ' ') {
                                        temp = {i, j};
                                        possibleMoves.push_back(temp);
                                };
                        };
                } 

                if(possibleMoves.empty() || depth == 4){
                    temp = {-1, -1};
                    return temp;
                }

                for (pair<int, int> move : possibleMoves) {
                        board[move.first][move.second] = player;
                        evaluation = evaluate();
                        minMax(depth+1, (player == 'X') ? 'O' : 'X');
                        board[move.first][move.second] = ' ';

                        if(player == 'X') evaluations.push_back(evaluation-depth);
                        else evaluations.push_back(evaluation+depth);
                }
                int index = 0;
                int value = 0;
                
                if(player == 'X') {
                        
                        int max = -50;
                        int counter = 1;
                        for (int eval : evaluations) {
                                 if(eval > max) {
                                        max = eval;
                                        index = counter;
                                        value = max;
                                }
                                counter++;
                        }
                }
                else{
                        int min = 50; 
                        int counter = 1;
                        for (int eval : evaluations) {
                                if(eval < min) {
                                        min = eval;
                                        index = counter;
                                        value = min;
                                }
                                counter ++;
                        }
                };
                cout << value  << " | {" << possibleMoves[index].first<<"," << possibleMoves[index].second << endl ;

                return possibleMoves[index];

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
    // Check rows
    for(int i = 0; i < boardSize; i++) {
        int counter = 1;
        char prev = board[i][0];
        if(prev == ' ') {
            counter = 0;
            prev = ';';
        }
        
        for(int j = 1; j < boardSize; j++) {
            if(board[i][j] != ' ') {
                if(prev == board[i][j]) {
                    counter++;
                } else {
                    counter = 1;
                    prev = board[i][j];
                }
                
                if(counter >= winCondition) {
                    return (prev == 'X') ? 20 : -20;
                }
            } else {
                counter = 0;
                prev = ';';
            }
        }
    }
    
    // Check columns
    for(int j = 0; j < boardSize; j++) {
        int counter = 1;
        char prev = board[0][j];
        if(prev == ' ') {
            counter = 0;
            prev = ';';
        }
        
        for(int i = 1; i < boardSize; i++) {
            if(board[i][j] != ' ') {
                if(prev == board[i][j]) {
                    counter++;
                } else {
                    counter = 1;
                    prev = board[i][j];
                }
                
                if(counter >= winCondition) {
                    return (prev == 'X') ? 20 : -20;
                }
            } else {
                counter = 0;
                prev = ';';
            }
        }
    }
    
    // Check main diagonals (top-left to bottom-right)
    for(int start = 0; start <= boardSize - winCondition; start++) {
        // Check diagonal starting from (start, 0)
        if(start < boardSize) {
            int counter = 1;
            char prev = board[start][0];
            if(prev == ' ') {
                counter = 0;
                prev = ';';
            }
            
            for(int k = 1; start + k < boardSize; k++) {
                if(board[start + k][k] != ' ') {
                    if(prev == board[start + k][k]) {
                        counter++;
                    } else {
                        counter = 1;
                        prev = board[start + k][k];
                    }
                    
                    if(counter >= winCondition) {
                        return (prev == 'X') ? 20 : -20;
                    }
                } else {
                    counter = 0;
                    prev = ';';
                }
            }
        }
        
        // Check diagonal starting from (0, start+1)
        if(start + 1 < boardSize) {
            int counter = 1;
            char prev = board[0][start + 1];
            if(prev == ' ') {
                counter = 0;
                prev = ';';
            }
            
            for(int k = 1; k < boardSize && start + 1 + k < boardSize; k++) {
                if(board[k][start + 1 + k] != ' ') {
                    if(prev == board[k][start + 1 + k]) {
                        counter++;
                    } else {
                        counter = 1;
                        prev = board[k][start + 1 + k];
                    }
                    
                    if(counter >= winCondition) {
                        return (prev == 'X') ? 20 : -20;
                    }
                } else {
                    counter = 0;
                    prev = ';';
                }
            }
        }
    }
    
    // Check anti-diagonals (top-right to bottom-left)
    for(int start = winCondition - 1; start < boardSize; start++) {
        // Check diagonal starting from (0, start)
        int counter = 1;
        char prev = board[0][start];
        if(prev == ' ') {
            counter = 0;
            prev = ';';
        }
        
        for(int k = 1; k < boardSize && start - k >= 0; k++) {
            if(board[k][start - k] != ' ') {
                if(prev == board[k][start - k]) {
                    counter++;
                } else {
                    //counter = 1;
                    prev = board[k][start - k];
                }
                
                if(counter >= winCondition) {
                    return (prev == 'X') ? 20 : -20;
                }
            } else {
                counter = 0;
                prev = ';';
            }
        }
        
        // Check diagonal starting from (start+1-boardSize+1, boardSize-1) 
        if(start < boardSize - 1) {
            int startRow = boardSize - start - 1;
            if(startRow > 0 && startRow < boardSize) {
                counter = 1;
                prev = board[startRow][boardSize - 1];
                if(prev == ' ') {
                    counter = 0;
                    prev = ';';
                }
                
                for(int k = 1; startRow + k < boardSize && boardSize - 1 - k >= 0; k++) {
                    if(board[startRow + k][boardSize - 1 - k] != ' ') {
                        if(prev == board[startRow + k][boardSize - 1 - k]) {
                            counter++;
                        } else {
                            counter = 1;
                            prev = board[startRow + k][boardSize - 1 - k];
                        }
                        
                        if(counter >= winCondition) {
                            return (prev == 'X') ? 20 : -20;
                        }
                    } else {
                        counter = 0;
                        prev = ';';
                    }
                }
            }
        }
    }
    
    return 0;
}

        

};
