/* Nicholas Gao
 * ECE469 Artificial Intelligence
 * Professor Sable
 * Project 1: Checkers */

// #include <stdlib.h>
#include <iostream>
#include <vector>
#include <fstream>
// #include <string>
#include <sstream>
#include "checkers.h"

using namespace std;

game::game(string file, char whoStarts, int timeLim) {
    this->setSquarePositions();
    this->setNeighbors();
    if(!file.empty()) {
        ifstream loadStream;
        loadStream.open(file.c_str());
        if(!loadStream) {
            cerr << "ERROR: could not open file: " << file << endl;
            exit(EXIT_FAILURE);
        }
        this->loadGame(loadStream);
        loadStream.close();
    }
    else { // Initialize new game board.
        this->whosTurn = whoStarts - '0';
        this->timeLimit = timeLim;
        for(int setup = 0; setup < 8; setup++) {
            if(setup >= 0 && setup < 3) {
                for(int setP2 = 0; setP2 < 4; setP2++) {
                    auto *p = new square::piece();
                    p->type = p2;
                    p->position = &(this->board[setup][setP2]);
                    this->board[setup][setP2].contents = p;
                    p2Pieces.push_back(p);
                }
            }
            else if(setup >= 5 && setup < 8) {
                for(int setP1 = 0; setP1 < 4; setP1++) {
                    auto *p = new square::piece();
                    p->type = p1;
                    p->position = &(this->board[setup][setP1]);
                    this->board[setup][setP1].contents = p;
                    p1Pieces.push_back(p);
                }
            }
            else {
                for(int setEmp = 0; setEmp < 4; setEmp++) {
                    auto *p = new square::piece();
                    p->type = blank;
                    p->position = &(this->board[setup][setEmp]);
                    this->board[setup][setEmp].contents = p;
                }
            }
        }
    }
}
void game::setSquarePositions() {
    for(int row = 0; row < 8; row++) {
        for(int col = 0; col < 4; col++) {
            this->board[row][col].column = col;
            this->board[row][col].row = row;
        }
    }
}
void game::setNeighbors() {
    for(int row = 0; row < 8; row++) {
        if(row == 0) {
            for(int col = 0; col < 4; col++) {
                this->board[row][col].neighbors[0]= nullptr;
                this->board[row][col].neighbors[1]= nullptr;
                this->board[row][col].neighbors[2]= &(this->board[row+1][col]);
                if(col < 3) {
                    this->board[row][col].neighbors[3]= &(this->board[row+1][col+1]);
                }else {
                    this->board[row][col].neighbors[3]= nullptr;
                }
            }
        }
        else if(row == 2 || row == 4 || row == 6) {
            for(int col = 0; col < 4; col++) {
                this->board[row][col].neighbors[0]= &(this->board[row-1][col]);
                this->board[row][col].neighbors[2]= &(this->board[row+1][col]);
                if(col < 3) {
                    this->board[row][col].neighbors[1]= &(this->board[row-1][col+1]);
                    this->board[row][col].neighbors[3]= &(this->board[row+1][col+1]);
                }else { // col = 3
                    this->board[row][col].neighbors[1]= nullptr;
                    this->board[row][col].neighbors[3]= nullptr;
                }
            }
        }
        else if(row == 7) {
            for(int col = 0; col < 4; col++) {
                this->board[row][col].neighbors[2]= nullptr;
                this->board[row][col].neighbors[3]= nullptr;
                if(col == 0) {
                    this->board[row][col].neighbors[0]= nullptr;
                    this->board[row][col].neighbors[1]= &(this->board[row-1][col]);
                }
                else {
                    this->board[row][col].neighbors[0]= &(this->board[row-1][col-1]);
                    this->board[row][col].neighbors[1]= &(this->board[row-1][col]);
                }
            }
        }
        else { // row = 1, 3, or 5
            for(int col = 0; col < 4; col++) {
                this->board[row][col].neighbors[1]= &(this->board[row-1][col]);
                this->board[row][col].neighbors[3]= &(this->board[row+1][col]);
                if(col == 0) {
                    this->board[row][col].neighbors[0]= nullptr;
                    this->board[row][col].neighbors[2]= nullptr;
                }
                else {
                    this->board[row][col].neighbors[0]= &(this->board[row-1][col-1]);
                    this->board[row][col].neighbors[2]= &(this->board[row+1][col-1]);
                }
            }
        }
    }
}
void game::loadGame(ifstream &loadStream) {
    int rowNum = 0;
    string row, whosTurn, timeLim;
    while(loadStream.good()) {
        if(rowNum < 8) {
            int colNum = 0;
            getline(loadStream,row);
            stringstream ss(row);
            string piece;
            while(ss >> piece) {
//                cout << "the piece: " << piece << "| and col: " << colNum << endl;
                auto pT = static_cast<pieceType>(stoi(piece));
                if(pT >= 0 && pT <= 4) {
                    auto *p = new square::piece();
                    p->type = pT;
                    p->position = &(this->board[rowNum][colNum]);
                    this->board[rowNum][colNum].contents = p;
                    if(pT == 1 || pT == 3) {
                        p1Pieces.push_back(p);
                    }
                    else if(pT == 2 || pT == 4) {
                        p2Pieces.push_back(p);
                    }
                }
                colNum++;
            }
        }
        else if (rowNum == 8){
            getline(loadStream,whosTurn);
            this->whosTurn = stoi(whosTurn);
        }
        else { //row = 9
            getline(loadStream,timeLim);
            this->timeLimit = stoi(timeLim);
        }
        rowNum++;
        if(rowNum >= 10) break;
    }
}

