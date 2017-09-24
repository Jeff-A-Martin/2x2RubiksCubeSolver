/** File: state_table.c
  * @author Jeff Martin
  * @date 06/03/2016
  * This file contains the functions used to store the cube states and cube
  * moves in the file:
  *   state_table.bin
  *  
  * The Pocket cube has 3,674,160 possible states. 
  * https://en.wikipedia.org/wiki/Pocket_Cube#Permutations
  *
  * For each cube we need to store the state and the move used to get there.
  * The state is stored in a 4 byte integer and the move is stored in a 1 byte
  * character. This means 5 * 3,674,160, = 18,370,800 bytes are needed.
  *
  * The states will be stored in sorted order in an array
  */
  
//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~//
//~~~~~~~~~~~~~~~~~~~~ Include Files and Define Constants ~~~~~~~~~~~~~~~~~~~~//
//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~//

#include <stdio.h>
#include <stdlib.h>
#include "state_table.h"
#include "queue.h"
#include "cube.h"

#define NUMBER_OF_CUBES 3674160 //number of possible cubes
#define SIZE_OF_CUBE 5 //5 bytes
#define SOLVED_CUBE 0x5FD3097E
//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~//
//~~~~~~~~~~~~~~~~~~~~~~~ Helper Function Prototypes ~~~~~~~~~~~~~~~~~~~~~~~~~//
//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~//
int find_index(unsigned char* state_table, int cube);
void shift_data_up(unsigned char* state_table, int index);
int get_last_element(unsigned char* state_table);
int add_state(unsigned char* state_table, int cube, char turn);

//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~//
//~~~~~~~~~~~~~~~~~~~~~~~~~ State_table Functions ~~~~~~~~~~~~~~~~~~~~~~~~~~~~//
//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~//

/** This function creates a character array to store the states of a cube
  * chars are used because they are very flexible 
  * @return A pointer to the array of chars
  */
unsigned char* make_state_table(){
  unsigned char* state_table = (unsigned char*)  \
                               calloc(NUMBER_OF_CUBES, SIZE_OF_CUBE);
  return state_table;
} 

/** This function writes a table to the binary file state_table.bin
  * @param state_table The table to write to memory
  */
void write_state_table(unsigned char* state_table){
  FILE* file = fopen("state_table.bin","wb");
  if(file == NULL) 
    printf("An error occured while writing to state_table.bin.\n");
  else
    fwrite(state_table, SIZE_OF_CUBE, NUMBER_OF_CUBES, file);

  fclose(file);
}

/** This functions reads from the binary file state_table.bin and stores
  * The data into a character array.
  * @param state_table The table to be filed
  */
void read_state_table(unsigned char* state_table){
  FILE* file = fopen("state_table.bin","rb");
  if(file == NULL) 
    printf("An error occured while reading from state_table.bin.\n");
  else
    fread(state_table, SIZE_OF_CUBE, NUMBER_OF_CUBES, file);

  fclose(file);
}

/** Fills an empty state_table. 
  * It begins by performing all possible turns on the solved cube, and storing 
  * any newly discovered cube states in the state table and in a queue.
  * It then performs all possible turns on all the states in the queue 
  * and adds any newly discovered states to both the state_table and the queue.
  * If a state is rediscovered, it is not added to the queue, and its 'turn'
  * is not updated in the state_table (we want to keep the first discovered turn 
  * for each cube to ensure the shortest possible solution path.
  * @param state_table The state tablee to be filled
  * @return 1 The state_table was completely filled
  * @return 0 The state_table was not completely filled
  */
int fill_state_table(unsigned char* state_table){
  Queue* queue = createQueue(NUMBER_OF_CUBES);
  enqueue(queue, SOLVED_CUBE);
  int count = 0;
  
  //while there are still states to be discovered.
  while(peek(queue) != 0){
    int this_cube = dequeue(queue);
    int front_C = frontC(this_cube);
    int front_CC = frontCC(this_cube);
    int left_C = leftC(this_cube); 
    int left_CC = leftCC(this_cube);
    int top_C = topC(this_cube);
    int top_CC = topCC(this_cube);

    //add the states to the state_table and if they are new... add them to the 
    //queue
    if(add_state(state_table, front_C, 0x01)){
      enqueue(queue, front_C);
      count++;
      printf("%d\n", count);
    }
    if(add_state(state_table, front_CC, 0x02)){
      enqueue(queue, front_CC);
      count++;
      printf("%d\n", count);
    }
    if(add_state(state_table, left_C, 0x03)){
      enqueue(queue, left_C);
      count++;
      printf("%d\n", count);
    }
    if(add_state(state_table, left_CC, 0x04)){
      enqueue(queue, left_CC);
      count++;
      printf("%d\n", count);
    }
    if(add_state(state_table, top_C, 0x05)){
      enqueue(queue, top_C);
      count++;
      printf("%d\n", count);
    }
    if(add_state(state_table, top_CC, 0x06)){
      enqueue(queue, top_CC);
      count++;
      printf("%d\n", count);
    }
  }
}

