#include <ncurses.h>  //needs -lncurses gcc flag
#include <time.h>   //srand(time(null))
#include <stdlib.h>	//malloc
#include <assert.h> //assert()


/* 
	- added gameOver
	- added dropPiece
*/

/*=========================== typedefs ======================================*/

typedef struct BLOCK {
	int x,y,destroyed;
	char *character;
} BLOCK;

typedef struct PIECE {
	int type,orientation;    
	/*  
		type: 1 = line, 2 = box, 3 = z, 4 = s, 5 = L, 6 = J, 7 = T
		orientation: position of block as it is fliped around. positions 0 - 3 
	*/
	BLOCK *blocks[4];
} PIECE;

/*=========================== macros, global vars, and constants ====================*/

#define HEIGHT 30
#define WIDTH 10
#define CURRENTBLOCK gamepieces[piece_number-1]
#define CURRENTBLOCK_INDEX CURRENTBLOCK->blocks[i]
#define BLOCK_ORIENTATION CURRENTBLOCK->orientation
#define BLOCK_I_J_INDEX gamepieces[i]->blocks[j]
#define BLOCK_TO_REMOVE gamepieces[mapbits[y_val][i]-1]->blocks[j]

const size_t SIZE_PIECE = sizeof(PIECE);
const size_t SIZE_BLOCK = sizeof(BLOCK);
const int REAL_WIDTH = WIDTH * 2;

int piece_number = 0;
int array_size = 10;
int mapbits[HEIGHT][WIDTH];
int game_continue = 1;
int player_score = 0;
int next_piece,current_piece;
PIECE **gamepieces;
WINDOW *game_win;
WINDOW *prompt_win;

/*=========================== prototypes ====================================*/

BLOCK *makeLine(const int i);
BLOCK *makeSquare(const int i);
BLOCK *makeZ(const int i);
BLOCK *makeS(const int i);
BLOCK *makeL(const int i);
BLOCK *makeJ(const int i);
BLOCK *makeT(const int i);
BLOCK* (*getMakePiece(void))(const int);
void increasePieceNumber(void);
void printBlock(void);
void moveX(const int add);
int moveY(const int add);
void flipLineV(void);
void flipLineH(void);
void newPiece(void);
void flipPiece(void);
void flipLine(void);
void flipZ(void);
void flipS(void);
void flipL(void);
void flipJ(void);
void flipT(void);
void flipZH(void);
void flipZV(void);
void flipS(void); 
void flipSH(void); 
void flipSV(void); 
void flipL(void);
void flipLH(const int i);
void flipLV(const int i);
void flipLDown(const int i);
void flipLUp(const int i);
void flipJ(void);
void flipJH(const int i);
void flipJV(const int i);
void flipJDown(const int i);
void flipJUp(const int i);
void flipTH(const int i);
void flipTV(const int i);
void flipTDown(const int i);
void flipTUp(const int i);
void initMapbits(void);
void initColors(void);
void placePiece(void);
int detectCollisionX(const int add);
int detectCollisionY(const int add);
void initNcur(void);
void initWindow(void);
void gameLoop(void);
int detectEdgeX(const int add);
int detectEdgeY(const int add);
void checkLines(void);
void removeLines(int const y_val);
int randint(int n);
int spaceOccupied(void);
void dropLines(const int y_val);
void setBlocks(const int y_val);
void getNextPiece(void);
void showNextPiece(void); 
void gameOver(void);
void dropPiece(void);

/*=========================== main program ==================================*/

int main(void) {
	gamepieces = malloc(SIZE_PIECE * array_size);
	initNcur();
	initNcur();
	initColors();
	initWindow();			
	initMapbits();		
	srand(time(NULL));
	next_piece = randint(7)+1;
	getNextPiece();
	printBlock();
	gameLoop();
	gameOver();
	return 0;
}

void gameLoop(void) {
	while(game_continue) {	
		switch(getch()){	
			case KEY_LEFT:
			moveX(-2);
			break;
		case KEY_RIGHT:
			moveX(2);
			break;
		case KEY_UP:
			flipPiece();
			break;
		case KEY_DOWN:
			moveY(1);
			break;	
		case 'n':
			dropPiece();
			break;
		case KEY_F(1): game_continue = 0;
			break;
		default: //do nothing
			break;
	}
		printBlock();
	}
}

