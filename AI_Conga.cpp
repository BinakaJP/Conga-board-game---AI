// AI_Conga.cpp : This file contains the 'main' function. Program execution begins and ends there.
//
#include <iostream>
#include <stdio.h>
#include <stdlib.h>
#include <vector>
#include <algorithm>
#include <io.h>
#include <time.h>
#include <chrono>
using namespace std;
int Depth = 3;
int Evaluation = 4;
const int POSITIVE_I = 100;
const int NEGATIVE_I = -100;
int nodeExplored = 0;
int nodePruned =0;

struct Board {
//to store board block
    int value;
    char player;
};
struct direction {
    //to store in which direction player will move : {0,1},{1,1},{1,0},{-1,-1},{-1,0}{0,-1},{1,-1},{-1,1}
    int x;
    int y;
};
struct moveinput {
    //detail of move : current position is (x,y) //moving into direction {x,y}
    int x;
    int y;
    struct direction dir;
    moveinput() {}
    moveinput(int i, int j, struct direction d) : x(i), y(j), dir(d) {  }
};
struct Pair
{
    //to store movr input and evaluation value of board after taking that move
    moveinput mi;
    int h;
    Pair() {}
    Pair(moveinput mio, int j) : mi(mio), h(j) {  }
};
bool PersonComparerMin(const Pair& p1, const Pair& p2)
{
    //sorting in decreasing order
    return p1.h < p2.h;
}
bool PersonComparerMax(const Pair& p1, const Pair& p2)
{
    //sorting in increasing order
    return p1.h > p2.h;
}
void printboard(Board board[4][4])
{  //to print the board position  
    for (int i = 0; i < 4; i++) {
        for (int j = 0; j < 4; j++) {
            cout << board[i][j].value;
            cout << board[i][j].player;
            cout << "   |";
        }
        cout << endl;
        cout << "___________________________";
        cout << endl;
    }
    cout << endl;
}
void getboardcopy(struct Board newboard[4][4], struct Board board[4][4]) { // to get duplicate board
    for (int i = 0; i < 4; i++) {
        for (int j = 0; j < 4; j++) {
            newboard[i][j].player = board[i][j].player;
            newboard[i][j].value = board[i][j].value;
        }
    }
}
void makingmove(struct moveinput mi, struct Board board[4][4]) { //to make move according to moveinput detail, it changes the board position
    int x = mi.x;                      
    int y = mi.y;
    int dir_x = mi.dir.x;
    int dir_y = mi.dir.y;
    char player = board[x][y].player;
    // check number of available block in given direction
    int moves_x = x;
    int moves_y = y;
    while (true) {
        moves_x += dir_x;
        moves_y += dir_y;
        // stop if the current block position is out of board
        if (moves_x < 0 || moves_y < 0 || moves_x >= 4 || moves_y >= 4) break;
        if (board[moves_x][moves_y].player != player && board[moves_x][moves_y].player != ' ') break;
    };
    // check if the player is blocked and has no move
    if (abs(moves_x - x) <= 1 && abs(moves_y - y) <= 1) {
        return;
    }
    //move the stones according the number of blocks available
    // move all into the first block
    if (abs(moves_x - x) == 2 || abs(moves_y - y) == 2) {
        board[x + dir_x][y + dir_y].player = board[x][y].player;
        board[x + dir_x][y + dir_y].value += board[x][y].value;
        board[x][y].player = ' ';
        board[x][y].value = 0;
    }
    // move one into the first block
    else if (abs(moves_x - x) == 3 || abs(moves_y - y) == 3) {
        board[x + dir_x][y + dir_y].player = board[x][y].player;
        board[x + dir_x][y + dir_y].value += 1;
        board[x][y].value -= 1;
        // then move everything into the second block
        if (board[x][y].value > 0) {
            board[x + 2 * dir_x][y + 2 * dir_y].player = board[x][y].player;
            board[x + 2 * dir_x][y + 2 * dir_y].value += board[x][y].value;
        }
        board[x][y].player = ' ';
        board[x][y].value = 0;
    }
    // move one into the first block
    else {
        board[x + dir_x][y + dir_y].player = board[x][y].player;
        board[x + dir_x][y + dir_y].value += 1;
        board[x][y].value -= 1;

        // then move two into the second block, and if not enough, move one only
        if (board[x][y].value >= 2) {
            board[x + 2 * dir_x][y + 2 * dir_y].player = board[x][y].player;
            board[x + 2 * dir_x][y + 2 * dir_y].value += 2;
            board[x][y].value -= 2;
        }
        else if (board[x][y].value == 1) {
            board[x + 2 * dir_x][y + 2 * dir_y].player = board[x][y].player;
            board[x + 2 * dir_x][y + 2 * dir_y].value += 1;
            board[x][y].value = 0;
        }

        // then the rest to the last block
        if (board[x][y].value > 0) {
            board[moves_x - dir_x][moves_y - dir_y].player = board[x][y].player;
            board[moves_x - dir_x][moves_y - dir_y].value += board[x][y].value;
        }

        board[x][y].player = ' ';
        board[x][y].value = 0;
    }
}
vector<moveinput> getAllMoves(struct Board board[4][4], char player) { //to get list of all available moves for the given player in the argument
    vector<moveinput> List;
    for (int i = 0; i < 4; i++) {
        for (int j = 0; j < 4; j++) {
            if (board[i][j].player != player) continue;
            if (i - 1 >= 0 && j - 1 >= 0) {
                if (board[i - 1][j - 1].player == ' ' || board[i - 1][j - 1].player == player) {
                    direction d;
                    d.x = -1;
                    d.y = -1;
                    List.push_back(moveinput(i, j, d));
                }
            }
            if (i - 1 >= 0) {
                if (board[i - 1][j].player == ' ' || board[i - 1][j].player == player) {
                    direction d;
                    d.x = -1;
                    d.y = 0;
                    List.push_back(moveinput(i, j, d));
                }
            }
            if (i - 1 >= 0 && j + 1 < 4) {
                if (board[i - 1][j + 1].player == ' ' || board[i - 1][j + 1].player == player) {
                    direction d;
                    d.x = -1;
                    d.y = 1;
                    List.push_back(moveinput(i, j, d));
                }
            }
            if (j - 1 >= 0) {
                if (board[i][j - 1].player == ' ' || board[i][j - 1].player == player) {
                    direction d;
                    d.x = 0;
                    d.y = -1;
                    List.push_back(moveinput(i, j, d));
                }
            }
            if (i + 1 < 4 && j + 1 < 4) {
                if (board[i + 1][j + 1].player == ' ' || board[i + 1][j + 1].player == player) {
                    direction d;
                    d.x = 1;
                    d.y = 1;
                    List.push_back(moveinput(i, j, d));
                }
            }
            if (i + 1 < 4) {
                if (board[i + 1][j].player == ' ' || board[i + 1][j].player == player) {
                    direction d;
                    d.x = 1;
                    d.y = 0;
                    List.push_back(moveinput(i, j, d));
                }
            }
            if (i + 1 < 4 && j - 1 >= 0) {
                if (board[i + 1][j - 1].player == ' ' || board[i + 1][j - 1].player == player) {
                    direction d;
                    d.x = 1;
                    d.y = -1;
                    List.push_back(moveinput(i, j, d));
                }
            }
            if (j + 1 < 4) {
                if (board[i][j + 1].player == ' ' || board[i][j + 1].player == player) {
                    direction d;
                    d.x = 0;
                    d.y = 1;
                    List.push_back(moveinput(i, j, d));
                }
            }
        }
    }
   /* random_shuffle(List.begin(), List.end());*/
    return List;
}
void displayavailablemoves(vector<moveinput> vect)
{
    if (vect.empty())
        cout << "\nSorry, no values exist in the vector.";
    else
        for (int count = 0; count < vect.size(); count++)                           //For Loop to Display All available moves detail
        {
            cout << "\nx: " << vect[count].x << "\ny: " << vect[count].y << "\ndir_x: " << vect[count].dir.x << "\\ndir_y:" << vect[count].dir.y << endl;
        }
    cout << endl;
}
int getholdedspace(char player, Board board[4][4]) {     // //remove this function once code is final
    char p = player;
    int x = 0;
    for (int i = 0; i < 4; i++) {
        for (int j = 0; j < 4; j++) {
            if (board[i][j].value > 0 && board[i][j].player == p) {
                x++;
            }

        }
    }
    return x;
}  
void printPlayernextposition(char board[4][4]) {
    cout << "******************" << endl;
    for (int i = 0; i < 4; i++) {
        for (int j = 0; j < 4; j++) {
            cout << board[i][j] << ",";
        }
        cout << "_________________";
        cout << endl;
    }
    cout << endl;
}

