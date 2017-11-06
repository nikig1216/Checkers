/* Nicholas Gao
 * ECE469 Artificial Intelligence
 * Professor Sable
 * Project 1: Checkers */

#include <iostream>
#include <vector>
#include <fstream>
#include <string>
#include <sstream>
#include "checkers.h"
#include "time.h"
#include <ctime>
#include <algorithm>
#include <cstdlib>

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

//// Alternate constructor for copying game states
game::game(game * g) {
    this->setSquarePositions();
    this->setNeighbors();
    this->whosTurn = g->whosTurn;
    this->timeLimit = g-> timeLimit;

    for(int i = 0; i<8; i++) {
        for(int j = 0; j <4; j++) {
            pieceType pT = g->board[i][j].contents->type;
            square::piece *p = new square::piece();
            p->type = pT;
            p->position = &(this->board[i][j]);

            this->board[i][j].contents = p;
            if(pT == 1 || pT == 3) {
                this->p1Pieces.push_back(p);
            }
            else if (pT == 2 || pT == 4) {
                this->p2Pieces.push_back(p);
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
    string blackBack = "\x1b[47;100m"; // Fix
    string redBack = "\x1b[47;100m"; // Fix
    string defBack = "\x1b[0m";
    if(x >= 4 || x < 0 || y < 0 || y >= 8) {
        cerr<< "ERROR: Invalid Coordinates Given To GetPiece()."<<endl;
        return blank;
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
    string blackBack = "\x1b[100m";
    string redBack = "\x1b[41m";
    string defBack = "\x1b[0m";
    string p1Color = "\x1b[32;42m";
    string p2Color = "\x1b[34;44m";
    string kingColor;
    char block = 64;
    string piece;
//    cout<<"  ================================="<<endl;
    for(int i = 0; i < 8; i++) {
        if(i%2 == 0) {
            for(int s = 0; s < 3; s++) {
                cout << i << " ";
                for (int col = 0; col < 4; col++) {
                    pieceType p = getPiece(col, i);

                    if(s == 1 && p > 2) kingColor = "\x1b[30;100m";
                    else kingColor = "";

                    if(p==0){
                        cout << redBack << "       " << blackBack << "  " << "   " << "  ";
//                        continue;
                    }
                    else if(p==1) {
                        piece = p1Color + block + block+ block;
                        cout << redBack << "       " << blackBack << "  " << piece << "\x1b[0;100m" << "  ";
                    }
                    else if(p==2) {
                        piece = p2Color + block + block+ block;
                        cout << redBack << "       " << blackBack << "  " << piece << "\x1b[0;100m" << "  ";
                    }
                    else if(p==3) {
                        piece = p1Color + block;
                        cout << redBack << "       " << blackBack << "  " << piece << kingColor<<"K" << piece<<"\x1b[0;100m"
                             << "  ";
                    }
                    else {
                        piece = p2Color + block;
                        cout << redBack << "       " << blackBack << "  " << piece << kingColor<< "K"<<piece <<"\x1b[0;100m"
                             << "  ";
                    }
                }
                cout << defBack << endl;
            }
        }
        else {
            for(int s = 0; s < 3; s++) {
                cout << i << " ";
                for (int col = 0; col < 4; col++) {
                    pieceType p = getPiece(col, i);

                    if(s == 1 && p > 2) kingColor = "\x1b[30;100m";
                    else kingColor = "";

                    if (p == 0) {
                        cout << blackBack << "  " << "   " << "  " << redBack << "       ";
//                        continue;
                    }
                    else if(p == 1) {
                        piece = p1Color + block + block+ block;
                        cout << blackBack << "  " << piece<<"\x1b[0;100m"<< "  " << redBack << "       ";
                    }
                    else if(p == 2) {
                        piece = p2Color + block + block+ block;
                        cout << blackBack << "  " << piece<<"\x1b[0;100m"<< "  " << redBack << "       ";
                    }
                    else if(p==3){
                        piece = p1Color + block;
                        cout << blackBack << "  " << piece<<kingColor<<"K"<<piece<<"\x1b[0;100m"<< "  " << redBack << "       ";
                    }
                    else {
                        piece = p2Color + block;
                        cout << blackBack << "  " << piece<<kingColor<<"K"<<piece<<"\x1b[0;100m"<< "  " << redBack << "       ";
                    }
                }
                cout << defBack << endl;
            }
        }
        cout<<defBack;

//        cout<<"_________________________________"<<endl;
    }
    cout<<defBack;
    cout<<"    aa     bb     cc     dd     ee     ff     gg     hh  "<<endl;
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
    if(isCapPresent) this->removeNonCaps(); // Removes nonCaptures from Legal Moves
    // Mark all Crownings

}
void game::findCaps(int direction, game::square *otherside, game *g, game::move *m) {
    int skip;
    int numCap = m->numCaps();
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
                if(numCap >= 3 && m->isAlreadyCap(new_n)) continue; // If already captured this piece on this turn, it's now blank, so skip.
                game::square *new_otherSide = (new_n->neighbors[j]);
                if(new_otherSide == nullptr) continue;
                pieceType new_OSpT = (new_otherSide->contents)->type;
                if(numCap >= 3 && new_otherSide == m->getStart()) new_OSpT = blank;
                if(new_OSpT != 0) {continue;}
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
                if(numCap >= 3 && m->isAlreadyCap(new_n)) continue; // If already captured this piece on this turn, it's now blank, so skip.
                game::square *new_otherSide = (new_n->neighbors[j]);
                if(new_otherSide == nullptr) continue;
                pieceType new_OSpT = (new_otherSide->contents)->type;
                if(numCap >= 3 && new_otherSide == m->getStart()) new_OSpT = blank;
                if(new_OSpT != 0) {continue;}
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
//            cout<<"GAME OVER! Player 2 Wins!"<<endl;
            return true;
        }
    }
    else { //P2 turn
        if(this->p2Pieces.size() == 0 || this->moves.size() == 0){
//            cout<<"GAME OVER! Player 1 Wins!"<<endl;
            return true;
        }
    }
    return false;
}
bool game::IsActualGameOver() {
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
    this->whosTurn = -1*(this->whosTurn) + 3;
    return;
}
void game::actualEndTurn() {
    // Clear Possible Moves since one was made. Ready for next Move.
    for(std::vector<move *>::iterator itM= this->moves.begin(); itM != this->moves.end(); ++itM) {
        delete *itM;
    }
    this->moves.clear();
    // Update Turn for next player.
    this->whosTurn = -1*(this->whosTurn) + 3;
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
        if(ref == -1) { //Undo
            inputGood = true;
        }
        else if (!cin || ref >= this->moves.size() || ref < 0) {
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

    if(oldPos != newPos){ // Don't need to swap if piece comes back.
        auto tempBlank = m->getEnd()->contents;
        if(tempBlank->type != 0) cerr<<"ERROR!"<<endl;
        // Move piece over to blank
        m->getEnd()->contents = m->getStart()->contents;
        m->getEnd()->contents->position = newPos;
        // Move blank piece into old spot
        m->getStart()->contents = tempBlank;
        m->getStart()->contents->position = oldPos;
    }

    // Check for Crowning
    if(isP1 && who == 1 && newPos->row == 0) {
        m->getEnd()->contents->type = p1K;
        m->markCrown();
    }
    else if(!isP1 && who == 2 && newPos->row == 7) {
        m->getEnd()->contents->type = p2K;
        m->markCrown();
    }
    // Delete Captured
    if(m->isCap()) {
        int i = 0;
        for(auto itC = m->getSqToCap().begin(); itC != m->getSqToCap().end(); ++itC) {
            if(itC == m->getSqToCap().end() || i >= m->getSqToCap().size()) { // For loop malfunctioning somehow...need extra stop check.
                cout<<"BreakingLoop========================"<<endl;
                break;
            }
            square::piece *p = (*itC)->contents;
            std::vector<game::square::piece *>::iterator itP;
            if(isP1) { // P1 Turn so delete piece from P2
                itP = std::find(this->p2Pieces.begin(),this->p2Pieces.end(),p);
                if(itP != this->p2Pieces.end()) {
                    this->p2Pieces.erase(itP);
                    m->addCapType(p->type,i);
                }
                else{
                    cerr<<"ERROR: Finding the piece from P2 to delete."<<endl;
                }
            }
            else { // P2 Turn, delete P1's piece
                itP = std::find(this->p1Pieces.begin(),this->p1Pieces.end(),p);
                if(itP != this->p1Pieces.end()) {
                    this->p1Pieces.erase(itP);
                    m->addCapType(p->type,i);
                }
                else {
                    cerr<<"ERROR: Finding the piece from P1 to delete."<<endl;
                }
            }
            (*itC)->contents->type = blank;
            i++;
        }
    }
    this->movesTaken.push_back(m);
}
void game::undo() {
    move *m = this->movesTaken.back();
    auto oldPos = m->getStart();
    auto newPos = m->getEnd();

    // DeCrown if necessary.
    if(m->isCrown()) {
        if(newPos->contents->type == 3) newPos->contents->type = p1;
        else newPos->contents->type = p2;
//        m->getEnd()->contents->type -= 2;
    }

    // Move piece back to original spot.
    if(oldPos != newPos){ // Don't need to swap if piece stayed.
        auto tempBlank = oldPos->contents;
        if(tempBlank->type != 0) cerr<<"ERROR! Original spot not blank."<<endl;

        // Move piece over to blank
        oldPos->contents = newPos->contents;
        oldPos->contents->position = oldPos;

        // Move blank piece into old spot
        newPos->contents = tempBlank;
        newPos->contents->position = newPos;
    }
    // Restore Captured Pieces
    if(m->isCap()) {
        int i = 0;
        for(auto itC = m->getSqToCap().begin(); itC != m->getSqToCap().end(); ++itC) {
            if(itC == m->getSqToCap().end()) { // For loop malfunctioning somehow...need extra stop check.
                cout<<"BreakingLoopUndoCap========================"<<endl;
                break;
            }
            // Set Blanks on Board back to Pieces
            square::piece *p = (*itC)->contents;
            if(p->type != 0) {
                cerr<<"ERROR: Restoring pieces to spots that aren't blank."<<endl;
            }
            pieceType pT = m->getCapType(i);
            p->type = pT;

            if(pT == 1 || pT == 3) {
                this->p1Pieces.push_back(p);
            }
            else {
                this->p2Pieces.push_back(p);
            }
            i++;
        }
    }
    // Delete move from existence and pop it from vector.
    delete m;
    this->movesTaken.pop_back();
}

void game::play(int mode) {
    int whoTurn = this->getWhichPlayersTurn();
    if(whoTurn==1) cout<<"Player 1's Turn (Green)"<<endl;
    else cout<<"Player 2's Turn (Blue AI)"<<endl;
    this->display();
    this->findMoves();

    if(this->IsActualGameOver()) return;
    int choice;
    if(mode == 1){ // Human vs. AI
        if(whoTurn == 1) this->DisplayMovesAndChoose(choice);
        else {
            this->searchForMove(choice);
            cout<<"Search chose choice: "<<choice<<endl;
        }
    }
    else{ //mode = 2 AI vs. AI
        this->searchForMove(choice);
        cout<<"Search chose choice: "<<choice<<endl;
    }

    if(choice == -1) { // Undo
        cout<<"Undo Chosen."<<endl;
        if(this->movesTaken.size() != 0) {
            this->undo();
            this->actualEndTurn();
        }
        else {
            cout<<"Cannot Undo."<<endl;
            while(choice < 0) {
                this->DisplayMovesAndChoose(choice);
            }
            this->Move(this->moves[choice]);

//            this->findMoves();
//            if(this->IsGameOver()) { // If you walk into a loss. Game should end instead of waiting for it to become your turn again to check.
//                cout << "Walked into a loss. This shouldn't happen."<<endl;
//            }
            this->actualEndTurn();
        }
    }
    else {
        this->Move(this->moves[choice]);

        this->findMoves();
        if(this->IsActualGameOver()) { // If you walk into a loss. Game should end instead of waiting for it to become your turn again to check.
            cout << "Walked into a loss. This shouldn't happen."<<endl;
        }
        this->actualEndTurn();
    }

    this->play(mode);
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
        theGame.play(mode-'0');
//    }

    return 0;
}

void game::searchForMove(int &choice) {
    int maxDepth = 10;
    time_t t1, t2;
    time(&t1);
    if(this->getMoves().size() == 1) {
        choice = 0;
        time(&t2);
        double sec = difftime(t2,t1);
        cout<<"Only one legal move. Time taken to find: "<< sec << " seconds." << endl;
        return;
    }
    //// Iterative Deepening
    for(int d = 1; d <= maxDepth; d++ ) {
        cout<<"Searching depth "<<d<<endl;
        int alpha = std::numeric_limits<int>::min();
        int beta = std::numeric_limits<int>::max();
        this->alphabeta(this,d,alpha,beta,choice);
        time(&t2);
        double sec = difftime(t2,t1);
        if(sec >= ((double)this->timeLimit)/2){ /// Halfway out of time //
            cout<<"Reached Depth: "<< d <<" in Time: "<< sec << " seconds." << endl;
            break;
        }
    }
}

int game::heuristic(game *g, int depth, int fullDepth) {
    int h = 0;
    int p1 = 0, p2 = 0, bonus = 0;

    // Endgame, give diagonal bonus
    int num1 = g->p1Pieces.size();
    int num2 = g->p2Pieces.size();

    for(int row = 0; row < 8;row++) {
        for(int col = 0; col < 4; col++) {
            /// Endgame diagonal bonus
            if(( num1 <= 5 || num2 <= 5) &&
                    ((row==0&&col==0)||((row>0&&row<3)&&(col>=0&&col<2))||
                            ((row>2&&row<5)&&(col>=1&&col<3))||
                            ((row>4&&row<7)&&(col>=2&&col<4))||
                            (row==7&&col==3))
                    ){
                bonus = 500;
            }

            pieceType pT = g->board[row][col].contents->type;
            if(pT == 1) {
                if(row < 4) p1 += 100; // Value progressed pawns
                p1 += 1000 + bonus;
            }
            if(pT == 2) {
                if(row > 3) p2 +=100;
                p2 += 1000 + bonus;
            }
            if(pT == 3) p1 += 1500 + bonus;
            if(pT == 4) p2 += 1500 + bonus;
        }
    }
    h += (p1-p2);

    // Reward Wins
    if(g->IsGameOver()) {
        if(g->getWhichPlayersTurn() == 1){ //Player 2 Won
            h -= 10000*(depth);
        }
        else {
            h += 10000*(depth);
        }
    }
//    if(g->getWhichPlayersTurn() == 1) {
//        return (p2-p1);
//    }
//    else {
        return h;
//    }
}

void game::alphabeta(game *g, int depth, int alpha, int beta, int &choice) {
    game node(g);
    bool isMaxP1;
    if(node.getWhichPlayersTurn() == 1) {
        isMaxP1 = true;
    }
    else {
        isMaxP1 = false;
    }
    node.findMoves();

    if (depth == 0 || node.IsGameOver()) {
        // No search or Game is Over. Just stop.
        return;
    }
    int v;
    std::vector<int> multChoices;
    if (isMaxP1) {
        v = std::numeric_limits<int>::min();
        for(int i = 0; i < node.getMoves().size(); i++) {/// each child of node
            game child(&node);
            child.findMoves();
            child.Move(child.getMoves()[i]);
            child.endTurn();
            int newV = alphabeta(depth, &child, (depth-1), alpha, beta);
            if(newV > v) {
                choice = i;
                multChoices.clear();
                multChoices.push_back(i);
            }
            else if(newV == v) multChoices.push_back(i);
            v = std::max(v, newV);
            alpha = std::max(alpha, v);
            if (beta <= alpha) break; // (* beta cut-off *)
        }
    }
    else {
        v = std::numeric_limits<int>::max();
        for(int i = 0; i < node.getMoves().size(); i++) { /// each child of node
            game child(&node);
            child.findMoves();
            child.Move(child.getMoves()[i]);
            child.endTurn();
            int newV = alphabeta(depth, &child, (depth - 1), alpha, beta);
            if (newV < v) {
                choice = i;
                multChoices.clear();
                multChoices.push_back(i);
            }
            else if(newV == v) multChoices.push_back(i);
            v = std::min(v, newV);
            beta = std::min(beta, v);
            if (beta <= alpha) break; //(* alpha cut-off *)
        }
    }
    int size = multChoices.size();
    if(size > 1) {
        srand ( time(NULL) );
        choice = multChoices[rand()%size];
    }
}
int game::alphabeta(int fullDepth, game *thisChild, int depth, int alpha, int beta) {
    bool isMaxP1;
    if(thisChild->getWhichPlayersTurn() == 1) {
        isMaxP1 = true;
    }
    else {
        isMaxP1 = false;
    }
//    game node(g);
    if (depth == 0 || thisChild->IsGameOver()) {
        //// return the heuristic value of node
        return heuristic(thisChild, depth, fullDepth);
    }
    int v;
    if (isMaxP1) {
        v = std::numeric_limits<int>::min();
        for(int i = 0; i < thisChild->getMoves().size(); i++) {/// each child of node
            game child(thisChild);
            child.findMoves();
            child.Move(child.getMoves()[i]);
            child.endTurn();
            int newV = alphabeta(fullDepth, &child, (depth-1), alpha, beta);
//            if(newV > v) choice = i;
            v = std::max(v, newV);
            alpha = std::max(alpha, v);
            if (beta <= alpha) break; // (* beta cut-off *)
        }
        return v;
    }
    else {
        v = std::numeric_limits<int>::max();
        for(int i = 0; i < thisChild->getMoves().size(); i++) { /// each child of node
            game child(thisChild);
            child.findMoves();
            child.Move(child.getMoves()[i]);
            child.endTurn();
            int newV = alphabeta(fullDepth, &child, (depth - 1), alpha, beta);
//            if (newV < v) choice = i;
            v = std::min(v, newV);
            beta = std::min(beta, v);
            if (beta <= alpha) break; //(* alpha cut-off *)
        }
        return v;
    }

}