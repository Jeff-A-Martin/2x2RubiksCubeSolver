/** File: solver.c
  * @author Jeff Martin
  * @date 05/24/16
  *
  * This file contains the main program for solving a 2x2 cube
  */

#include <stdio.h>
#include <stdlib.h>
#include "cube.h"
#include "state_table.h"  

#define BUFF_SIZE 24
#define c21_6 (85766121)
#define c21_5 (4084101)
#define c21_4 (194481)
#define c21_3 (9261)
#define c21_2 (441)
#define c21_1 (21)
#define c21_0 (1)

extern char state_table0356[21][3];
extern char state_table1247[21][3];

static char* state_table; //global variable (local to file)

void print_intro();
int fill_buffer(char* buffer);
int compress(char* buffer);
char which_piece(char* piece);
char contains(char character, char* piece);
char get_piece_state(char piece, char pos, char* piece_orientation);
char compare_pieces(char* piece1, char* piece2);

int main(){
  state_table = make_state_table();//initialize table
  read_state_table(state_table);//load table 

  print_intro(); //Provide instructions of how to format data entry
  //data will be a string of length 24

  char buffer[BUFF_SIZE];

  while(fill_buffer(buffer)){
    printf("Please re-enter the state of your cube.\n");
    printf("____ ____ ____ ____ ____ ____\n");
  }; //loop until the user enters valid data

  int cube = compress(buffer);
  printf("The cube you entered is:\n\n");
  print_cube(decompress(cube));

  char* turn_sequence = solve_cube(cube, state_table);
  
  if(turn_sequence == NULL){
    printf("Something went wrong\n");  
    return 1; //signal failure
  }
  
  int i = 0;
  while(turn_sequence[i] != 0){
    switch(turn_sequence[i]){
      case 1:
        printf("FCC\n");
        break;
      case 2:
        printf("FC\n");
        break;
      case 3:
        printf("LCC\n");
        break;
      case 4:
        printf("LC\n");
        break;
      case 5:
        printf("TCC\n");
        break;
      case 6:
        printf("TC\n");
        break;

    }
  i++;
  }

}

/** This function prints an introductory screen with instructions of how
  * to enter the state of the cube.
  */
void print_intro(){
  printf("Welcome to the 2x2 Rubik's cube solver!\n");
  printf("Author: Jeff Martin\n");
  printf("  Worcester Polytechnic Institute\n");
  printf("  Computer Science 2019\n");
  printf("~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~\n");
  printf("To enter the state of your cube, please do the following:\n");
  printf("  1) Orient your cube such that the red-yellow-blue corner\n");
  printf("     is in the bottom, back, right corner. (respectivly)\n");
  printf("  2) Enter your colors using the following: \n");
  printf("       o - orange\n");
  printf("       r - red\n");
  printf("       w - white\n");
  printf("       y - yellow\n");
  printf("       g - green\n");
  printf("       b - blue\n");
  printf("     Such that they are entered in numeric order based on\n");
  printf("     the order provided below.\n\n");
  
  printf("             |00|01|\n");
  printf("             |02|03|\n\n");

  printf("     |04|05| |08|09| |12|13| |16|17|\n");
  printf("     |06|07| |10|11| |14|15| |18|19|\n\n");

  printf("             |20|21|\n");
  printf("             |22|23|\n\n");

  printf("  EXAMPLE: solved cube = \"oooo gggg wwww bbbb yyyy rrrr\"\n");
  printf("  Note: \n");
  printf("    - Including spaces is optional\n");
  printf("    - The following will always be true:\n");
  printf("          15 = b  18 = y  23 = r\n\n");

  printf("____ ____ ____ ____ ____ ____\n");
}

/** This function fills the buffer with the user's input 
  * (an array of 24 characters)
  * @param buffer The pointer to the buffer to be filled
  * @return 0 The data represents a valid cube
  * @return 1 The data represents an invalid cube
  */
