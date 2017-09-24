/** File: cube.c
  * @author Jeff Martin
  * @date 05/23/2016
  *
  * This file contains the functions that implement the 2x2 rubiks cube.
  * A cube will be stored within a 32 bit int.
  * Implementation based on that of Antti Valmari
  * http://www.cs.tut.fi/~ava/rub-self.pdf
  */

/** We must label both the possible positions on the cube,
  * and the individual pieces. Below, they are labeled in such a way that 
  * for a solved cube, the piece labels and positon labels match.
  *
  * Labeling the pieces:
  * 0: orange_white_green corner
  * 1: red_white_green corner
  * 2: orange_white_blue corner
  * 3: red_white_blue corner
  * 4: orange_yellow_green corner
  * 5: red_yellow_green corner
  * 6: orange_yellow_blue corner
  * 7: red_yellow_blue corner *FIXED POSITION* (bottom_back_right)
  * 
  * Labeling the Positions:
  * 0: top_front_left corner
  * 1: bottom_front_left corner
  * 2: top_front_right corner
  * 3: bottom_front_right corner
  * 4: top_back_left corner
  * 5: bottom_back_left corner
  * 6: top_back_right corner
  * 7: bottom_back_right corner *FIXED PIECE* (red_yellow_blue) 
  *
  * Labeling the Piece Orientation:
  * Each corner piece has 3 colors, and the colors are placed on
  * on the three faces top/bottom, front/back, and left/right.
  * Notice that all pieces above are labeled so that the first color is 
  * on either the top or bottom, the second is on the front or back, and the 
  * third is on the left or right.(color1_color2_color3)
  * Thus the sequence of colors represents the orientation of the piece. 
  * However, there are 6 possible sequences of colors (3!) while there are
  * only 3 actual orientations of a piece.
  * This is because only 3 of the 6 possible permutations are allowed at any
  * given position. These three permutaitons vary from position to position. 
  * Thus we need a way to map from the 3 orientations at a position into 
  * the 6 possible permutations of the color sequence. 
  * In order to write such a map we should consider that their are two types of
  * corner pieces. Positions 0, 3, 5, and 6 are grouped together, and 
  * Pieces 1, 2, 4, and 7 are grouped together. 
  * Pieces/Positions in the same group are 'an even number of moves apart'
  * while Pieces/Positions in different groups are 'an odd number of moves 
  * apart'. Pieces in the same group share the following Coloring property:
  *   green and blue can be interchanged to create a valid piece.
  *   yellow and white can be interchanged to create a valid piece.
  *   red and orange can be interchange to create a valid piece.
  * This property is not shared between the seperate groups. 
  * Because of this property we can use 1 function to map between pieces of 
  * of the same group. Therefore we only need 2 mapping functions (or tables)
  * to map between the 3 orientations of any piece and the 6 possible 
  * permutations of three colors. 
  */

/** How the cube is built into an integer:
  * because piece 7 is fixed to postion 7, we need not account for either of 
  * them in our data storage. 
  *
  * Each of the 7 moveable pieces (0-6) can have 3 (0-2) orientations 
  * which totals 21 (0-20) possible states. Let X represent the state of piece p
  * p's position = X / 3 (rounding down)
  * p's orientation = X % 3 
  * 
  * the cube can be represented as the summation of (for pieces 0-6)
  * piece_state * 21^piece_number 
  *
  * Example: Encoding the Solved Cube: 
  *   (orientations based on orientation tables below)
  *   Piece 0: Position --> 0 Orientation --> 0 ==> State: 0
  *   Piece 1: Position --> 1 Orientation --> 2 ==> State: 5
  *   Piece 2: Position --> 2 Orientation --> 0 ==> State: 6
  *   Piece 3: Position --> 3 Orientation --> 0 ==> State: 9
  *   Piece 4: Position --> 4 Orientation --> 1 ==> State: 13
  *   Piece 5: Position --> 5 Orientation --> 0 ==> State: 15
  *   Piece 6: Position --> 6 Orientation --> 0 ==> State: 18
  *  
  * Cube = 0 + 5*21 + 6*21^2 + 9*21^3 + 13*21^4 + 15*21^5 + 18*21^6
  *      = 105 + 2646 + 83349 + 2528253 + 61261515 + 1543790178
  *      = 1,607,666,046 = 0x5FD3097E
  */