game::pieceType game::getPiece(int x, int y) {
    if(x >= 4 || x < 0 || y < 0 || y >= 8) {
        cerr<< "Invalid Coordinates Given."<<endl;
    }
    else {
        square::piece *p = board[y][x].contents;
        if (p == nullptr) {
            return blank;
        }
        else {
            return p->type;
        }
    }
}

void game::display() {
    cout<<"  ================================="<<endl;
    for(int i = 0; i < 8; i++) {
        if(i%2 == 0) {
            cout<< i <<" |   | "<< getPiece(0,i) <<" |   | "<< getPiece(1,i) << " |   | "<< getPiece(2,i) << " |   | "<< getPiece(3,i)<<" |"<<endl;
        }
        else {
            cout<< i <<" | "<< getPiece(0,i) <<" |   | "<< getPiece(1,i) << " |   | "<< getPiece(2,i) << " |   | "<< getPiece(3,i) << " |   |" << endl;
        }
        cout<<"  ================================="<<endl;

//        cout<<"_________________________________"<<endl;
    }
    cout<<"    a   b   c   d   e   f   g   h  "<<endl;
}

void getInteger(string message, int &ref) {
    bool inputGood = false;
    while (!inputGood) {
        inputGood = true;

        cout << message;
        cin >> ref;

        if (!cin) {
            // Non-integer in input buffer, get out of "fail" state
            cin.clear();
            inputGood = false;
        }
        while (cin.get() != '\n'); // clear buffer
    }
}

