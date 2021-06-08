#include <iostream>
#include <cstdlib>
#include <ctime>
using namespace std;

// NOTE: updating board size requires also updating board[][20] across the program, changing 20 to the desired size
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
    virtual bool get_up() {return up;}
    virtual bool get_down() {return down;}
    virtual bool get_left() {return left;}
    virtual bool get_right() {return right;}
    virtual bool isStarved () {return false;}; // not used by ant, but required for virtual definition in Organism
    void set_up(){up = true;}
    void set_down() {down = true;}
    void set_left(){left = true;}
    void set_right(){right = true;}
    virtual void reset_sides() {up = false; down = false; left = false; right = false;}
    virtual void move(char board[][BOARD_SIZE], int row, int column);
    virtual char get_type() {return 'a';}
    virtual void breed(char board[][BOARD_SIZE], Organism* list[], int& listSize);
    bool isClear(int move) const;
    virtual void update_all_sides(char board[][BOARD_SIZE]);
    virtual bool isSurrounded() const {if (up && down && left && right){return true;} else {return false;}}
};
class Doodlebug: public Organism{
    int until_starves;
    char up_type, down_type, left_type, right_type;  //settings are: 'b' (border), 'd' (Doodlebug), and 'a' (ant)... free is 'e' (empty), undefined is 'x'
    bool isAnt(int move);
    bool ate;
public:
    Doodlebug(int rowNum, int columnNum) : Organism(rowNum, columnNum), until_starves(3), up_type('x'),down_type('x'), left_type('x'), right_type('x'), ate(false) {}
    Doodlebug():Organism(), until_starves(3), up_type('x'), down_type('x'), left_type('x'), right_type('x'), ate(false){}
    ~Doodlebug() =default;
    void eat();
    virtual bool isStarved() {if(until_starves ==0){return true;} else {return false;}};
    virtual bool get_up() {return up_type;}
    virtual bool get_down() {return down_type;}
    virtual bool get_left() {return left_type;}
    virtual bool get_right() {return right_type;}
    int get_until_starves() {return until_starves;}
    void set_up_type(char type){up_type = type;}
    void set_down_type(char type) {down_type = type;}
    void set_left_type(char type){left_type = type;}
    void set_right_type(char type){right_type = type;}
    bool isClear(int move);
    virtual void update_all_sides(char board[][BOARD_SIZE]);
    virtual void reset_sides() {up_type ='x'; down_type ='x'; left_type ='x'; right_type ='x';}
    virtual void move(char board[][BOARD_SIZE], int row, int column);
    virtual char get_type() {return 'd';}
    virtual void breed(char board[][BOARD_SIZE], Organism* list[], int& listSize);
    virtual bool isSurrounded() const;
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
    Organism *critterList[BOARD_SIZE * BOARD_SIZE]; // generates array necessary to store place values on square board
    int critterListSize;
    char userInput;
    bool loop;

    // INITIALIZATION
    initializeBoard(gameBoard);
    spawnCritters(critterList, critterListSize, 100, 5); // use this function to change number of starting ants and dbugs
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
/*Precondition: Takes a 2D char array composed of BOARD_SIZE*BOARD_SIZE number of squares
//Postcondition: Assigns the value 'e' to every square
*/
    for (int i = 0; i < BOARD_SIZE; i++){
        for (int j= 0; j < BOARD_SIZE; j++)
            board[i][j] = 'e';
    }
}

void spawnCritters(Organism* list[], int& listSize, int numAnts, int numDbugs){
/*Precondition: Takes the empty Organism list and uninitialized listSize, the number of ants to spawn, and the number of Doodlebugs to spawn
//Postcondition: Creates the appropriate number of new Ant and Doodlebug objects, assigning each to a unique position in the array where they
//exist as base class Organism pointers to derived class Ant or Doodlebug objects (polymorphism). Function updates listSize passed by reference
*/
    int i;
    for (i = 0; i < numAnts; i++){
        list[i] = new Ant;
    }
    for (i = numAnts; i < (numAnts + numDbugs); i++) {
        list[i] = new Doodlebug;
    }
    listSize = i;
}

void removeDoubles(char board[][BOARD_SIZE], Organism* list[], int listSize){
/*Precondition: Takes the 2D array board, and the Organism array immediately after critters have been initially spawned.
//Postcondition: Iterates through 'list', checking the row-column position to see if a previous Organism in the array was already assigned to that
//same row-column pair. If the row-column pair comes back as already assigned, then the function respawns the organism in currentindex position 'i'.
//When creating a new Ant or Doodlebug, a new row-column pair is randomly assigned. If this new row-column pair is unique (i.e. does not belong to a
//previous Ant or Doodlebug), then the Organism's type is assigned to the empty square on the board.
*/
    for (int i = 0; i < listSize; i++) {
        while (board[list[i]->get_row()][list[i]->get_column()] != 'e'){
            board[list[i]->get_row()][list[i]->get_column()] = list[i]->get_type();
            char x = list[i]->get_type();
            delete list[i];
            if (x == 'a'){
                list[i] = new Ant();
                cout << list[i]->get_row() << " " << list[i]->get_column() << " occupied - respawning Ant to new location" << endl;
            }
            else if (x == 'd'){
                list[i] = new Doodlebug();
                cout << list[i]->get_row() << " " << list[i]->get_column() << " occupied - respawning Doodlebug to new location" << endl;
            }
        }
        if (board[list[i]->get_row()][list[i]->get_column()] == 'e')
            board[list[i]->get_row()][list[i]->get_column()] = list[i]->get_type();
    }
}