/** This function comsumes a cube and the finished state table. It returns the 
  * turn used to get to that cube state.
  * @param state_table The finished state_table 
  * @param cube the Cube for which to find the turn.
  * @return a number between 1 and 6 representing the turn.
  *   1:FC  2:fCC  3:Lc  4:LCC  5:TC  6:TCC
  * @return -1 cube does not exist
  */
char get_turn(unsigned char* state_table, int cube){
  int min = 0;
  int max = NUMBER_OF_CUBES - 1;
  int middle;
  int this_cube;
  //Find the index in the table
  do{
    middle = min + ((max - min) / 2);

    unsigned char byte3 = state_table[middle * 5];
    unsigned char byte2 = state_table[(middle * 5) + 1];
    unsigned char byte1 = state_table[(middle * 5) + 2];
    unsigned char byte0 = state_table[(middle * 5) + 3];

    this_cube = (((int) byte3) << 24) | (((int) byte2) << 16) | 
                     (((int) byte1) << 8) | ((int) byte0);

    if(this_cube < cube){
      min = middle + 1;
    }else if (this_cube > cube){
      max = middle - 1;
    }else
      return state_table[(middle * 5) + 4];

  }while(max >= min);  

  return -1; //cube not found
}

/** This funciton solves a cube. It returns a character array representing the 
  * turns used to solve a cube. It returns null if an invalid cube is passed in
  * as a parameter.
  * @param cube The cube to be solved
  * @param state_table The state_table
  * @return A character array representing the turns needed to solve the cube. 
  *    1:FC  2:fCC  3:Lc  4:LCC  5:TC  6:TCC
  * @return NULL signal an error (invalid cube)
  */
char* solve_cube(int cube, char* state_table){
  char this_turn;
  char* turn_sequence = calloc(1,15); //all cubes can be solved in 14 moves or
                                      //or less. The last spot holds the 
                                      //terminator (0)
  int count = 0;
  do{
    this_turn = get_turn(state_table, cube);
    if(this_turn == -1){
      return NULL; //signal an invalid cube.
      break;
    }
    else{
      turn_sequence[count] = this_turn; //store the turn 

      //perform the turn 
      switch(this_turn){
      case 0:
        break;
      case 1:
	cube = frontCC(cube);
        break;
      case 2:
        cube = frontC(cube);
        break;
      case 3:
        cube = leftCC(cube);
        break;
      case 4:
        cube = leftC(cube);
        break;
      case 5:
        cube = topCC(cube);
        break;
      case 6:
        cube = topC(cube);
        break;
      }
      count++;
    }
  }while(this_turn != 0); //zero signal's final turn
  return turn_sequence;
}

//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~//
//~~~~~~~~~~~~~~~~~~~~~~~~~ Local Helper Functions ~~~~~~~~~~~~~~~~~~~~~~~~~~~//
//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~//

/** This function adds a cube and the turn used to get to that state
  * into the state table. If the cube already exists in the table it will not
  * be added. The cubes are added such that they are in ascending order. 
  * @param cube the cube to be added to the table 
  * @param turn the Turn used to reach the state
  * @return 0 When the cube was already in the table
  * @return 1 When the cube was successfully added
  */
int add_state(unsigned char* state_table, int cube, char turn){
  int cube_index; //indexed by cube_state (5 bytes)
  cube_index = find_index(state_table, cube);
  if (cube_index == -1) return 0; //the cube already existed in the table
  shift_data_up(state_table, cube_index);
  
  //break down the cube int into four chars
  unsigned char byte0 = (unsigned char) cube;
  unsigned char byte1 = (unsigned char) (cube >> 8);
  unsigned char byte2 = (unsigned char) (cube >> 16);
  unsigned char byte3 = (unsigned char) (cube >> 24);

  int insert_index = cube_index * 5; //indexed by bytes

  //insert data into state_table (integer stored in big endian format)
  //integer stored before char
  state_table[insert_index] = byte3;
  state_table[insert_index + 1] = byte2;
  state_table[insert_index + 2] = byte1;
  state_table[insert_index + 3] = byte0;
  state_table[insert_index + 4] = turn;
  return 1;
}