int fill_buffer(char* buffer){

  char next_char = getchar();
  char this_char;
  int count = 0;
  int valid_entry = 1;
  do{
    this_char = next_char;
    if(count < BUFF_SIZE){ //protect from buffer overflow
      switch(this_char){
        case ' ': //ignore spaces; immeditatly break
          break;
        case 'o':
        case 'r':
        case 'w':
        case 'y':
        case 'g':
        case 'b':
          buffer[count] = this_char;
          count++;
          break;
        default:
          printf("'%c' is not a valid character\n", this_char);
          //empty input buffer
          while (getchar() != '\n'){};
          return 1;
      }
    }else{ //count >= 24
      printf("You entered two many colors!\n");
      //empty input buffer
      while (getchar() != '\n'){};
      return 1;
    }

    next_char = getchar(); //get next character

    if(next_char == '\n' && count < BUFF_SIZE){ //they entered too few colors
      printf("You did not completely enter the state of the cube!\n");
      return 1;
    }
  }while(next_char != '\n');

  //If it gets here the buffer contains exactly 24 characters, each representing
  //a real color
  
  //count colors;
  char color_count[6] = {0,0,0,0,0,0};
  for(count = 0; count < BUFF_SIZE; count++){
    switch(buffer[count]){
      case 'o':
        color_count[0]++;
        break;
      case 'r':
        color_count[1]++;
        break;
      case 'y':
        color_count[2]++;
        break;
      case 'w':
        color_count[3]++;
        break;
      case 'g':
        color_count[4]++;
        break;
      case 'b':
        color_count[5]++;
        break;
    }
  }

  //check color count
  for(count = 0; count < 6; count++){
    if(color_count[count] != 4){
      printf("You do not have the right amount of each color.\n");
      return 1;
    }
  }

  //check proper orientation (red-yellow-blue in bottom-back-right)
  if(buffer[15] != 'b' || buffer[18] != 'y' || buffer[23] != 'r'){
    printf("Your cube is not properly oriented.\n");
    printf("Remember to put the red-yellow-blue corner in the \
            bottom-back-right.\n");
    return 1;
  }
  //the cube has all the valid colors, check if it is a real state.
  int cube = compress(buffer);
  if(cube == -1){
    printf("The Cube you entered is not in a possible state.\n");
    return 1;
  }
  if(cube == -2){
    printf("A very bad error occured..."); //hopefully it never gets here
  }
  return 0;

}

/** This function determines if the cube is in a valid state
  * @param buffer The buffer containing the cube colors
  * @param return The integer interpretation of the cube
  * @param return -1 if the cube is not valid
  * @param return -2 if the cube appears valid but does not exist in the table
  */
int compress (char* buffer){
  char positions[7][3] = {
    {buffer[2], buffer[8], buffer[5]}, //Pos0
    {buffer[20], buffer[10], buffer[7]}, //Pos1
    {buffer[3], buffer[9], buffer[12]}, //Pos2
    {buffer[21], buffer[11], buffer[14]}, //Pos3
    {buffer[0], buffer[17], buffer[4]}, //Pos4
    {buffer[22], buffer[19], buffer[6]}, //Pos5
    {buffer[1], buffer[16], buffer[13]} //Pos6
  };

  char piece_to_pos_map[7]; //array to hold which pieces are in which position
                            //EX: first element = 3 means piece0 is in pos3

  //determine which piece is in which position
  int i;
  for(i = 0; i < 7; i++){
    char piece = which_piece(positions[i]);
    switch(piece){
      case -1: //invalid piece
        return -1; //indicate invalid cube state
      default:
        piece_to_pos_map[piece] = i;
    }

  }
  //determine orientation of each cube

  char states[7] = {
    get_piece_state(0, piece_to_pos_map[0], positions[piece_to_pos_map[0]]),
    get_piece_state(1, piece_to_pos_map[1], positions[piece_to_pos_map[1]]),
    get_piece_state(2, piece_to_pos_map[2], positions[piece_to_pos_map[2]]),
    get_piece_state(3, piece_to_pos_map[3], positions[piece_to_pos_map[3]]),
    get_piece_state(4, piece_to_pos_map[4], positions[piece_to_pos_map[4]]),
    get_piece_state(5, piece_to_pos_map[5], positions[piece_to_pos_map[5]]),
    get_piece_state(6, piece_to_pos_map[6], positions[piece_to_pos_map[6]])
  };

  //check for invalid states
  for(i = 0; i < 7; i++){
    if(states[i] == -1) return -1; //invalid state
  }
  
  int cube = states[0] + (c21_1 * states[1]) + (c21_2 * states[2]) + \
    (c21_3 * states[3]) + (c21_4 * states[4]) + (c21_5 * states[5]) + \
    (c21_6 * states[6]);

  //At this point the cube should be valid.
  //make sure that it is in the table. 
  
  if(get_turn(state_table, cube) == -1) return -2;
  
  return cube;
  
}