void gameOver(void){
	int i,j;
	attron(COLOR_PAIR(1));
	attron(A_BOLD);
	mvprintw(HEIGHT/2,(REAL_WIDTH/2)-4,"Game Over!");
	refresh();
	for (i = 0; i < piece_number; i++){
		for(j = 0; j < 4; j++) {
			free(BLOCK_I_J_INDEX);
		}
		free(gamepieces[i]);
	}
	getch();
	endwin();
}


//increase pice_number. check if array is out of space, adn if so attempt to use realloc to get more space
void increasePieceNumber(void) {
	piece_number++;
	if (piece_number >= array_size) {
		PIECE **new_p = realloc(gamepieces,(SIZE_PIECE * array_size) + (SIZE_PIECE * 10));
		if(new_p == NULL) {
			new_p = realloc(gamepieces,(SIZE_PIECE * array_size) + (SIZE_PIECE * 10));
		}
		gamepieces = new_p;
		array_size += 10;
	}
}

//makes new piece
void newPiece(void) {
	int i;
	BLOCK* (*makePiece)(const int i);  //function pointer for make piece function
	increasePieceNumber();
	makePiece = getMakePiece();
	CURRENTBLOCK = malloc(SIZE_PIECE);
	CURRENTBLOCK->type = current_piece;
	for(i = 0; i < 4; i++) {
		CURRENTBLOCK_INDEX = makePiece(i);
		CURRENTBLOCK_INDEX->character = malloc(3);
		CURRENTBLOCK_INDEX->character = "[]";
		CURRENTBLOCK_INDEX->destroyed = 0;
	}

	//if start location is already full, game is over
	if(spaceOccupied() == 1) {
		game_continue = 0;
	}
}

//returns function pointer to correct make piece function
BLOCK* (*getMakePiece(void))(const int) {
	switch(current_piece) {
		case 1: return makeLine;
			break;
		case 2: return makeSquare;
			break;
		case 3: return makeZ;
			break;
		case 4: return makeS;
			break;
		case 5: return makeL;
			break;
		case 6: return makeJ;
			break;
		case 7: return makeT;
			break;
		default: return makeLine;  //shouldnt ever get here. mostly to keep compiler happy
			break;
	}
}


BLOCK *makeLine(const int i){
	BLOCK *block = malloc(SIZE_BLOCK);
	block->y = 0 + i;	
	block->x = REAL_WIDTH - 4;

	return block;
}

BLOCK *makeSquare(const int i){ 
	BLOCK *block = malloc(SIZE_BLOCK);
	switch (i) {
		case 0:
			block->y = 0;
			block->x = REAL_WIDTH - 6;
			break;
		case 1:
			block->y = 0;
			block->x = REAL_WIDTH - 4;
			break;
		case 2:
			block->y = 1;
			block->x = REAL_WIDTH - 6;
			break;
		case 3:
			block->y = 1;
			block->x = REAL_WIDTH - 4;
			break;
		default : //do nothing. should not get here
			break;	
	}
	return block;
}

BLOCK *makeZ(const int i){ 
	BLOCK *block = malloc(SIZE_BLOCK);	
	switch (i) {
		case 0:
			block->y = 0;
			block->x = REAL_WIDTH - 8;
			break;
		case 1:
			block->y = 0;
			block->x = REAL_WIDTH - 6;
			break;
		case 2:
			block->y = 1;
			block->x = REAL_WIDTH - 6;
			break;
		case 3:
			block->y = 1;
			block->x = REAL_WIDTH - 4;
			break;
		default : //do nothing. should not get here
			break;	
	}
	return block;
}

BLOCK *makeS(const int i){ 
	BLOCK *block = malloc(SIZE_BLOCK);
	switch (i) {
		case 0:
			block->y = 0;
			block->x = REAL_WIDTH - 6;
			break;
		case 1:
			block->y = 0;
			block->x = REAL_WIDTH - 4;
			break;
		case 2:
			block->y = 1;
			block->x = REAL_WIDTH - 8;
			break;
		case 3:
			block->y = 1;
			block->x = REAL_WIDTH - 6;
			break;
		default : //do nothing. should not get here
			break;	
		}
	return block;
}