void game::findMoves() {
    bool isCapPresent = false;
    this->moves.clear();
    if((this->whosTurn) == 1) {
        for(std::vector<square::piece *>::iterator it = (this->p1Pieces).begin(); it != (this->p1Pieces).end(); ++it) {
            pieceType pT = (*it)->type;
            if(pT == 1 || pT == 3) { // Should always go in here.
                // Check neighbors
                for(int i = 0; i <= 3; i++){
                    if(pT == 1 && i >= 2) break; // Only search forwards for reg pieces.
                    square *n = ((*it)->position)->neighbors[i];
                    if(n != nullptr) { // Not off board
                        pieceType nPiece = (n->contents)->type;
                        if(nPiece == 1 || nPiece == 3) {
                            continue;
                        }
                        else if(nPiece == 0) {
                            auto m = new move((*it)->position, n, false);
                            this->moves.push_back(m);
                        }
                        else { // nP = 2 or 4 - Opponent P2
                            square *otherSide = (n->neighbors[i]);
                            if(otherSide == nullptr) continue;
                            if((otherSide->contents)->type != 0) {continue;}
                            else {
                                isCapPresent = true;
                                auto m = new move((*it)->position, otherSide, true, n);
                                findCaps(i, otherSide, this, m);
                            }
                        }
                    }
                }
            }
        }
    }
    else{ //P2 turn
        for(std::vector<square::piece *>::iterator it = (this->p2Pieces).begin(); it != (this->p2Pieces).end(); ++it) {
            pieceType pT = (*it)->type;
            if(pT == 2 || pT == 4) { // Should always go in here.
                for(int i = 3; i >= 0; i--){
                    if(pT == 2 && i < 2) break; // Only search forwards for reg pieces.
                    square *n = ((*it)->position)->neighbors[i];
                    if(n != nullptr) {
                        pieceType nPiece = (n->contents)->type;
                        if(nPiece == 2 || nPiece == 4) {
                            continue;
                        }
                        else if(nPiece == 0) {
                            auto m = new move((*it)->position, n, false);
                            this->moves.push_back(m);
                        }
                        else { //nP = 1 or 3 - Opponent P1
                            square *otherSide = (n->neighbors[i]);
                            if(otherSide == nullptr) continue;
                            if((otherSide->contents)->type != 0) {continue;}
                            else {
                                isCapPresent = true;
                                auto m = new move((*it)->position, otherSide, true, n);
                                findCaps(i, otherSide, this, m);
                            }
                        }
                    }
                }
            }
        }
    }
    if(isCapPresent) this->removeNonCaps();
}
void game::findCaps(int direction, game::square *otherside, game *g, game::move *m) {
    int skip;
    bool isEnd = true;
    skip = -1*direction + 3; // Dont check where you came from.
    pieceType pT = (m->getStart())->contents->type;

    for(int j = 0; j < 4; j++) {
        if(pT == 1 && j > 1) continue;
        if(pT == 2 && j < 2) continue; // Pawns move 1D.
        if(j == skip) continue;
        game::square *new_n = otherside->neighbors[j];
        if(new_n == nullptr) continue; // No neighbor/At Edge
        game::pieceType nPT = new_n->contents->type;
        if(g->getWhichPlayersTurn() == 1){
            if(nPT == 2 || nPT == 4) {
                game::square *new_otherSide = (new_n->neighbors[j]);
                if(new_otherSide == nullptr) continue;
                if((new_otherSide->contents)->type != 0) {continue;}
                else {
                    isEnd = false;
//                    m->updateEnd(new_otherSide);
                    m->addCap(new_n);
                    findCaps(j, new_otherSide, g, m);
                }
            }
            //Else continue
        }
        else { // Player 2's Turn
            if(nPT == 1 || nPT == 3) {
                game::square *new_otherSide = (new_n->neighbors[j]);
                if(new_otherSide == nullptr) continue;
                if((new_otherSide->contents)->type != 0) {continue;}
                else {
                    isEnd = false;
//                    m->updateEnd(new_otherSide);
                    m->addCap(new_n);
                    findCaps(j, new_otherSide, g, m);
                }
            }
            //Else continue
        }
    }
    if(isEnd){
        auto m_copy = new move(m);
        m_copy->updateEnd(otherside);
        g->moves.push_back(m_copy);
        m->popCap(); // Remove the captures to this point. Move back up tree for alt routes.
    }
    else {
        m->popCap();
    }
}

bool game::IsGameOver() {
    if(this->whosTurn == 1) {
        if(this->p1Pieces.size() == 0 || this->moves.size() == 0){
            cout<<"GAME OVER! Player 2 Wins!"<<endl;
            return true;
        }
    }
    else { //P2 turn
        if(this->p2Pieces.size() == 0 || this->moves.size() == 0){
            cout<<"GAME OVER! Player 1 Wins!"<<endl;
            return true;
        }
    }
    return false;
}

int game::getWhichPlayersTurn() {
    return this->whosTurn;
}
void game::endTurn() {
    // Clear Possible Moves since one was made. Ready for next Move.
    for(std::vector<move *>::iterator itM= this->moves.begin(); itM != this->moves.end(); ++itM) {
        delete *itM;
    }
    this->moves.clear();
    // Update Turn for next player.
    this->whosTurn = -1*(this->whosTurn) + 1;
    cout<<"=================================================================="<<endl;
    return;
}