/** This function determines which piece it has based on the colors
  * @param piece Character array containing 3 colors
  * @return The number of the piece
  * @return -1 If the piece is invalid
  */
char which_piece(char* piece){
  if(contains('o', piece) && contains('w', piece) && contains('g', piece))
    return 0;
  else if(contains('r', piece) && contains('w', piece) && contains('g', piece))
    return 1;
  else if(contains('o', piece) && contains('w', piece) && contains('b', piece))
    return 2;
  else if(contains('r', piece) && contains('w', piece) && contains('b', piece))
    return 3;
  else if(contains('o', piece) && contains('y', piece) && contains('g', piece))
    return 4;
  else if(contains('r', piece) && contains('y', piece) && contains('g', piece))
    return 5;
  else if(contains('o', piece) && contains('y', piece) && contains('b', piece))
    return 6;
  else 
    return -1;
}

/** This function determins if a character is in a character array of length 3
  * @param character The char to be found
  * @param piece the 3 char character array
  * @return 1 if the char is in the array 
  * @return 0 if the char is not in the array
  */
char contains(char character, char* piece){
  int i;
  for(i = 0; i < 3; i++){
    if(piece[i] == character) return 1;
  }
  return 0;
}

/** This function determines the state of a piece (position and orientation)
  * @param piece The piece number (0-6)
  * @param pos The position number (0-6)
  * @param piece_orientation Orientation of colors
  * @return The state of the piece (0-21)
  * @return -1 the piece is not in a valid state
  */
char get_piece_state(char piece, char pos, char* piece_orientation){
  //see cube.c for an explanation of interpretting orientation
  char number_orientation[3];
  int i;
  for(i = 0; i < 3; i++){
    switch(piece_orientation[i]){
      case 'r':
      case 'o':
        number_orientation[i] = 0;
        break;
      case 'y':
      case 'w': 
        number_orientation[i] = 1;
        break;
      case 'g':
      case 'b':
        number_orientation[i] = 2;
        break;
    }
  }
  
  char state;
  char offset = pos * 3; 
  if (piece == 0 || piece == 3 || piece == 5 || piece == 6){
    if (compare_pieces(number_orientation, state_table0356[offset]))
      state = offset;
    else if (compare_pieces(number_orientation, state_table0356[offset + 1]))
      state = offset + 1;
    else if (compare_pieces(number_orientation, state_table0356[offset + 2]))
      state = offset + 2;
    else
      state = -1; //invalid
  }else{//pieces 1,2,4,7
    if (compare_pieces(number_orientation, state_table1247[offset]))
      state = offset;
    else if (compare_pieces(number_orientation, state_table1247[offset + 1]))
      state = offset + 1;
    else if (compare_pieces(number_orientation, state_table1247[offset + 2]))
      state = offset + 2;
    else
      state = -1; //invalid
  }

  return state;
}

/** This function determines if two pieces are the same
  * @param piece1 
  * @param piece2
  * @return 1 if the pieces are the same
  * @return 0 if the pieces are not the same
  */
char compare_pieces(char* piece1, char* piece2){
  int i;
  for(i = 0; i < 3; i++){
    if(piece1[i] != piece2[i]) return 0;
  }
  return 1;
}
