void updateCritterAge(Organism* list[], int listSize){
/*Precondition: Takes the Organism* array and its size
//Postcondition: Increments the age variable in the Organism class
*/
    for (int i = 0; i < listSize; i++)
        list[i]->update_age();
}

void updateBoard(char board[][BOARD_SIZE], Organism* list[], int listSize){
/*Precondition: Takes the 2D char array board, the Organism* array, and the array size
//Postcondition: Updates the 2D-char array by plotting each organism on the 2D array according to its row-column pair
*/
    for (int j = 0; j < listSize; j++) {
        board[list[j]->get_row()][list[j]->get_column()] = list[j]->get_type();
    }
}

void drawBoard(char board[][BOARD_SIZE]){
/*Precondition: Takes the updated 2D char array board
//Postcondition: Outputs the board to the screen, indicating the position of Ants with 'o', Doodlebugs with 'X', and empty squares with '-'
*/
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
/*Precondition: Takes the updated 2D char array board, the organism array, and the array size
//Postcondition: Moves the Organisms one by one, first the Doodlebugs, then the Ants. Doodlebugs can 'eat' ants (implemented as subfunction of move())
during their turns. After every move, eaten ants are removed. The start of every turn updates the board so that only one Organism can occupy a space
at any given time. listSize is called by reference because it can change depending on the outcomes of the move phase (i.e. an ant being eaten)
*/


    // Move Doodlebugs
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
    // Move Ants
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
/*Precondition: Takes 2D char array board, Organism* array, and Organism* array size. Each Organism* should point to an Ant or Doodlebug derived class
//object with a derived-class specific definition for breed
//Postcondition: Calls the breed function yfor each item in the Organism* list array and updates the listSize accordingl
*/
    for (int i = 0; i < listSize; i++) {
        updateBoard(board, list, listSize);
        list[i]->reset_sides();
        list[i]->update_all_sides(board);
        list[i]->breed(board, list, listSize);
    }
}

void starveDoodlebugs(char board[][BOARD_SIZE], Organism* list[], int& listSize) {
/*Precondition: Takes 2D char array board, Organism* array, and Organism* array size. Because of polymorphism (Organism* points to dervived class object)
//isStarved must be defined for both Ants and Doodlebugs, although only Doodlebugs' behavior is affected by starving
//Postcondition: Checks isStarved() and if returns true, then the Organism is removed from the list. The last item in the list in placed in the empty
//array position, and the array size and index position are decremented. Note: isStarved always returns false for Ants; for Doodlebugs, it will check
//to see if an ant has been 'eaten' in the last 3 turns
*/
    for (int i = 0; i < listSize; i++) {
        if (list[i]->isStarved()) {
            delete list[i];
            list[i] = list[listSize-1];
            listSize--;
            i--;
        }
    }
}


// Helper functions (called as sub-functions of those used in main)
int findAnt(int searchRow, int searchCol, Organism* list[], int listSize){
/*Precondition: Takes the target row and column values, the Organism array, and the array size
//Postcondition: Returns the index position of the ant with the matching row and column values. Returns -1 if not found
//(indicates an error in this program since function is only called when there is a matching ant, and we just need to know where the ant 'lives')
*/
    for (int i = 0; i < listSize; i++){
        char x = list[i]->get_type();
        if (x == 'a' && list[i]->get_row() == searchRow && list[i]->get_column() == searchCol)
            return i;
    }
    return -1;
}

void removeDeadAnt(int& k, Organism* list[], int& listSize) {
/*Used to remove an ant that has been 'eaten' by Doodlebugs
//Precondition: Takes the array index 'k' of the Doodlebug that ate an ant, the Organism array, and the size of the array
//Postcondition: Calls a subfunction 'findAnt' that returns the array position of the ant having the same row and column values as the Doodlebug. After
//deleting the ant and returning memory to the heap, if the ant is in a posterior index position (i.e. has not been read in yet by the move function),
//then the last object in the array is placed in the now empty array position. No adjustment is needed for 'k' because the changes took place
//before the information was read in by the originating function.
//If the index position of the ant is anterior to the Doodlebug, then all values in the array must be shifted down by one position. The index is
//decremented by 1 to reflect the shift (hence why 'k' must be passed in as a reference), and array size is decremented by one
*/
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
//Returns a random number 0-19
    return rand() % 20;
}