BLOCK *makeL(const int i){ 
	BLOCK *block = malloc(SIZE_BLOCK);	
	switch (i) {
		case 0: //fall through to case 2
		case 1: //fall through to case 2
		case 2:
			block->y = 0 + i;
			block->x = REAL_WIDTH - 6;
			break;
		case 3:
			block->y = 2;
			block->x = REAL_WIDTH - 4;
			break;
		default : //do nothing. should not get here
			break;	
		}
	return block;
}

BLOCK *makeJ(const int i){ 
	BLOCK *block = malloc(SIZE_BLOCK);
	switch (i) {
		case 0: //fall through to case 2
		case 1: //fall through to case 2
		case 2:
			block->y = 0 + i;
			block->x = REAL_WIDTH - 4;
			break;
		case 3:
			block->y = 2;
			block->x = REAL_WIDTH - 6;
			break;
		default : //do nothing. should not get here
			break;	
		}
	return block;
}

BLOCK *makeT(const int i){ 
	BLOCK *block = malloc(SIZE_BLOCK);
	switch (i) {
		case 0: //fall through to case 2
		case 1: //fall through to case 2
		case 2:
			block->y = 0 + i;
			block->x = REAL_WIDTH - 6;
			break;
		case 3:
			block->y = 1;
			block->x = REAL_WIDTH - 4;
			break;
		default : //do nothing. should not get here
			break;	
		}
	return block;
}

void flipPiece(void) {
	switch(CURRENTBLOCK->type) {
		case 1:	flipLine();
			break;
		case 2: //do nothing. squares dont need to be flipped
			break;
		case 3: flipZ();
			break;
		case 4: flipS();
			break;
		case 5: flipL();
			break;
		case 6: flipJ();
			break;
		case 7: flipT();
			break;
		default: //do nothing. should never get here
			break;
	}
}

void flipLine(void) {
	if(BLOCK_ORIENTATION == 1) {
		flipLineH();
	}
	else {
		flipLineV();
	}
}

void flipLineV(void) {
	int i;
	int incrx = 2;
	int incry = 1;
	for(i = 2; i >= 0; i--){
		CURRENTBLOCK_INDEX->x -= incrx++;
		CURRENTBLOCK_INDEX->y += incry++;
		incrx++;
	}
	BLOCK_ORIENTATION = 1;

	//if flipping block would make it collide with edge or another block, undo flip
	if(detectCollisionY(0) == 1 || detectCollisionX(0) == 1 || detectEdgeX(0) == 1 || detectEdgeY(0) == 1) {
		flipLine();
	}
}

void flipLineH(void) {
	int i;
	int incrx = 2;
	int incry = 1;
	for(i = 2; i >= 0; i--){
		CURRENTBLOCK_INDEX->x += incrx++;
		CURRENTBLOCK_INDEX->y -= incry++;
		incrx++;
	}
	BLOCK_ORIENTATION = 0;

	//if flipping block would make it collide with edge or another block, undo flip
	if(detectCollisionY(0) == 1 || detectCollisionX(0) == 1 || detectEdgeX(0) == 1 || detectEdgeY(0) == 1) {
		flipLine();
	}
}

void flipZ(void) {
	switch(BLOCK_ORIENTATION) {
		case 0:flipZV();
			break;
		case 1: flipZH();
			break;
		default : //do nothing
			break;
	}
	
}

void flipZH(void) {
	CURRENTBLOCK->blocks[0]->x -= 4;
	CURRENTBLOCK->blocks[1]->y -= 2;
	BLOCK_ORIENTATION = 0;

	//if flipping block would make it collide with edge or another block, undo flip
	if(detectCollisionY(0) == 1 || detectCollisionX(0) == 1 || detectEdgeX(0) == 1 || detectEdgeY(0) == 1) {
		flipZ();
	}
}

void flipZV(void) {
	CURRENTBLOCK->blocks[0]->x += 4;
	CURRENTBLOCK->blocks[1]->y += 2;
	BLOCK_ORIENTATION = 1;

	//if flipping block would make it collide with edge or another block, undo flip
	if(detectCollisionY(0) == 1 || detectCollisionX(0) == 1 || detectEdgeX(0) == 1 || detectEdgeY(0) == 1) {
		flipZ();
	}
}