int firstEvaluation(Board board[4][4]) {
    int whitecount = getholdedspace('w', board);
    int blackcount = getholdedspace('b', board);
    int x = (blackcount - whitecount);
    if (whitecount == 0) {
        return POSITIVE_I;
    }
    else if (blackcount == 0) {
        return NEGATIVE_I;
    }
    else {
        return x;
    }
    
} 
int secondEvaluation(struct Board board[4][4]) {
    vector<moveinput> blackMoveList = getAllMoves(board, 'b');
    vector<moveinput> whiteMoveList = getAllMoves(board, 'w');
    int blackcount = blackMoveList.size();
    int whitecount = whiteMoveList.size();

    int heuristic = blackcount - whitecount;
    if (whitecount == 0) {
        return POSITIVE_I;
    }
    else if (blackcount == 0) {
        return NEGATIVE_I;
    }
    else {
        return heuristic;
    }

}
int thirdEvaluation(struct Board board[4][4]) {
    char blackPosition[4][4];
    char whitePosition[4][4];
    int blackCount = 0;
    int whiteCount = 0;
    for (int i = 0; i < 4; i++) {
        for (int j = 0; j < 4; j++) {
            blackPosition[i][j] = ' ';
            whitePosition[i][j] = ' ';
        }
        for (int i = 0; i < 4; i++) {
            for (int j = 0; j < 4; j++) {
                if (board[i][j].player == 'b') {
                    if (i - 1 >= 0 && j - 1 >= 0 && board[i - 1][j - 1].player == ' ') {
                        blackPosition[i - 1][j - 1] = 'b';
                    }
                    if (i - 1 >= 0 && board[i - 1][j].player == ' ') { 
                        blackPosition[i - 1][j] = 'b';
                    }
                    if (i - 1 >= 0 && j + 1 < 4 && board[i - 1][j + 1].player == ' ') {
                        blackPosition[i - 1][j + 1] = 'b';
                    }
                    if (j - 1 >= 0 && board[i][j - 1].player == ' ') {
                        blackPosition[i][j - 1] = 'b';
                    }
                    if (j + 1 < 4 && board[i][j + 1].player == ' ') {
                        blackPosition[i][j + 1] = 'b';
                    }
                    if (i + 1 < 4 && j - 1 >= 0 && board[i + 1][j - 1].player == ' ') {
                        blackPosition[i + 1][j - 1] = 'b';
                    }
                    if (i + 1 < 4 && board[i + 1][j].player == ' ') {
                        blackPosition[i + 1][j] = 'b';
                    }
                    if (i + 1 < 4 && j + 1 < 4 && board[i + 1][j + 1].player == ' ') {
                        blackPosition[i + 1][j + 1] = 'b';
                    }
                }
                else if (board[i][j].player == 'w') {
                    if (i - 1 >= 0 && j - 1 >= 0 && board[i - 1][j - 1].player == ' ') {
                        whitePosition[i - 1][j - 1] = 'w';
                    }
                    if (i - 1 >= 0 && board[i - 1][j].player == ' ') {
                        whitePosition[i - 1][j] = 'w';
                    }
                    if (i - 1 >= 0 && j + 1 < 4 && board[i - 1][j + 1].player == ' ') {
                        whitePosition[i - 1][j + 1] = 'w';
                    }
                    if (j - 1 >= 0 && board[i][j - 1].player == ' ') {
                        whitePosition[i][j - 1] = 'w';
                    }
                    if (j + 1 < 4 && board[i][j + 1].player == ' ') {
                        whitePosition[i][j + 1] = 'w';
                    }
                    if (i + 1 < 4 && j - 1 >= 0 && board[i + 1][j - 1].player == ' ') {
                        whitePosition[i + 1][j - 1] = 'w';
                    }
                    if (i + 1 < 4 && board[i + 1][j].player == ' ') {
                        whitePosition[i + 1][j] = 'w';
                    }
                    if (i + 1 < 4 && j + 1 < 4 && board[i + 1][j + 1].player == ' ') {
                        whitePosition[i + 1][j + 1] = 'w';
                    }
                }
            }
        }            
        for (int i = 0; i < 4; i++) {
            for (int j = 0; j < 4; j++) {
                if (blackPosition[i][j] == 'b') {
                    blackCount++;
                }
                if (whitePosition[i][j] == 'w') {
                    whiteCount++;
                }
            }
        }
        int heuristic = blackCount - whiteCount;
        if (whiteCount == 0) {
            return POSITIVE_I;
        }
        else if (blackCount == 0) {
            return NEGATIVE_I;
        }
        else {            
            return heuristic;
        }
    }
}
int fourthEvaluation(struct Board board[4][4]) {
    int h1 = secondEvaluation(board);
    int h2 = thirdEvaluation(board);
   // int h3 = firstEvaluation(board);
    if (h1 == 100 || h2 == 100) {
        return POSITIVE_I;
    }
    else if (h1 == -100 || h2 == -100) {
        return NEGATIVE_I;
    }
    else {
        return h1+h2;
    }
}
int fifthEvaluation(struct Board board[4][4]) {
    int h1 = secondEvaluation(board);
    int h2 = thirdEvaluation(board);
    int h3 = firstEvaluation(board);
    if (h1 == 100 || h2 == 100) {
        return POSITIVE_I;
    }
    else if (h1 == -100 || h2 == -100) {
        return NEGATIVE_I;
    }
    else {
        return h1 + h2 + h3;
    }
}

