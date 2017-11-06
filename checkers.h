/* Nicholas Gao
 * ECE469 Artificial Intelligence
 * Professor Sable
 * Project 1: Checkers */

#ifndef CHECKERS_CHECKERS_H
#define CHECKERS_CHECKERS_H

#include <vector>
#include <fstream>
#include <string>

using namespace std;

class game {

public:
    game(std::string file, char whoStarts = '1', int timeLim = 5);
    game(game *g);
    void setSquarePositions();
    void setNeighbors();
    void loadGame(ifstream &loadStream);
    void display();
    int getWhichPlayersTurn();
    void endTurn();
    void actualEndTurn();
    void DisplayMovesAndChoose(int &ref);

    void play(int mode);
    bool IsGameOver();
    bool IsActualGameOver();

    void findMoves();

    void removeNonCaps() {
        for(std::vector<move *>::iterator it = this->moves.begin(); it != this->moves.end(); ++it) {
            if(!(*it)->isCap()) { // Not Capture, so not valid
                delete *it; // free up memory.
                this->moves.erase(it);
                it--;
            }
        }
    }
//    void markCrowns() {
//        for(std::vector<move *>::iterator it = this->moves.begin(); it != this->moves.end(); ++it) {
//            pieceType pT = (*it)->getStart()->contents->type;
//            if(pT == 1){}
//        }
//    }

    enum pieceType {
        blank, p1, p2, p1K, p2K
        // Never should have to use blank
    };
    pieceType getPiece(int x, int y);
/*    class piece {
//    public:
//        pieceType type;
//        square *position;
//
//        void getPos(square *position, int &row, int &column) {
//            int pos = position - &board[0][0];
//            row = pos/4;
//            column = pos%4;
//            return;
//        }
    };*/
    class square {
    public:
        class piece {
        public:
            pieceType type;
            square *position;
        };

        string ToString() {
            char col;
            if(row%2 == 0) { // Even Row
                col = 'B' + this->column*2;
            }
            else {
                col = 'A' + this->column*2;
            }
            string pos = col + to_string(this->row);
            return pos;
        }
        int row;
        int column;
        piece *contents;
        square *neighbors[4]; // Up-Left, Up-Right, Down-Left, Down-Right
    };

    class move {
    public:
        move(game::square *s, game::square *e, bool isCap, game::square *Cap = nullptr) {
            this->end = e;
            this->start = s;
            this->isCapture = isCap;
            this->isCrowning = false;
            if(isCap == true) {
                this->squaresToCap.push_back(Cap);
            }
        }
        move(move *m) {
            this->start = m->start;
            this->end = m->end;
            this->isCapture = m->isCapture;
            this->isCrowning = m->isCrowning;
            this->squaresToCap = m->squaresToCap;
        }

        string ToString() {
            string moveText = "Move " + start->ToString() + " to " + end->ToString() + ". Capture = " + std::string(isCapture ? "Yes: " : "No.");
            if(isCapture){
                for(std::vector<game::square *>::iterator it = this->squaresToCap.begin(); it != this->squaresToCap.end(); ++it){
                    if(it == this->squaresToCap.begin()) moveText = moveText + (*it)->ToString();
                    else moveText = moveText + ", " + (*it)->ToString();
                }
                moveText = moveText + ".";
            }
            return moveText;
        }
        game::square *getStart() {
            return this->start;
        }
        game::square *getEnd() {
            return this->end;
        }
        void updateEnd(game::square *nEnd) {
            this->end = nEnd;
        }
        void addCap(game::square *nextCap) {
            this->squaresToCap.push_back(nextCap);
        }
        void popCap() {
            this->squaresToCap.pop_back();
        }
        bool isCap() {
            return this->isCapture;
        }
        std::vector<game::square *> getSqToCap() {
            return this->squaresToCap;
        }
        int numCaps() {
            return this->squaresToCap.size();
        }
        bool isAlreadyCap(game::square *s) {
            std::vector<game::square *>::iterator itS = std::find(this->squaresToCap.begin(),this->squaresToCap.end(),s);
            if(itS != this->squaresToCap.end()) {
                return true;
            }
            return false;
        }
        bool isCrown() {
            return this->isCrowning;
        }
        void markCrown() {
            this->isCrowning = true;
        }
        void addCapType(pieceType p, int i) {
            this->capTypes[i]=p;
        }
        pieceType getCapType(int i) {
            return this->capTypes[i];
        }

    private:
        game::square *start;
        game::square *end;
        bool isCapture;
        bool isCrowning;
        std::vector<game::square *>squaresToCap;
        pieceType capTypes[12];
    };

    //// Public Game Methods
    void Move(game::move *m);
    void findCaps(int direction, game::square *otherside, game *g, game::move *m);
    vector<move *> getMoves() {
        return this->moves;
    }
    void undo();
    void searchForMove(int &choice);
    int heuristic(game *g, int depth, int fullDepth);
    void alphabeta(game *node, int depth, int alpha, int beta, int &choice);
    int alphabeta(int fullDepth, game *node, int depth, int alpha, int beta);

private:
    game::square board[8][4];
    std::vector<square::piece *> p1Pieces;
    std::vector<square::piece *> p2Pieces;
    std::vector<move *> moves;
    std::vector<move *> movesTaken;

    int whosTurn;
    int timeLimit;
};

#endif //CHECKERS_CHECKERS_H