void flipS(void) {
	if (BLOCK_ORIENTATION == 1) {
		flipSH();
	}
	else {
		flipSV();
	}
}

void flipSH(void) {
	CURRENTBLOCK->blocks[2]->x -= 4;
	CURRENTBLOCK->blocks[1]->y -= 2;
	BLOCK_ORIENTATION = 0;

	//if flipping block would make it collide with edge or another block, undo flip
	if(detectCollisionY(0) == 1 || detectCollisionX(0) == 1 || detectEdgeX(0) == 1 || detectEdgeY(0) == 1) {
		flipS();
	}
}

void flipSV(void) {
	CURRENTBLOCK->blocks[2]->x += 4;
	CURRENTBLOCK->blocks[1]->y += 2;
	BLOCK_ORIENTATION = 1;

	//if flipping block would make it collide with edge or another block, undo flip
	if(detectCollisionY(0) == 1 || detectCollisionX(0) == 1 || detectEdgeX(0) == 1 || detectEdgeY(0) == 1) {
		flipS();
	}
}

void flipL(void) {
	switch(BLOCK_ORIENTATION) {
		case 0:flipLH(1);
			break;
		case 1:flipLV(1);
			break;
		case 2:flipLDown(1);
			break;
		case 3:flipLUp(1);
			break;
		default:  //do nothing. shouldnt ever reach here
			break;		
	}
}

void flipLH(const int i) {
	CURRENTBLOCK->blocks[0]->x -= (2 * i);
	CURRENTBLOCK->blocks[0]->y += (1 * i);
	CURRENTBLOCK->blocks[1]->x -= (2 * i);
	CURRENTBLOCK->blocks[1]->y += (1 * i);
	CURRENTBLOCK->blocks[2]->y -= (1 * i);
	CURRENTBLOCK->blocks[3]->y -= (1 * i);

	//if flipping block would make it collide with edge or another block, undo flip
	if(i == 1) {
		if (detectCollisionY(0) == 1 || detectCollisionX(0) == 1 || detectEdgeX(0) == 1 || detectEdgeY(0) == 1) {
			BLOCK_ORIENTATION = 0;
			flipLH(-1);
		}
		else {
			BLOCK_ORIENTATION = 1;
		}
	}
}

void flipLV(const int i) {
	CURRENTBLOCK->blocks[0]->x += (2 * i);
	CURRENTBLOCK->blocks[1]->x += (2 * i);
	CURRENTBLOCK->blocks[2]->y -= (1 * i); 
	CURRENTBLOCK->blocks[2]->x -= (2 * i); 
	CURRENTBLOCK->blocks[3]->y -= (1 * i); 
	CURRENTBLOCK->blocks[3]->x -= (2 * i);

	//if flipping block would make it collide with edge or another block, undo flip
	if(i == 1) {
		if (detectCollisionY(0) == 1 || detectCollisionX(0) == 1 || detectEdgeX(0) == 1 || detectEdgeY(0) == 1) {
			BLOCK_ORIENTATION = 1;
			flipLV(-1);
		}
		else{
			BLOCK_ORIENTATION = 2;
		}
	}
}

void flipLDown(const int i) {
	CURRENTBLOCK->blocks[0]->y -= (1 * i);
	CURRENTBLOCK->blocks[0]->x += (2 * i);
	CURRENTBLOCK->blocks[1]->y -= (1 * i);
	CURRENTBLOCK->blocks[1]->x += (2 * i);
	CURRENTBLOCK->blocks[2]->y += (1 * i);
	CURRENTBLOCK->blocks[3]->y += (1 * i);

	//if flipping block would make it collide with edge or another block, undo flip
	if(i == 1) {
		if (detectCollisionY(0) == 1 || detectCollisionX(0) == 1 || detectEdgeX(0) == 1 || detectEdgeY(0) == 1) {
			BLOCK_ORIENTATION = 2;
			flipLDown(-1);
		}
		else {
			BLOCK_ORIENTATION = 3;
		}
	}
}
void flipLUp(const int i) {
	CURRENTBLOCK->blocks[0]->x -= (2 * i);
	CURRENTBLOCK->blocks[1]->x -= (2 * i);
	CURRENTBLOCK->blocks[2]->y += (1 * i);
	CURRENTBLOCK->blocks[2]->x += (2 * i);
	CURRENTBLOCK->blocks[3]->y += (1 * i);
	CURRENTBLOCK->blocks[3]->x += (2 * i);

	//if flipping block would make it collide with edge or another block, undo flip
	if(i == 1) {
		if (detectCollisionY(0) == 1 || detectCollisionX(0) == 1 || detectEdgeX(0) == 1 || detectEdgeY(0) == 1) {
			BLOCK_ORIENTATION = 3;
			flipLUp(-1);
		}
		else {
			BLOCK_ORIENTATION = 0;
		}
	}
}