void moveAndprint(vector<moveinput> vect, Board board[4][4]) {
    if (vect.empty())
        cout << "\nSorry, no values exist in the vector.";
    else
        for (int count = 0; count < vect.size(); count++)                           //For Loop to Display board state 
        {                                                                            //after making All available moves 
            Board b1[4][4];
            getboardcopy(b1, board);
            cout << "\nx: " << vect[count].x << "\ny: " << vect[count].y << "\ndir_x: " << vect[count].dir.x << "\\ndir_y:" << vect[count].dir.y << endl;
            makingmove(vect[count], b1);
            printboard(b1);
          /*  cout << "\nmove:" << count << endl;
            int h = firstHeuristic(b1);*/
            //cout << "\n heuristic:" << h << endl;
           /* int h = bestHeuristic(board);
            cout << "given :" << h;*/
            cout << endl;
            int g = thirdEvaluation(board);
            cout << endl;
            cout << "given :" << g;
            cout << endl;
        }
    cout << endl;
}
void minSorting(vector<moveinput> temp, struct Board board[4][4]) {
    vector<Pair> heuristic;

    for (int i = 0; i < temp.size(); i++) {
        struct Board b1[4][4];
        getboardcopy(b1, board);
        moveinput mi = temp[i];
        makingmove(mi, b1);
        heuristic.push_back(Pair(mi, fourthEvaluation(b1)));
    }
    sort(heuristic.begin(), heuristic.end(), PersonComparerMin);
    for (int i = 0; i < temp.size(); i++) {
        temp[i] = heuristic[i].mi;
    }
}
void maxSorting(vector<moveinput> temp, struct Board board[4][4]) {
    vector<Pair> heuristic;

    for (int i = 0; i < temp.size(); i++) {
        struct Board b1[4][4];
        getboardcopy(b1, board);
        moveinput mi = temp[i];
        makingmove(mi, b1);
        heuristic.push_back(Pair(mi, fourthEvaluation(b1)));
    }
    sort(heuristic.begin(), heuristic.end(), PersonComparerMax);
    for (int i = 0; i < temp.size(); i++) {
        temp[i] = heuristic[i].mi;
    }
}