//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~//
//~~~~~~~~~~~~~~~~~~~~ Include Files and Define Constants ~~~~~~~~~~~~~~~~~~~~//
//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~//

#include "cube.h"
#include <stdio.h>
#include <stdlib.h>

//Powers of 21
#define c21_6 (85766121)
#define c21_5 (4084101)
#define c21_4 (194481)
#define c21_3 (9261)
#define c21_2 (441)
#define c21_1 (21)
#define c21_0 (1)

//define the colors of each piece
static char zero[3] = {'o','w','g'};
static char one[3] = {'r','w','g'};
static char two[3] = {'o','w','b'};
static char three[3] = {'r','w','b'};
static char four[3] = {'o','y','g'};
static char five[3] = {'r','y','g'};
static char six[3] = {'o','y','b'};

/** The following table is based on Annitti Valmari's paper (see table 1)
  * the rows represent the different types of turns 
  * Each the columns are indexed by the state
  * EX: turn_table[3][4] holds the state that results from performing a left
  *     clockwise turn on a piece in the fifth state
  */
static int turn_table[6][21] = {
  {4,5,3,11,9,10,1,2,0,8,6,7,12,13,14,15,16,17,18,19,20}, //front CC
  {8,6,7,2,0,1,10,11,9,4,5,3,12,13,14,15,16,17,18,19,20}, //Front C
  {13,14,12,1,2,0,6,7,8,9,10,11,17,15,16,5,3,4,18,19,20}, //Left CC
  {5,3,4,16,17,15,6,7,8,9,10,11,2,0,1,13,14,12,18,19,20}, //left C
  {7,8,6,3,4,5,20,18,19,9,10,11,1,2,0,15,16,17,14,12,13}, //Top CC
  {14,12,13,3,4,5,2,0,1,9,10,11,19,20,18,15,16,17,7,8,6}}; //Top C

/** The following tables provide the map between orientation and three color
  * sequence. Let 0 represent either o or r, 1 represent either y or w, and 
  * 2 represent either g or b. The ordering 0_1_2 means the top/bottom is
  * red/orange, the front/back is white/yellow, and the left/right is 
  * green/blue. Color depends on the piece being used, and the face depends on 
  * the position of the piece. The following tables have 21 rows (indexed by
  * piece state) and each row has 3 chars that represent the three colors
  * (using 0, 1, 2).
  */

/** table for pieces 0, 3, 5, and 6
  * based on piece 0 having orientation 0 when 'solved'
  */
char state_table0356[21][3] = {
  {0,1,2}, {2,0,1}, {1,2,0},
  {0,2,1}, {2,1,0}, {1,0,2},
  {1,0,2}, {0,2,1}, {2,1,0},
  {0,1,2}, {2,0,1}, {1,2,0},
  {2,1,0}, {1,0,2}, {0,2,1},
  {0,1,2}, {2,0,1}, {1,2,0}, 
  {0,1,2}, {2,0,1}, {1,2,0}};

/** table for pieces 1, 2, 4, and '7'
  * based on piece 2 having orientation 0 when 'solved'
  */
char state_table1247[21][3] = {
  {1,0,2}, {2,1,0}, {0,2,1},
  {1,2,0}, {2,0,1}, {0,1,2},
  {0,1,2}, {1,2,0}, {2,0,1},
  {1,0,2}, {2,1,0}, {0,2,1},
  {2,1,0}, {0,1,2}, {1,0,2},
  {1,0,2}, {2,1,0}, {0,2,1},
  {2,1,0}, {1,0,2}, {0,2,1}};


//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~//
//~~~~~~~~~~~~~~~~~~~~~~~ Helper Function Prototypes ~~~~~~~~~~~~~~~~~~~~~~~~~//
//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~//
 
void insert(Cube* cube_S, char* piece_colors, int piece, int state);
Cube* make_cube();
int move_piece (int curr_state, int turn);
int rotate(int cube, int turn);

//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~//
//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~ Cube Functions ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~//
//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~//

/** This function performs a front counter-clockwise turn on a cube
  * @param cube The cube to be turned
  * @return The turned cube.
  */
int frontCC(int cube){
  return rotate(cube, 0);
}

/** This function performs a front clockwise turn on a cube
  * @param cube The cube to be turned
  * @return The turned cube.
  */