void flipJ(void) {
	switch(BLOCK_ORIENTATION) {
		case 0:flipJH(1);
			break;
		case 1:flipJV(1);
			break;
		case 2:flipJDown(1);
			break;
		case 3:flipJUp(1);
			break;
		default:  //do nothing. shouldnt ever reach here
			break;		
	}
}

void flipJH(const int i) {
	CURRENTBLOCK->blocks[0]->x -= (2 * i);
	CURRENTBLOCK->blocks[1]->x -= (2 * i);
	CURRENTBLOCK->blocks[2]->x += (2 * i);
	CURRENTBLOCK->blocks[2]->y -= (1 * i);
	CURRENTBLOCK->blocks[3]->x += (2 * i);
	CURRENTBLOCK->blocks[3]->y -= (1 * i);	

	//if flipping block would make it collide with edge or another block, undo flip
	if(i == 1) {
		if (detectCollisionY(0) == 1 || detectCollisionX(0) == 1 || detectEdgeX(0) == 1 || detectEdgeY(0) == 1) {
			BLOCK_ORIENTATION = 0;
			flipJH(-1);
		}
		else {
			BLOCK_ORIENTATION = 1;
		}
	}
}

void flipJV(const int i) {
	CURRENTBLOCK->blocks[0]->y += (1 * i);
	CURRENTBLOCK->blocks[0]->x += (2 * i);
	CURRENTBLOCK->blocks[1]->y += (1 * i);
	CURRENTBLOCK->blocks[1]->x += (2 * i);
	CURRENTBLOCK->blocks[2]->y -= (1 * i); 
	CURRENTBLOCK->blocks[3]->y -= (1 * i);

	//if flipping block would make it collide with edge or another block, undo flip
	if(i == 1) {
		if (detectCollisionY(0) == 1 || detectCollisionX(0) == 1 || detectEdgeX(0) == 1 || detectEdgeY(0) == 1) {
			BLOCK_ORIENTATION = 1;
			flipJV(-1);
		}
		else {
			BLOCK_ORIENTATION = 2;
		}
	}
}

void flipJDown(const int i) { 
	CURRENTBLOCK->blocks[0]->x += (2 * i);
	CURRENTBLOCK->blocks[1]->x += (2 * i);
	CURRENTBLOCK->blocks[2]->y += (1 * i);
	CURRENTBLOCK->blocks[2]->x -= (2 * i);
	CURRENTBLOCK->blocks[3]->y += (1 * i);
	CURRENTBLOCK->blocks[3]->x -= (2 * i);

	//if flipping block would make it collide with edge or another block, undo flip
	if(i == 1) {
		if (detectCollisionY(0) == 1 || detectCollisionX(0) == 1 || detectEdgeX(0) == 1 || detectEdgeY(0) == 1) {
			BLOCK_ORIENTATION = 2;
			flipJDown(-1);
		}
		else {
			BLOCK_ORIENTATION = 3;
		}
	}
}

void flipJUp(const int i) {
	CURRENTBLOCK->blocks[0]->y -= (1 * i);
	CURRENTBLOCK->blocks[0]->x -= (2 * i);
	CURRENTBLOCK->blocks[1]->y -= (1 * i);
	CURRENTBLOCK->blocks[1]->x -= (2 * i);
	CURRENTBLOCK->blocks[2]->y += (1 * i);
	CURRENTBLOCK->blocks[3]->y += (1 * i);

	//if flipping block would make it collide with edge or another block, undo flip
	if(i == 1) {
		if (detectCollisionY(0) == 1 || detectCollisionX(0) == 1 || detectEdgeX(0) == 1 || detectEdgeY(0) == 1) {
			BLOCK_ORIENTATION = 3;
			flipJUp(-1);
		}
		else {
			BLOCK_ORIENTATION = 0;
		}
	}
}