int evaluateMove(struct Board board[4][4], struct moveinput newMove, int level, char player, int prune) {
    nodeExplored++;
    // make a copy of the board and make the move
    struct Board newBoard[4][4];
    getboardcopy(newBoard, board);
    makingmove(newMove, newBoard);

    if (level == Depth) {  
        switch (Evaluation)
        {
        case 1 :
            return firstEvaluation(newBoard);
            break;
        case 2:
            return secondEvaluation(newBoard);
            break;
        case 3:
            return thirdEvaluation(newBoard);
            break;
        case 4:
            return fourthEvaluation(newBoard);
            break;
        case 5:
            return fifthEvaluation(newBoard);
            break;
        default:
            return fourthEvaluation(newBoard);
            break;
        }
        
    }

    vector<moveinput> moveList = getAllMoves(newBoard, player);
    /*cout <<"no.of available moves at level "<<numMove<<"=="<< moveList.size();*/
    if (moveList.size() == 0) {
        switch (Evaluation)
        {
        case 1:
            return firstEvaluation(newBoard);
            break;
        case 2:
            return secondEvaluation(newBoard);
            break;
        case 3:
            return thirdEvaluation(newBoard);
            break;
        case 4:
            return fourthEvaluation(newBoard);
            break;
        case 5:
            return fifthEvaluation(newBoard);
            break;
        default:
            return fourthEvaluation(newBoard);
            break;
        }
    }

    // traversing the game tree according depth-first
    int result;
    if (player == 'b') { result = NEGATIVE_I; /*maxSorting(moveList, board);*/   }
    else { result = POSITIVE_I;  /*minSorting(moveList, board);*/    }
    char nextPlayer = (player == 'b' ? 'w' : 'b');
    int value = 0;
    for (int i = 0; i < moveList.size(); i++) {
        value = evaluateMove(newBoard, moveList[i], level + 1, nextPlayer, result);
       
        if (player == 'w') {
            // min move
            if (value < prune) { 
                // prune the node
                nodePruned++;
                return value;
            }
            else if (value < result) {
                result = value;
            }
        }
        else {
            // max move
            if (value > prune) {
                // prune the node
                nodePruned++;
                return value;
            }
            else if (value > result) {
                result = value;
            }
        }
    }

    return result;
}
int evaluateMoveImproved(struct Board board[4][4], struct moveinput newMove, int level, char player, int prune) {
    nodeExplored++;
    // make a copy of the board and make the move
    struct Board newBoard[4][4];
    getboardcopy(newBoard, board);
    makingmove(newMove, newBoard);

    if (level == Depth) {
        return fourthEvaluation(newBoard);
    }

    vector<moveinput> moveList = getAllMoves(newBoard, player);
    /*cout <<"no.of available moves at level "<<numMove<<"=="<< moveList.size();*/
    if (moveList.size() == 0) {
        return fourthEvaluation(newBoard);
    }

    // traversing the game tree according depth-first
    int result;
    if (player == 'b') { result = NEGATIVE_I; maxSorting(moveList, board); }
    else { result = POSITIVE_I;  minSorting(moveList, board); }
    char nextPlayer = (player == 'b' ? 'w' : 'b');
    int value = 0;
    for (int i = 0; i < moveList.size(); i++) {
        value = evaluateMove(newBoard, moveList[i], level + 1, nextPlayer, result);

        if (player == 'w') {
            // min move
            if (value < prune) {
                // prune the node
                nodePruned++;
                return value;
            }
            else if (value < result) {
                result = value;
            }
        }
        else {
            // max move
            if (value > prune) {
                // prune the node
                nodePruned++;
                return value;
            }
            else if (value > result) {
                result = value;
            }
        }
    }

    return result;
}
struct moveinput whitemoves(struct Board board[4][4]) {
    vector<moveinput> whitelist = getAllMoves(board, 'w');
    moveinput mi;
    if (whitelist.size() == 0) {
        mi.x = 100;
        return mi;
    }
    else {
        /*random_shuffle(whitelist.begin(), whitelist.end());*/
      // random_shuffle(whitelist.begin(), whitelist.end());
      //  random_shuffle(whitelist.begin(), whitelist.end()); //infinite
      // random_shuffle(whitelist.begin(), whitelist.end());  //infinite
      //random_shuffle(whitelist.begin(), whitelist.end());
        mi = whitelist[rand() % whitelist.size()];
        makingmove(mi, board);
        return mi;
    }

}
struct moveinput blackmoves(struct Board board[4][4]) {
    vector<moveinput> blacklist = getAllMoves(board, 'b');
    // cout << "available no.of moves now" << blacklist.size() << endl;
    int alpha = NEGATIVE_I;
    moveinput mi;
    int score;

