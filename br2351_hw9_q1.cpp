#include <iostream>
#include <cstdlib>
#include <ctime>
using namespace std;

int const BOARD_SIZE = 20;

class Organism{
    int randomNumMax19();
    int row, column;
    int age;
    bool remove_ant, remove_dbug;
public:
    Organism (int rowNum, int columnNum): row(rowNum), column(columnNum), age(0), remove_ant(false), remove_dbug(false){}
    Organism() :row(randomNumMax19()), column(randomNumMax19()), age(0), remove_ant(false), remove_dbug(false) {}
    virtual ~Organism()= default;
    virtual void move(char board[][BOARD_SIZE], int row, int column) =0;
    virtual void breed(char board[][BOARD_SIZE], Organism* list[], int& listSize)=0;
    virtual bool isSurrounded() const =0;
    virtual void reset_sides()=0;
    virtual void update_all_sides(char board[][BOARD_SIZE])=0;
    virtual char get_type() = 0;
    virtual bool get_up() =0;
    virtual bool get_down()=0;
    virtual bool get_left() =0;
    virtual bool get_right() =0;
    virtual bool isStarved()=0;
    int get_age() const {return age;}
    int get_row() const {return row;}
    int get_column() const {return column;}
    void set_row(int new_row) {row = new_row;}
    void set_column(int new_column) {column = new_column;}
    void set_remove_ant(bool val){remove_ant = val;}
    void set_remove_dbug(bool val){remove_dbug = val;}
    bool get_remove_ant() const{return remove_ant;}
    bool get_remove_dbug() const{return remove_dbug;}
    void update_age() {age++;}
protected:
    int randomNumMax3() const {return rand() % 4;}
};
class Ant: public Organism{
    bool up, down, left, right;
public:
    Ant(int rowNum, int columnNum) : Organism(rowNum, columnNum), up(false), down(false), left(false), right(false) {}
    Ant():Organism(), up(false), down(false), left(false), right(false){}
    ~Ant() override =default;
    bool get_up() override {return up;}
    bool get_down() override {return down;}
    bool get_left() override {return left;}
    bool get_right() override {return right;}
    bool isStarved () override {return false;}; // not used by ant, but required for virtual definition in Organism
    void set_up(){up = true;}
    void set_down() {down = true;}
    void set_left(){left = true;}
    void set_right(){right = true;}
    void reset_sides() override {up = false; down = false; left = false; right = false;}
    void move(char board[][BOARD_SIZE], int row, int column) override;
    char get_type() override {return 'a';}
    void breed(char board[][BOARD_SIZE], Organism* list[], int& listSize) override;
    bool isClear(int move) const;
    void update_all_sides(char board[][BOARD_SIZE]) override;
    bool isSurrounded() const override {if (up && down && left && right){return true;} else {return false;}}
};
class Doodlebug: public Organism{
    int until_starves;
    char up_type, down_type, left_type, right_type;  //settings are: 'b' (border), 'd' (Doodlebug), and 'a' (ant)... free is 'e' (empty), undefined is 'x'
    bool isAnt(int move);
    bool ate;
public:
    Doodlebug(int rowNum, int columnNum) : Organism(rowNum, columnNum), until_starves(3), up_type('x'),down_type('x'), left_type('x'), right_type('x'), ate(false) {}
    Doodlebug():Organism(), until_starves(3), up_type('x'), down_type('x'), left_type('x'), right_type('x'), ate(false){}
    ~Doodlebug() override =default;
    void eat();
    bool isStarved() override {if(until_starves ==0){return true;} else {return false;}};
    bool get_up() override {return up_type;}
    bool get_down() override {return down_type;}
    bool get_left() override {return left_type;}
    bool get_right() override {return right_type;}
    int get_until_starves() {return until_starves;}
    void set_up_type(char type){up_type = type;}
    void set_down_type(char type) {down_type = type;}
    void set_left_type(char type){left_type = type;}
    void set_right_type(char type){right_type = type;}
    bool isClear(int move);
    void update_all_sides(char board[][BOARD_SIZE]) override;
    void reset_sides() override {up_type ='x'; down_type ='x'; left_type ='x'; right_type ='x';}
    void move(char board[][BOARD_SIZE], int row, int column) override;
    char get_type() override {return 'd';}
    void breed(char board[][BOARD_SIZE], Organism* list[], int& listSize) override ;
    bool isSurrounded() const override;
};