void flipT(void) {
	switch(BLOCK_ORIENTATION) {
		case 0:flipTH(1);
			break;
		case 1:flipTV(1);
			break;
		case 2:flipTDown(1);
			break;
		case 3:flipTUp(1);
			break;
		default:  //do nothing. shouldnt ever reach here
			break;		
	}
}

void flipTH(const int i){
	CURRENTBLOCK->blocks[0]->x -= (2 * i);
	CURRENTBLOCK->blocks[0]->y += (1 * i);

	//if flipping block would make it collide with edge or another block, undo flip
	if(i == 1) {
		if (detectCollisionY(0) == 1 || detectCollisionX(0) == 1 || detectEdgeX(0) == 1 || detectEdgeY(0) == 1) {
			BLOCK_ORIENTATION = 0;
			flipTH(-1);
		}
		else {
			BLOCK_ORIENTATION = 1;
		}
	}
}

void flipTV(const int i){
	CURRENTBLOCK->blocks[3]->x -= (2 * i);
	CURRENTBLOCK->blocks[3]->y -= (1 * i);

	//if flipping block would make it collide with edge or another block, undo flip
	if(i == 1) {
		if (detectCollisionY(0) == 1 || detectCollisionX(0) == 1 || detectEdgeX(0) == 1 || detectEdgeY(0) == 1) {
			BLOCK_ORIENTATION = 1;
			flipTV(-1);
		}
		else {
			BLOCK_ORIENTATION = 2;
		}
	}
}

void flipTDown(const int i){
	CURRENTBLOCK->blocks[2]->x += (2 * i);
	CURRENTBLOCK->blocks[2]->y -= (1 * i);

	//if flipping block would make it collide with edge or another block, undo flip
	if(i == 1) {
		if (detectCollisionY(0) == 1 || detectCollisionX(0) == 1 || detectEdgeX(0) == 1 || detectEdgeY(0) == 1) {
			BLOCK_ORIENTATION = 2;
			flipTDown(-1);
		}
		else {
			BLOCK_ORIENTATION = 3;
		}
	}
}

void flipTUp(const int i){
	CURRENTBLOCK->blocks[0]->x += (2 * i);
	CURRENTBLOCK->blocks[0]->y -= (1 * i);
	CURRENTBLOCK->blocks[2]->x -= (2 * i);
	CURRENTBLOCK->blocks[2]->y += (1 * i);
	CURRENTBLOCK->blocks[3]->x += (2 * i);
	CURRENTBLOCK->blocks[3]->y += (1 * i);

	//if flipping block would make it collide with edge or another block, undo flip
	if(i == 1) {
		if (detectCollisionY(0) == 1 || detectCollisionX(0) == 1 || detectEdgeX(0) == 1 || detectEdgeY(0) == 1) {
			BLOCK_ORIENTATION = 3;
			flipTUp(-1);
		}
		else {
			BLOCK_ORIENTATION = 0;
		}
	}
}


void dropPiece(void){
	while(moveY(1) != 1) {
		//move piece down until it hits another piece or the bottom edge
	}
}

void moveX(const int add) {
	int i;
	if (detectCollisionX(add) != 1 && detectEdgeX(add) != 1) {
		for (i = 0; i < 4; i++) {
			CURRENTBLOCK_INDEX->x += add;
		}
	}
}

int moveY(const int add) {
	int i;
	//if block is moved ontop of another, or onto the bottom of map, place it
	if (detectCollisionY(add) == 1 || detectEdgeY(add) == 1) {
		placePiece();
		return 1;
	}
	else {
		for (i = 0; i < 4; i++) {
			CURRENTBLOCK_INDEX->y += add;
		}
	return 0;
	}
}

int detectCollisionY(const int add) {
	int i;
	for (i = 0; i < 4; i++) {
		if(mapbits[CURRENTBLOCK_INDEX->y + add][CURRENTBLOCK_INDEX->x/2] != 0) {
			return 1;
		}
	}
	return 0;
}
int detectCollisionX(const int add) {
	int i;
	for (i = 0; i < 4; i++) {
		if (mapbits[(CURRENTBLOCK_INDEX->y)][((CURRENTBLOCK_INDEX->x) + add)/2] != 0 ){
			return 1;
		}
	}
	return 0;
}