    if (blacklist.size() == 0) {
        mi.x = 100;
        return mi;
    }

    for (int i = 0; i < blacklist.size(); i++) {
        // cout<<"checking for ="<< "\nx: " << blacklist[i].x << "\ny: " << blacklist[i].y << "\ndir_x: " << blacklist[i].dir.x << "\\ndir_y:" << blacklist[i].dir.y << endl;
        score = evaluateMove(board, blacklist[i], 0, 'b', NEGATIVE_I);
        if (score > alpha) {
            mi = blacklist[i];
            alpha = score;
        }
    }

    makingmove(mi, board);

    return mi;
}
void initializeBoard(struct Board board[4][4]) {
    for (int i = 0; i < 4; i++) {
        for (int j = 0; j < 4; j++) {
            board[i][j].value = 0;
            board[i][j].player = ' ';

        }
    }
    board[0][0].value = 10;
    board[3][3].value = 10;
    board[0][0].player = 'b';
    board[3][3].player = 'w';
}
void gameplay(struct Board board[4][4]) {
    moveinput mi;
    int movecount = 0;

    while (true)
    {
        mi = blackmoves(board);
        movecount++;
        if (mi.x != 100) {
            cout << movecount << ": Black has moved from position (" << mi.x << "," << mi.y << ") in direction x: " << mi.dir.x << "direction_y:" << mi.dir.y << endl;
        }
        else {
            cout << "white is Winner!!" << endl;
            break;
        }
        printboard(board);
        movecount++;
        mi = whitemoves(board);
        if (mi.x != 100) {
            cout << movecount << ": White has moved from position (" << mi.x << "," << mi.y << ") in direction x: " << mi.dir.x << "direction_y:" << mi.dir.y << endl;
        }
        else {
            cout << "Black is Winner !!" << endl;
            break;
        }
        printboard(board);
    }
    cout << "Game ended in " << movecount << " moves" << endl;
}