int frontC(int cube){
  return rotate(cube, 1);
}

/** This function performs a left counter-clockwise turn on a cube
  * @param cube The cube to be turned
  * @return The turned cube.
  */
int leftCC(int cube){
  return rotate(cube, 2);
}

/** This function performs a left clockwise turn on a cube
  * @param cube The cube to be turned
  * @return The turned cube.
  */
int leftC(int cube){
  return rotate(cube, 3);
}

/** This function performs a top counter-clockwise turn on a cube
  * @param cube The cube to be turned
  * @return The turned cube.
  */
int topCC(int cube){
  return rotate(cube, 4);
}

/** This function performs a top clockwise turn on a cube
  * @param cube The cube to be turned
  * @return The turned cube.
  */
int topC(int cube){
  return rotate(cube, 5);
}

/** This function consumes an integer representation of a cube and converts it
  * to the Cube struct.
  * @param cube The integer representation of the cube
  * @return A pointer to the Cube struct
  */
Cube* decompress(int cube){

  //determine the states of each piece
  int state6 = cube / c21_6;
  cube %= c21_6;
  int state5 = cube / c21_5;
  cube %= c21_5;
  int state4 = cube / c21_4;
  cube %= c21_4;
  int state3 = cube / c21_3;
  cube %= c21_3;
  int state2 = cube / c21_2;
  cube %= c21_2;
  int state1 = cube / c21_1;
  int state0 = cube % c21_1;

  Cube* my_cube = make_cube();


  //place the pieces in the cube struct
  insert(my_cube, six, 6, state6);
  insert(my_cube, five, 5, state5);
  insert(my_cube, four, 4, state4);
  insert(my_cube, three, 3, state3);
  insert(my_cube, two, 2, state2);
  insert(my_cube, one, 1, state1);
  insert(my_cube, zero, 0, state0);

  return my_cube;
}

/** This function displays a Cube struct to the screen
  * @param cube Pointer to the cube struct
  */
void print_cube(Cube* cube){
  printf("      |%c|%c|\n", cube->cube[0][2], cube->cube[0][3]);
  printf("      |%c|%c|\n\n", cube->cube[1][2], cube->cube[1][3]);
  printf("|%c|%c| |%c|%c| |%c|%c| |%c|%c|\n", cube->cube[2][0], 
  cube->cube[2][1], cube->cube[2][2], cube->cube[2][3], cube->cube[2][4], 
  cube->cube[2][5], cube->cube[2][6], cube->cube[2][7]);
  printf("|%c|%c| |%c|%c| |%c|%c| |%c|%c|\n\n", cube->cube[3][0], 
  cube->cube[3][1], cube->cube[3][2], cube->cube[3][3], cube->cube[3][4], 
  cube->cube[3][5], cube->cube[3][6], cube->cube[3][7]);
  printf("      |%c|%c|\n", cube->cube[4][2], cube->cube[4][3]);
  printf("      |%c|%c|\n\n", cube->cube[5][2], cube->cube[5][3]);

}


//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~//
//~~~~~~~~~~~~~~~~~~~~~~~~~ Local Helper Functions ~~~~~~~~~~~~~~~~~~~~~~~~~~~//
//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~//

/** This function inserts a piece into a cube data structure
  * @param cube Pointer to the cube struct
  * @param piece character array representing the piece's colors
  * @param state state of the piece
  */