void initializeBoard(char board[][BOARD_SIZE]);
void spawnCritters(Organism* list[], int& listSize, int numAnts, int numDbugs);
void drawBoard(char board[][BOARD_SIZE]);
void removeDoubles(char board[][BOARD_SIZE], Organism* list[], int listSize);
void updateCritterAge(Organism* list[], int listSize);
void updateBoard(char board[][BOARD_SIZE], Organism* list[], int listSize);
void moveCritters(char board[][BOARD_SIZE], Organism* list[], int& listSize);
void breedCritters(char board[][BOARD_SIZE], Organism* list[], int& listSize);
void removeDeadAnt(int& currPosition, Organism* list[], int& listSize);
void starveDoodlebugs(char board[][BOARD_SIZE], Organism* list[], int& listSize);
int findAnt(int searchRow, int searchCol, Organism* list[], int listSize);


int main() {
    srand(time(0)); // seed random number generator
    char gameBoard[BOARD_SIZE][BOARD_SIZE];
    int timeStepCounter = 0;
    Organism *critterList[BOARD_SIZE * BOARD_SIZE]; // generates array necessary to store place values on square board
    int critterListSize;
    char userInput;
    bool loop;

    // INITIALIZATION
    initializeBoard(gameBoard);
    spawnCritters(critterList, critterListSize, 100, 25);
    removeDoubles(gameBoard, critterList, critterListSize);

    // CYCLE
    loop = true;
    while (loop) {
        initializeBoard(gameBoard);
        updateCritterAge(critterList, critterListSize);
        updateBoard(gameBoard, critterList, critterListSize);
        drawBoard(gameBoard);
        moveCritters(gameBoard, critterList, critterListSize);
        breedCritters(gameBoard, critterList, critterListSize);
        starveDoodlebugs(gameBoard, critterList, critterListSize);
        cout << "Press enter to continue. Enter any other key to quit." << endl;
        cin.get(userInput);
        if (userInput != '\n')
            loop = false;
        }

        // CLEAN-UP
        for (int i = 0; i < critterListSize; i++)
            delete critterList[i];

        return 0;
}

// Main functions
void initializeBoard(char board[][BOARD_SIZE]){
    for (int i = 0; i < BOARD_SIZE; i++){
        for (int j= 0; j < BOARD_SIZE; j++)
            board[i][j] = 'e';
    }
}
void spawnCritters(Organism* list[], int& listSize, int numAnts, int numDbugs){
    int i;
    for (i = 0; i < numAnts; i++){ list[i] = new Ant; }
    for (i = numAnts; i < (numAnts + numDbugs); i++) { list[i] = new Doodlebug; }
    listSize = i;
}
void removeDoubles(char board[][BOARD_SIZE], Organism* list[], int listSize){
    Organism *currCritter = nullptr;
    // starts by updating board, respawns if there is already a critter there
    for (int i = 0; i < listSize; i++) {
        while (board[list[i]->get_row()][list[i]->get_column()] != 'e'){ // returns the position on the board corresponding to critter's coordinates
            cout << "Current type: " << list[i]->get_type();
            board[list[i]->get_row()][list[i]->get_column()] = list[i]->get_type();
            char x = list[i]->get_type();
            delete list[i];
            if (x == 'a'){
                currCritter = new Ant();
                cout << "Got rid of double A and respawned" << endl;
            }
            else if (x == 'd'){
                currCritter = new Doodlebug();
                cout << "Got rid of double D and respawned" << endl;
            }
        }
    }
}
void updateCritterAge(Organism* list[], int listSize){
    for (int i = 0; i < listSize; i++)
        list[i]->update_age();
}
void updateBoard(char board[][BOARD_SIZE], Organism* list[], int listSize){
    for (int j = 0; j < listSize; j++) {
        board[list[j]->get_row()][list[j]->get_column()] = list[j]->get_type();
    }
}
void drawBoard(char board[][BOARD_SIZE]){
    for (int row = 0; row < BOARD_SIZE; row++){
        for (int column = 0; column < BOARD_SIZE; column++){
            switch (board[row][column]){
                case 'a':
                    cout << 'o';
                    break;
                case 'e':
                    cout << '-';
                    break;
                case 'd':
                    cout << 'X';
                    break;
                default:
                    cout << "?";
                    break;
            }
            cout << "   ";
        }
        cout << endl;
    }
}
void moveCritters(char board[][BOARD_SIZE], Organism* list[], int& listSize) {

//    // debug code for iterating critter types in list
//    for (int k = 0; k < listSize; k++)
//        cout << k << list[k]->get_type() << "  ";

    cout << endl;
    for (int i = 0; i < listSize; i++) {
        updateBoard(board, list, listSize);
        list[i]->reset_sides();
        list[i]->update_all_sides(board);
        if (list[i]->get_type() == 'd') {
            list[i]->move(board, list[i]->get_row(), list[i]->get_column());
            if(list[i]->get_remove_ant())
                removeDeadAnt(i, list, listSize);
        }
    }

    for (int i = 0; i < listSize; i++){
        updateBoard(board, list, listSize);
        list[i]->reset_sides();
        list[i]->update_all_sides(board);
        if (list[i]->get_type() == 'a') {
            list[i]->move(board, list[i]->get_row(), list[i]->get_column());
        }
    }
}
void breedCritters(char board[][BOARD_SIZE], Organism* list[], int& listSize){
    /* CAUTION: breed() increments listSize whenever new */
    for (int i = 0; i < listSize; i++) {
        updateBoard(board, list, listSize);
        list[i]->reset_sides();
        list[i]->update_all_sides(board);
        list[i]->breed(board, list, listSize);
    }
}
void starveDoodlebugs(char board[][BOARD_SIZE], Organism* list[], int& listSize) {
    for (int i = 0; i < listSize; i++) {
//        if (list[i]->get_type() == 'd') {
            if (list[i]->isStarved()) {
                delete list[i];
                list[i] = list[listSize-1];
                listSize--;
                i--;
//            }
        }
    }
}