void getdepthvalue() {
    int response;
    cout << "enter valid Depth value from 2 to 5 =";
    cin >> response;
    if (response >= 1 or response <= 5)
    {
        Depth = response;
    }
    else
    {
        cout << "You haven't entered valid Depth value";
        cout << " Would you like to go with Default Depth value = 3 ?";
        char response;
        cout << "Y/N";
        cin >> response;
        if (response == 'Y' or response == 'y') //not "Y" but only one 'Y' )
        {
            Depth = 3;
        }
        else
        {
            getdepthvalue();
        }

    }
}
void getEvaluationFunction() {
    int response;
    cout << "///////////Discription of evaluation function /////////////// " << endl;
    cout << "1. Spreading my stones all over board" << endl;
    cout << "2. Choosing position where opponent have less number of moves"<< endl;
    cout << "3. Choosing position where i have more moves and oponnent have less moves"<< endl;
    cout << "4. combination of 2nd and 3rd evaluation function(most promising)"<< endl;
    cout << "5. combination of 1st, 2nd and 3rd evaluation function"<< endl;
    cout << "Enter Index of evaluation function you want to use (1 to 5)"<< endl;
    cin >> response;
    if (response >= 0 or response <= 4)
    {
        Evaluation = response;
    }
    else
    {
        cout << "You haven't entered valid Evaluation Index value"<< endl;
        cout << " Would you like to go with Default Evaluation value = 4 ?" << endl;
        char response;
        cout << "Y/N";
        cin >> response;
        if (response == 'Y' or response == 'y') //not "Y" but only one 'Y' )
        {
            Evaluation = 4;
        }
        else
        {
            getEvaluationFunction();
        }

    }
}

int main()
{
    struct Board board[4][4];

    initializeBoard(board);                              //setting initial board values
    printboard(board); 
    //show initial board position
    getdepthvalue();
    getEvaluationFunction();
    auto startinggame = chrono::high_resolution_clock::now();
    gameplay(board);                                  //start playing - first move by black
    auto endingGame = chrono::high_resolution_clock::now();
    auto gameTime = chrono::duration_cast<chrono::microseconds>(endingGame - startinggame);
    cout << "Time taken by game :" << (gameTime.count() / 1000) << endl;
    cout << "Total Node Explored: " << nodeExplored << endl;
    cout << "Number of node where search ended :" << nodePruned << endl << endl;
    /*char endit;
    cout << "Enter any key to close the screen:";
    cin >> endit;*/
     /*moveinput mi;
     mi.x = 1;
     mi.y = 3;
     mi.dir.x = -1;
     mi.dir.y = 0;*/
     ////test random position ? give input in below array
    /* board[0][0] = { 0,' ' }; board[0][1] = { 0,' ' }; board[0][2] = { 0, ' ' }; board[0][3] = { 7, 'w' };
     board[1][0] = { 1,'b' }; board[1][1] = { 0, ' ' }; board[1][2] = { 0, ' ' }; board[1][3] = { 2,'w' };
     board[2][0] = { 0,' ' }; board[2][1] = { 0, ' ' }; board[2][2] = { 9, 'b' }; board[2][3] = { 1, 'w' };
     board[3][0] = { 0,' ' }; board[3][1] = { 0,' ' }; board[3][2] = { 0, ' ' }; board[3][3] = { 0,' ' };
     printboard(board);
     cout << endl;*/
    ///* int g = bestHeuristic(board);    
    // cout << "given :" << g;
    // cout << endl;*/
    // int g1 = second(board);
    // cout << endl;
    // cout << "given :" << g1;
    // cout << endl;
    ///* makingmove(mi, board);
    // printboard(board);*/   
  
    return 0;
}