void game::DisplayMovesAndChoose(int &ref) {
    bool inputGood = false;
    while (!inputGood) {
        inputGood = true;
        cout<< "Choose a valid move using it's number."<<endl;
        int moveNum = 0;
        for(std::vector<move *>::iterator itM= this->moves.begin(); itM != this->moves.end(); ++itM) {
            cout << "[" << moveNum << "] : " << (*itM)->ToString() << endl;
            moveNum++;
        }
        cout<<"Enter your choice: ";
        cin >> ref;

        if (!cin || ref >= this->moves.size() || ref < 0) {
            // Non-integer in input buffer, get out of "fail" state
            // Or invalid integer.
            cin.clear();
            inputGood = false;
        }
        while (cin.get() != '\n'); // clear buffer
    }
}
void game::Move(game::move *m) {
    bool isP1;
    pieceType who = m->getStart()->contents->type;
    if(who == 1 || who == 3) {
        isP1 = true;
    }
    else{
        isP1 = false;
    }
    auto oldPos = m->getStart();
    auto newPos = m->getEnd();
    auto tempBlank = m->getEnd()->contents;
    if(tempBlank->type != 0) cerr<<"ERROR!"<<endl;
    // Move piece over to blank
    m->getEnd()->contents = m->getStart()->contents;
    m->getEnd()->contents->position = newPos;
    // Move blank piece into old spot
    m->getStart()->contents = tempBlank;
    m->getStart()->contents->position = oldPos;

    // Check for Crowning
    if(isP1 && who == 1 && newPos->row == 0) {
        m->getEnd()->contents->type = p1K;
    }
    else if(!isP1 && who == 2 && newPos->row == 7) {
        m->getEnd()->contents->type = p2K;
    }
    // Delete Captured
    if(m->isCap()) {
        for(std::vector<square *>::iterator it = m->getSqToCap().begin(); it != m->getSqToCap().end(); ++it) {
            square::piece *p = (*it)->contents;
            std::vector<game::square::piece *>::iterator itP;
            if(isP1) { // P1 Turn so delete piece from P2
                itP = std::find(this->p2Pieces.begin(),this->p2Pieces.end(),p);
                if(itP != this->p2Pieces.end()) {
                    this->p2Pieces.erase(itP);
                }
                else{
                    cerr<<"ERROR: Finding the piece from P2 to delete."<<endl;
                }
            }
            else { // P2 Turn, delete P1's piece
                itP = std::find(this->p1Pieces.begin(),this->p1Pieces.end(),p);
                if(itP != this->p1Pieces.end()) {
                    this->p1Pieces.erase(itP);
                }
                else {
                    cerr<<"ERROR: Finding the piece from P1 to delete."<<endl;
                }
            }
            (*it)->contents->type = blank;
        }
    }

}

void game::play() {
    this->display();
    this->findMoves();
    if(this->IsGameOver()) return;
    int choice;
    this->DisplayMovesAndChoose(choice);
    this->Move(this->moves[choice]);
    this->endTurn();

    this->play();
    return;
}

int main() {
    cout<<"Welcome to Checkers! Please choose an option numbers from below to start: "<<endl;

    char newOrLoad, mode, whoStarts;
    int timeLimit;
    string file;
    while(newOrLoad != '1' && newOrLoad != '2') {
        cout<<"1) New Game OR 2) Load Game from TextFile"<<endl;
        cin>>newOrLoad;
    }
    if(newOrLoad == '2') {
        cout<<"Please enter in the text file to load from: ";
        cin>>file;
        cin.clear();
        cin.ignore(10000, '\n');
    }
    else {
        while(whoStarts != '1' && whoStarts != '2') {
            cout << "Please select which player moves first: 1) Player 1 OR 2) Player 2" << endl;
            cin >> whoStarts;
        }
        getInteger("Please enter an integer value for the AI's time limit in seconds: ", timeLimit);
    }
    while(mode != '1' && mode != '2') {
        cout << "Please choose a mode number: 1) Play AI OR 2) Observe AI vs. AI" << endl;
        cin >> mode;
    }

    game theGame(file, whoStarts, timeLimit);
//    while(!(theGame.IsGameOver())){
        theGame.play();
//    }

    return 0;
}