void Ant::breed(char board[][BOARD_SIZE], Organism* list[], int& listSize){
/*Spawns a new Ant in a random empty adjacent square every 3 turns
//Precondition: Takes the 2D board array with updated values, the Organism array containing all critters on the board, and the current size
//of the critter array
//Postcondition: If one or more adjacent spaces are available, randomly selects one of the spaces and creates a new Ant inside of it,
//adds the Ant to the array, and updates the critter array size
*/
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
/* Precondition: Takes a 2D board array containing updated values for board, the row position of the calling Ant object, and its column position
*  Postcondition: Randomly selects an adjacent empty square to move to. Move is executed by using the setter to reassign the member variable. If
*  there are no empty squares, then the Ant does not move.
*/
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
// Precondition: Takes the attempted move mapped to an integer and checks it for legality
// Postcondition: Returns false if illegal move attempted
    if (left && move == 0)
        return false;
    if (up && move == 1)
        return false;
    if (right && move == 2)
        return false;
    if  (down && move == 3)
        return false;

    return true;
}

void Ant::update_all_sides(char board[][20]) {
/*Precondition: Takes as input the gameboard, which should already contain a char type in every location corresponding to the status of the square.
//Postcondition: hen called on an Ant object, function assigns true to the variable when the adjacent space is occupied or a border. Returns false if it is empty
*/
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
}

void Doodlebug::breed(char board[][BOARD_SIZE], Organism* list[], int& listSize){
/*Spawns a new Doodlebug in a random empty adjacent square every 8 turns
//Precondition: Takes the 2D board array with updated values, the Organism array containing all critters on the board, and the current size
//of the critter array
//Postcondition: If one or more adjacent spaces are available, randomly selects one of the spaces and creates a new Doodlebug inside of it,
//adds the Doodlebug to the array, and updates the critter array size
*/
    if (get_age() % 8 == 0 && get_age() > 0){
        int spawnDeck = randomNumMax3();
        while (!isClear(spawnDeck)){
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
}

void Doodlebug::eat(){
/* Precondition: Doodlebug object calls the function
// Postcondition: If there is no adjacent ant, the remaining turns until the Doodlbug starves is decremented.If there is one or more ants next to
// the dbug, function randomly chooses one to "eat" and a flag is also set on the Doodlebug to indicate that the ant previously residing in that
// location needs to be removed from the array. Since the Doodlebug has eaten, the number of turns until starving is restored to 3.
*/
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
}

void Doodlebug::move(char board[][BOARD_SIZE], int row, int column) {
/* Precondition: Takes a 2D board array containing updated values for board, the row position of the calling Doodlebug object, and its column position
*  Postcondition: Attempts to "eat" an ant in an adjacent square and move to the eaten ant's square. If no ant is adjacent, then the function randomly
*  selects an adjacent empty square to move to. Move is executed by using the setter to reassign the member variable. If there are no empty squares,
*  then the Doodlebug does not move.
*/
    eat(); //subfunction 'eat' will set bool value "ate" to true if eating was successful
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
}

void Doodlebug::update_all_sides(char board[][BOARD_SIZE]){
/* Precondition: Takes the 2D board array with updated values for each "square" and sets the Doodlebug's directional variables to
*  the values of its adjacent squares.
*  Postcondition: The Doodlebug's directional variables (up_type, down_type, left_type, right_type) contain the updated values of the adjacent squares
*/
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
}

bool Doodlebug::isClear(int move) {
/* Precondition: Takes the attempted move mapped to an integer and checks it for legality
*  Postcondition: Returns false if illegal move attempted
*/

    if (left_type != 'e' && move == 0)
        return false;
    if (up_type != 'e' && move == 1)
        return false;
    if (right_type != 'e' && move == 2)
        return false;
    if  (down_type !='e' && move == 3)
        return false;

    return true;
}

bool Doodlebug::isAnt(int move){
/* Precondition: Used by 'eat' function, takes the attempted move mapped to an integer
*  Postcondition: Returns true if the adjacent square is an ant
*/
    if (left_type == 'a' && move == 0)
        return true;
    if (up_type == 'a' && move == 1)
        return true;
    if (right_type == 'a' && move == 2)
        return true;
    if  (down_type == 'a' && move == 3)
        return true;

    return false;
}

bool Doodlebug::isSurrounded() const{
/* Precondition: Doodlebug object calls the member function
*  Postcondition: Returns true if there is no empty adjacent square
*/
    //chars stand for 'b' (border), 'd' (Doodlebug), and 'a' (ant)... free is 'e' (empty), undefined is 'x'
    if (up_type != 'e' && down_type != 'e' && left_type != 'e' && right_type != 'e' )
        return true;
    else
        return false;
}