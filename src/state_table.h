/** File: state_table.h
  * @author Jeff Martin
  * @date 06/03/2016
  * This file contains the function prototypes for the file state_table.c
  */

#ifndef STATE_TABLE_H
#define STATE_TABLE_H

//Function Prototypes
unsigned char* make_state_table();
void write_state_table(unsigned char* state_table);
void read_state_table(unsigned char* state_table);
int fill_state_table(unsigned char* state_table);
char get_turn(unsigned char* state_table, int cube);
char* solve_cube(int cube, char* state_table);

void test_state_table();
#endif