// Helper functions (called as sub-functions of those used in main)
int findAnt(int searchRow, int searchCol, Organism* list[], int listSize){
    for (int i = 0; i < listSize; i++){
        char x = list[i]->get_type();
        if (x == 'a' && list[i]->get_row() == searchRow && list[i]->get_column() == searchCol)
            return i;
    }
    return -1;
}
void removeDeadAnt(int& k, Organism* list[], int& listSize) {
    int dead_ant_index;
    dead_ant_index = findAnt(list[k]->get_row(), list[k]->get_column(), list, listSize);
    if (dead_ant_index != -1) {
        delete list[dead_ant_index];
        if (dead_ant_index > k) {
            list[dead_ant_index] = list[listSize - 1];
            listSize--;
        }
        if (dead_ant_index < k) {
            while (dead_ant_index < listSize - 1) {
                list[dead_ant_index] = list[dead_ant_index + 1];
                dead_ant_index++;
            }
            k--;
            listSize--;
        }
        list[k]->set_remove_ant(false);
    }
}

// Class member functions
int Organism::randomNumMax19(){
    return rand() % 20;
}

void Ant::breed(char board[][BOARD_SIZE], Organism* list[], int& listSize){
    if (get_age() % 3 == 0 && get_age() > 0){
        int spawnDeck = randomNumMax3(); // randomly choose a location to multiply
        while (!isClear(spawnDeck)){ // check if space is clear
            if(isSurrounded()){
                break;
            }
            spawnDeck = randomNumMax3();
        }
        if (!isSurrounded()){
            switch(spawnDeck){
                case 0:                                     // spawn left
                    list[listSize] = new Ant(get_row(), get_column() - 1);
                    listSize++;
                    break;
                case 1:                                     // spawn above
                    list[listSize] = new Ant(get_row() - 1, get_column());
                    listSize++;
                    break;
                case 2:                                     // spawn right
                    list[listSize] = new Ant(get_row(), get_column() + 1);
                    listSize++;
                    break;
                case 3:                                     // spawn below
                    list[listSize] = new Ant(get_row() + 1, get_column());
                    listSize++;
                    break;
                default:
                    cout << "Invalid operation: program exit" << endl;
                    exit(1);
            }
        }
    }
    reset_sides();
}
void Ant::move(char board[][BOARD_SIZE], int row, int column) {
    int nextMove = randomNumMax3(); // will be 0-3
    if (isClear(nextMove)) {
        switch (nextMove) {
            case 0:         // move left
                set_column(column-1);
                break;
            case 1:         // move up
                set_row(row-1);
                break;
            case 2:         // move right
                set_column(column+1);
                break;
            case 3:         // move down
                set_row(row+1);
                break;
            default:        // error
                cout << "Illegal operation: program ended. Please contact developer" << endl;
                exit(1);
        }
    }
}
bool Ant::isClear(int move) const {
    // Returns false if illegal move attempted
    if (left && move == 0)
        return false;
    if (up && move == 1)
        return false;
    if (right && move == 2)
        return false;
    if  (down && move == 3)
        return false;

    return true;
} // checks if move is legal
void Ant::update_all_sides(char board[][20]) {
    //look around and update flags
    if (get_column()== 0 || board[get_row()][get_column()-1] != 'e')  // look left
        left = true;
    else
        left = false;
    if (get_row() == 0 || board[get_row()-1][get_column()] != 'e') // look up
        up=true;
    else
        up = false;
    if (get_column() == 19 || board[get_row()][get_column()+1] != 'e') // look right
        right = true;
    else
        right = false;
    if (get_row() == 19 || board[get_row()+1][get_column()] != 'e') // look down
        down = true;
    else
        down = false;
} // sets the ants up, down, left, right values (true if filled, false if empty)