void insert(Cube* cube_S, char* piece_colors, int piece, int state){
  char colorTB, colorFB, colorRL; //top-bottom, front-back, right-left
  if (piece == 0 || piece == 3 || piece == 5 || piece == 6){
    colorTB = state_table0356[state][0];
    colorFB = state_table0356[state][1];
    colorRL = state_table0356[state][2];
  }else{
    colorTB = state_table1247[state][0];
    colorFB = state_table1247[state][1];
    colorRL = state_table1247[state][2];
  }

  colorTB = piece_colors[colorTB];//retrieve the character at the specific index
  colorFB = piece_colors[colorFB];
  colorRL = piece_colors[colorRL];

  int pos = state / 3;

  switch(pos){
    //The cube indexes are arbitrary to my struct definition of a cube
    case 6: //top_back_right
      cube_S->cube[0][3] = colorTB;
      cube_S->cube[2][6] = colorFB;
      cube_S->cube[2][5] = colorRL;
      break;
    case 5: //bottom_back_left
      cube_S->cube[5][2] = colorTB;
      cube_S->cube[3][7] = colorFB;
      cube_S->cube[3][0] = colorRL;
      break;
    case 4: //top_back_left
      cube_S->cube[0][2] = colorTB;
      cube_S->cube[2][7] = colorFB;
      cube_S->cube[2][0] = colorRL;
      break;
    case 3: //bottom_front_right
      cube_S->cube[4][3] = colorTB;
      cube_S->cube[3][3] = colorFB;
      cube_S->cube[3][4] = colorRL;
      break;
    case 2: //top_front_right
      cube_S->cube[1][3] = colorTB;
      cube_S->cube[2][3] = colorFB;
      cube_S->cube[2][4] = colorRL;
      break;
    case 1: //bottom_front_left
      cube_S->cube[4][2] = colorTB;
      cube_S->cube[3][2] = colorFB;
      cube_S->cube[3][1] = colorRL;
      break;
    case 0: //top_front_left
      cube_S->cube[1][2] = colorTB;
      cube_S->cube[2][2] = colorFB;
      cube_S->cube[2][1] = colorRL;
      break;
  }
}

/** This function allocates memory for a cube an intializes all the data.
  * @return Pointer to the cube struct.
  */
Cube* make_cube(){
  Cube* cube = malloc(sizeof(Cube));
  int i,j;
  for(i = 0; i < 6; i++){
    for(j = 0; j < 8; j++){
      cube->cube[i][j] = '-'; //initialize with spaces
    }
  }
  //add stationary piece
  cube->cube[5][3] = 'r';
  cube->cube[3][5] = 'b';
  cube->cube[3][6] = 'y';
  return cube;
}

/** This function performs a rotation on a cube. 
  * @param cube The cube to be turned
  * @param turn The type of turn (ex front counter clockwise)
  * @return the turned cube.
  */
int rotate(int cube, int turn){
  //obtain original states
  int state6 = cube / c21_6;
  int state5 = (cube % c21_6) / c21_5;
  int state4 = (cube % c21_5) / c21_4;
  int state3 = (cube % c21_4) / c21_3;
  int state2 = (cube % c21_3) / c21_2;
  int state1 = (cube % c21_2) / c21_1;
  int state0 = (cube % c21_1);

  //convert states
  state6 = turn_table[turn][state6];
  state5 = turn_table[turn][state5];
  state4 = turn_table[turn][state4];
  state3 = turn_table[turn][state3];
  state2 = turn_table[turn][state2];
  state1 = turn_table[turn][state1];
  state0 = turn_table[turn][state0];

  //rebuild cube and return
  return state0 + (state1 * c21_1) + (state2 * c21_2) + (state3 * c21_3) +
         (state4 * c21_4) + (state5 * c21_5) + (state6 * c21_6);
}


//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~//
//~~~~~~~~~~~~~~~~~~~~~~~~~~~~ TESTING FUNCTIONS ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~//
//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~//


/** This function starts with the solved cube, and performs all possible turns 
  * on it, then prints out the corrusponding cubes. This is used to test that
  * the rotation, compression, and decompression funcitons work.
  */
void test_cube(){
  int solved_cube = 0x5FD3097E;
  
  Cube* cube = decompress(solved_cube);
  printf("SOLVED CUBE:\n");
  print_cube(cube);
  free(cube);
  
  cube = decompress(topC(solved_cube));
  printf("Top Clockwise:\n");
  print_cube(cube);
  free(cube);

  cube = decompress(topCC(solved_cube));
  printf("Top Counter-Clockwise:\n");
  print_cube(cube);
  free(cube);

  cube = decompress(frontC(solved_cube));
  printf("Front Clockwise:\n");
  print_cube(cube);
  free(cube);

  cube = decompress(frontCC(solved_cube));
  printf("Front Counter-Clockwise:\n");
  print_cube(cube);
  free(cube);

  cube = decompress(leftC(solved_cube));
  printf("Left Clockwise:\n");
  print_cube(cube);
  free(cube);

  cube = decompress(leftCC(solved_cube));
  printf("left Counter-Clockwise:\n");
  print_cube(cube);
  free(cube);
}





