/** This function determines the index in which a cube shouuld be inserted
  * If the cube already exists in the table the function returns -1
  * @param state_table The table in-which to insert the cube
  * @param cube The cube to be inserted
  * @return The index that the cube should be inserted at
  * @return -1 if no insertion needs to take place.
  */
int find_index(unsigned char* state_table, int cube){
  //break cube up into bytes
  unsigned char byte0 = (unsigned char) cube;
  unsigned char byte1 = (unsigned char) (cube >> 8);
  unsigned char byte2 = (unsigned char) (cube >> 16);
  unsigned char byte3 = (unsigned char) (cube >> 24);

  int i;
  for(i = 0; i < NUMBER_OF_CUBES; i++){
    int byte_index = i * 5;
    
    if (state_table[byte_index] == 0x00 &&
        state_table[byte_index + 1] == 0x00 &&
        state_table[byte_index + 2] == 0x00 &&
        state_table[byte_index + 3] == 0x00){
      return i;//if the new cube is bigger than all previous cubes, the index
               //should be the 'empty space' to the right of the biggest element
    }
    if (state_table[byte_index] < byte3) continue;
    else if(state_table[byte_index] > byte3) return i;
    else { //bytes are equal
      if (state_table[byte_index + 1] < byte2) continue;
      else if(state_table[byte_index + 1] > byte2) return i;
      else { //bytes are equal
        if (state_table[byte_index + 2] < byte1) continue;
        else if(state_table[byte_index + 2] > byte1) return i;
        else { //bytes are equal
          if (state_table[byte_index + 3] < byte0) continue;
          else if(state_table[byte_index + 3] > byte0) return i;
          else { //bytes are equal
            return -1; // the cube is already in the table
          }
        }    
      }
    }
  }
}

/** This function shifts the contents of a state table by 5 bytes. It only 
  * shifts the data at or after the provided index.
  * @param state_table The table to be shifted
  * @param index The index at which to begin shifting 
  *  (where an int and turn are 1 element)
  */
void shift_data_up(unsigned char* state_table, int index){
  int i;
  for(i = NUMBER_OF_CUBES - 2; i >= index; i--){ //for all cube_states except
    int this_index = i * 5;                       //the last
    int j;
    for(j = 0; j < 5; j++){ //shift up 1 cube_state
      state_table[this_index+5] = state_table[this_index];
      this_index++;
    }
  }
}

/** This function returns the last element of the state_table as an int
  * @param state_table State table to read from
  * @return the integer representation of the last element in the state_table
  */
int get_last_element(unsigned char* state_table){
  unsigned char byte3 = state_table[((NUMBER_OF_CUBES - 1) * SIZE_OF_CUBE)];
  unsigned char byte2 = state_table[((NUMBER_OF_CUBES - 1) * SIZE_OF_CUBE) + 1];
  unsigned char byte1 = state_table[((NUMBER_OF_CUBES - 1) * SIZE_OF_CUBE) + 2];
  unsigned char byte0 = state_table[((NUMBER_OF_CUBES - 1) * SIZE_OF_CUBE) + 3];

  int last_element = (((int) byte3) << 24) | (((int) byte2) << 16) | 
                     (((int) byte1) << 8) | ((int) byte0);
  return last_element;
}

//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~//
//~~~~~~~~~~~~~~~~~~~~~~~~~~~~ TESTING FUNCTIONS ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~//
//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~//

/** This function solves every cube contained in the state_table.
  * it is used to prove that the state table was correctly built. 
  */
void test_state_table(){
  unsigned char* state_table = make_state_table();

  read_state_table(state_table);

  int i;
  for(i = 0; i < NUMBER_OF_CUBES; i++){
    unsigned char byte3 = state_table[i * 5];
    unsigned char byte2 = state_table[(i * 5) + 1];
    unsigned char byte1 = state_table[(i * 5) + 2];
    unsigned char byte0 = state_table[(i * 5) + 3];

    int this_cube = (((int) byte3) << 24) | (((int) byte2) << 16) | 
                     (((int) byte1) << 8) | ((int) byte0);

    if(solve_cube(this_cube, state_table) == NULL){
      printf("INVALID %d\n", this_cube);
    }
  }

  free(state_table);
}
































