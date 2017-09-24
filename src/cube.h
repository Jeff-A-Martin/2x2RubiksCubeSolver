/** File: cube.h
  * @author Jeff Martin
  * @date: 5/24/2016
  *
  * This file contains the function prototypes for the functions contained in cube.c
  */

#ifndef CUBE_H
#define CUBE_H

  /** The following is a simple structure to represent a cube. 
    * When a cube needs to be stored or operated on efficiently, 
    * the integer implementation will be used, 
    * when efficiency is not crucial, this structure is provided for conviennce.
    * Example: Representation of the solved cube:
    *
    *      o o
    *      o o
    *  g g w w b b y y 
    *  g g w w b b y y 
    *      r r
    *      r r
    */
  typedef struct Cube {
    //6 rows and 8 columns 
    char cube[6][8]; 
  } Cube;


  //Function Prototypes
  int frontCC(int cube);
  int frontC(int cube);
  int leftCC(int cube);
  int leftC(int cube);
  int topCC(int cube);
  int topC(int cube);

  Cube* decompress(int cube);
  void print_cube(Cube* cube);

#endif