int detectEdgeX(const int add) { 
	int i;
	for(i = 0; i < 4; i++) {
		if(CURRENTBLOCK_INDEX->x + add == -2 || CURRENTBLOCK_INDEX->x + add == REAL_WIDTH) {
			return 1;
		}
	}
	return 0;
}

int detectEdgeY(const int add) { 
	int i;
	for(i = 0; i < 4; i++) {
		if(CURRENTBLOCK_INDEX->y + add <= -2 || CURRENTBLOCK_INDEX->y + add == HEIGHT) {
			return 1;
		}
	}
	return 0;
}

int spaceOccupied(void) {
	int i;
	for (i = 0; i < 4; i++) {
		if(mapbits[CURRENTBLOCK_INDEX->y][CURRENTBLOCK_INDEX->x/2] != 0) {
			return 1;
		}
	}
	return 0;
}

void placePiece(void) {
	int i;
	for (i = 0; i < 4; i++) {
		mapbits[CURRENTBLOCK_INDEX->y][CURRENTBLOCK_INDEX->x/2] = piece_number;
	}
	checkLines();
	getNextPiece();
	refresh();
}

void printBlock(void) {	
	int i,j;
	wclear(game_win);
	for(i = 0; i < piece_number; i++) {
		wattron(game_win,COLOR_PAIR(gamepieces[i]->type));
		for (j = 0; j < 4; j++) {
			if (BLOCK_I_J_INDEX->destroyed == 0){
				mvwprintw(game_win,BLOCK_I_J_INDEX->y, BLOCK_I_J_INDEX->x,"%s",BLOCK_I_J_INDEX->character);
			}	
		}
		wattroff(game_win,COLOR_PAIR(gamepieces[i]->type));
	}
	wrefresh(game_win);
	curs_set(0);
}

void initMapbits(void) {
	int i,j;
	for(i = 0; i < HEIGHT; i++) {
		for(j = 0; j < WIDTH; j++)
			mapbits[i][j] = 0;
	}
}

void initColors(void) {		
	start_color();
	init_pair(1,COLOR_RED,COLOR_BLACK);
	init_pair(2,COLOR_BLUE,COLOR_BLACK);
	init_pair(3,COLOR_GREEN,COLOR_BLACK);
	init_pair(4,COLOR_MAGENTA,COLOR_BLACK);
	init_pair(5,COLOR_YELLOW,COLOR_BLACK);
	init_pair(6,COLOR_CYAN,COLOR_BLACK);
	init_pair(7,COLOR_WHITE,COLOR_BLACK);
}

void initNcur(void) {
	curs_set(0);	
	initscr();	
	cbreak();	
	keypad(stdscr, TRUE);		
	noecho();
	refresh();
}

void initWindow(void) {
	game_win = newwin(HEIGHT,REAL_WIDTH,1,1);
	prompt_win = newwin(8,25,1,REAL_WIDTH + 4);
	WINDOW *game_border = newwin(HEIGHT + 2,REAL_WIDTH + 2,0,0);
	wborder(game_border,'|','|','-', '-', '+', '+', '+', '+');
	wborder(prompt_win,'|','|','-', '-', '+', '+', '+', '+');
	mvwprintw(prompt_win,1,10,"score: %d",player_score);
	wrefresh(game_border);
	wrefresh(prompt_win);
	wrefresh(game_win);
}

//checks if an enire row is filled up
void checkLines(void){
	int i,j,count,score_mod;
	score_mod = 0;
	for(i = HEIGHT-1; i > 0; i--) {
		count = 0;
		for (j =0; j < WIDTH; j++) {
			//if block is in spot, increase count
			if (mapbits[i][j] != 0 ) {
				count++;
			}
			//if blocks fill up entire row
			if (count == WIDTH) {
				removeLines(i);
				dropLines(i);
				score_mod *= 2;
				score_mod++;
				i++;  //after blocks dorp down, rechecks this row to see if it is filled up
			}
		}
	}
	player_score += 100 * score_mod;
	mvwprintw(prompt_win,1,10,"score: %d",player_score);
	wrefresh(prompt_win);
}