void Doodlebug::breed(char board[][BOARD_SIZE], Organism* list[], int& listSize){
    if (get_age() % 8 == 0 && get_age() > 0){
        int spawnDeck = randomNumMax3(); // randomly choose a location to multiply
        while (!isClear(spawnDeck)){ // check if space is clear
            if(isSurrounded()){
                break;
            }
            spawnDeck = randomNumMax3();
        }
        if (!isSurrounded()){
            switch(spawnDeck){
                case 0:                                     // spawn left
                    list[listSize] = new Doodlebug(get_row(), get_column() - 1);
                    listSize++;
                    break;
                case 1:                                     // spawn above
                    list[listSize] = new Doodlebug(get_row() - 1, get_column());
                    listSize++;
                    break;
                case 2:                                     // spawn right
                    list[listSize] = new Doodlebug(get_row(), get_column() + 1);
                    listSize++;
                    break;
                case 3:                                     // spawn below
                    list[listSize] = new Doodlebug(get_row() + 1, get_column());
                    listSize++;
                    break;
                default:
                    cout << "Invalid operation: program exit" << endl;
                    exit(1);
            }
        }
    }
    reset_sides();
} //spawns a new dbug in adjacent square every 8 turns
void Doodlebug::eat(){
    if (left_type != 'a' && right_type != 'a' && up_type != 'a' && down_type != 'a'){
        until_starves--;
    }
    else{
        int adjSquare = randomNumMax3();
        while(!isAnt(adjSquare))
            adjSquare = randomNumMax3();
        switch (adjSquare) {
            case 0:         // move left
                set_column(get_column()-1);
                break;
            case 1:         // move up
                set_row(get_row()-1);
                break;
            case 2:         // move right
                set_column(get_column()+1);
                break;
            case 3:         // move down
                set_row(get_row()+1);
                break;
            default:        // error
                cout << "Illegal operation: program ended. Please contact developer" << endl;
                exit(1);
        }
        ate = true;
        until_starves = 3;
        set_remove_ant(true);
    }
} // if there is one or more ants next to the dbug, function randomly chooses one to "eat"
void Doodlebug::move(char board[][BOARD_SIZE], int row, int column) {
    // implement the dbug eat ant function
    // PRIORITY: eat an ant if its next to the dbug
    eat();

    // if no ants then move randomly:
    if (!ate) {
        int nextMove = randomNumMax3(); // will be 0-3
        if (isClear(nextMove)) {
            switch (nextMove) {
                case 0:         // move left
                    set_column(column - 1);
                    break;
                case 1:         // move up
                    set_row(row - 1);
                    break;
                case 2:         // move right
                    set_column(column + 1);
                    break;
                case 3:         // move down
                    set_row(row + 1);
                    break;
                default:        // error
                    cout << "Illegal operation: program ended. Please contact developer" << endl;
                    exit(1);
            }
        }
    }
    ate = false;
} // if adj ant, will eat the ant; otherwise, moves into empty adj square
void Doodlebug::update_all_sides(char board[][BOARD_SIZE]){
    //look around and update adjacent type
    if (get_column() == 0)   // look left
        left_type = 'b';
    else{
        left_type = board[get_row()][get_column()-1];
    }
    if (get_row() == 0)  // look up
        up_type ='b';
    else {
        up_type = board[get_row()-1][get_column()];
    }
    if (get_column() == 19)  // look right
        right_type = 'b';
    else{
        right_type = board[get_row()][get_column()+1];
    }
    if (get_row() == 19) // look down
        down_type = 'b';
    else{
        down_type = board[get_row()+1][get_column()];
    }
} // retrieves type values for adjacent squares
bool Doodlebug::isClear(int move) {
    // Returns false if illegal move
    if (left_type != 'e' && move == 0)
        return false;
    if (up_type != 'e' && move == 1)
        return false;
    if (right_type != 'e' && move == 2)
        return false;
    if  (down_type !='e' && move == 3)
        return false;

    return true;
}  // returns true if move is legal
bool Doodlebug::isAnt(int move){
    if (left_type == 'a' && move == 0)
        return true;
    if (up_type == 'a' && move == 1)
        return true;
    if (right_type == 'a' && move == 2)
        return true;
    if  (down_type == 'a' && move == 3)
        return true;

    return false;
} // returns true if the move is to the same square as an ant (eat)
bool Doodlebug::isSurrounded() const{
    //objects are 'b' (border), 'd' (Doodlebug), and 'a' (ant)... free is 'e' (empty), undefined is 'x'
    if (up_type != 'e' && down_type != 'e' && left_type != 'e' && right_type != 'e' )
        return true;
    else
        return false;
} // returns true if no empty adjacent square