//if row is filled up, removes teh blocks
void removeLines(int const y_val){
	int i,j;
	for(i = 0; i < WIDTH; i++) {
		for(j = 0; j < 4; j++) {
			if(BLOCK_TO_REMOVE->y == y_val) {
				BLOCK_TO_REMOVE->destroyed = 1;

			}
		}
	}
}

//after row of blocks is removed, drop all blocks above it down one row
void dropLines(const int y_val) {
	int i,j;
	//sets all mapbits to 0 for the row which was removed adn all above it
	for(i = y_val; i > 0; i--) {
		for(j = 0; j < WIDTH; j++) {
			mapbits[i][j] = 0;
		}
	}
	//moves blocks down one row, sets mapbits to piece_number of that block
	for(i = 0; i < piece_number; i++) {
		for(j = 0; j < 4; j++) {
			if(BLOCK_I_J_INDEX->y < y_val) {
				BLOCK_I_J_INDEX->y++;
				mapbits[BLOCK_I_J_INDEX->y][(BLOCK_I_J_INDEX->x)/2] = i + 1;
			}
		}
	}
}


/* Returns an integer in the range [0, n).
 *
 * Uses rand(), and so is affected-by/affects the same seed.
 */
//found in an answer by Laurence Gonsalves on stack overflow
int randint(int n) {
  if ((n - 1) == RAND_MAX) {
    return rand();
  } else {

    // Supporting larger values for n would requires an even more
    // elaborate implementation that combines multiple calls to rand()
    assert (n <= RAND_MAX);

    // Chop off all of the values that would cause skew...
    int end = RAND_MAX / n; // truncate skew
    assert (end > 0);
    end *= n;

    // ... and ignore results from rand() that fall above that limit.
    // (Worst case the loop condition should succeed 50% of the time,
    // so we can expect to bail out of this loop pretty quickly.)
    int r;
    while ((r = rand()) >= end);

    return r % n;
  }
}

void getNextPiece(void) {
	current_piece = next_piece;
	next_piece = randint(7) + 1;
	showNextPiece(); 
	newPiece();
}

void showNextPiece(void) {
	mvwprintw(prompt_win,2,2,"next piece is: ");
	wattron(prompt_win,COLOR_PAIR(next_piece));
	switch(next_piece) {
		case 1: //line
			mvwprintw(prompt_win,3,16,"  []  ");
			mvwprintw(prompt_win,4,16,"  []  ");
			mvwprintw(prompt_win,5,16,"  []  ");
			mvwprintw(prompt_win,6,16,"  []  ");
			break;
		case 2: //square
			mvwprintw(prompt_win,3,16,"[][]  ");
			mvwprintw(prompt_win,4,16,"[][]  ");
			mvwprintw(prompt_win,5,16,"      ");
			mvwprintw(prompt_win,6,16,"      ");
			break;
		case 3: //z
			mvwprintw(prompt_win,3,16,"[][]  ");
			mvwprintw(prompt_win,4,16,"  [][]");
			mvwprintw(prompt_win,5,16,"      ");
			mvwprintw(prompt_win,6,16,"      ");
			break;
		case 4: //s
			mvwprintw(prompt_win,3,16,"  [][]");
			mvwprintw(prompt_win,4,16,"[][]  ");
			mvwprintw(prompt_win,5,16,"      ");
			mvwprintw(prompt_win,6,16,"      ");
			break;
		case 5: //L
			mvwprintw(prompt_win,3,16,"  []  ");
			mvwprintw(prompt_win,4,16,"  []  ");
			mvwprintw(prompt_win,5,16,"  [][]");
			mvwprintw(prompt_win,6,16,"      ");
			break;
		case 6: //J
			mvwprintw(prompt_win,3,16,"  []  ");
			mvwprintw(prompt_win,4,16,"  []  ");
			mvwprintw(prompt_win,5,16,"[][]  ");
			mvwprintw(prompt_win,6,16,"      ");
			break;
		case 7: //T
			mvwprintw(prompt_win,3,16,"[]    ");
			mvwprintw(prompt_win,4,16,"[][]  ");
			mvwprintw(prompt_win,5,16,"[]    ");
			mvwprintw(prompt_win,6,16,"      ");
			break;
		default:  //do nothing
			break;		
	}
	wattroff(prompt_win,COLOR_PAIR(next_piece));
	wrefresh(prompt_win);
}
