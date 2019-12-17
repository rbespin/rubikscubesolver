/* Name: Robert Espinoza
 * Email: rbespinoza96@gmail.com
 * Description: The purpose of this program is to solve a scrambled Rubik's
 * cube utilizing Thistlethwaite's algorithm. This is the fifth attempt at
 * optimizing the cubes performance to perform with speed.
 * ---Special thanks to Stephen Pochmann for the guidance---
 * ---Thistlethwaite for his algorithm---
 * ---Jaapsch for his website---
 */

#include <utility>
#include <string>
#include <vector> //vi, vc
#include <iostream> // cout 
#include <stdio.h>  // random
#include <stdlib.h> // random
#include <time.h> // random
#include <queue>
#include <map>
#include <list>
#include <ctime>
#include <fstream>


using namespace std;

//--- Cube will be predefined as a 40 element vector 
//--- <12 edge locations, 8 corner locations, 12 corner orien., 8 edge orient.>
//--- A solved cube will have all cubies in their proper locations with 0
//orientation values, i.e.
//---
//({0,1,2,3,4,5,6,7,...,19},{0,0,0,0,0,0,0,0,0,0,0,0},{0,0,0,0,0,0,0,0}

typedef vector<int> vi;
int phase = 0; // class variable for current phase of algorithm
int totalMoves = 0; // class variable for tracking iterations of our BFS

//--- Edge identifiers
int UF = 0;
int UR = 1;
int UB = 2;
int UL = 3;
int FR = 4;
int FL = 5;
int BR = 6;
int BL = 7;
int DF = 8;
int DR = 9;
int DB = 10;
int DL = 11;

//--- Corner identifiers
int UFR = 12;
int UBR = 13;
int UBL = 14;
int UFL = 15;
int DFR = 16;
int DBR = 17;
int DBL = 18;
int DFL = 19;

//--- Gives the relevant info at the current phase the cube is currently in
//--- These are the only relevant information pieces for that particular
//phase. Elements in the return vector will 'track' the pieces for the breadth
//first search.
vi id(vi state){

   //--- Phase 1
   // fix edge orientations
   if( phase == 1 ){
	  vi id;
	  for(int i = 20; i < 32; i++ ){
		 id.push_back(state[i]);
	  }
	  return id;
   }

   //--- Phase 2
   // fix corner orientations, M-Slice
   if( phase == 2 ){
	  vi id;
	  // corner orientations to be zero
	  for( int i = 32; i < 40; i++){
		 id.push_back(state[i]);
	  }
	  // Ensuring corners have L/R stickers facing L/R faces
	  // This is how Thistlethwaite defines an oriented corner.
	  // Corners 1,3,4,6 will naturally fall into place if corners 0,2,5,7 are
	  // put into their proper groups
	  for( int i = 12; i < 20; i++ ){
		 if( state[i] == UFR ||
			   state[i] == UBL ||
			   state[i] == DBR ||
			   state[i] == DFL ){
			id.push_back(i);
		 }
	  }

	  // M-Slice (slice between L/R face)
	  // push back indices of 0,2,8,10 to track 
	  for( int i = 0; i < 12; i++ ){
		 if( (state[i] == 0) || 
			   (state[i] == 2) || 
			   (state[i] == 8) || 
			   (state[i] == 10) ){
			id.push_back(i);
		 }
	  }
	  return id;
   }

   //--- Phase 3
   //E-Slice, S-Slice, corner tetrads
   if( phase == 3 ){
	  vi id;

	  //----------------------------------------------

	  // E-slice: 4,5,6,7
	  for( int i = 0; i < 12; i++ ){
		 if( (state[i] == 4) || 
			   (state[i] == 5) || 
			   (state[i] == 6) || 
			   (state[i] == 7) ){
			id.push_back(i);
		 }
	  }
	  // S-Slice cubies will naturally fall into place with E-slice cubies put
	  // into place

	  //-----------------------------------------------------------

	  // Corner orbits:
	  // UFR and UBL can only be in position 0, 2, 5, 7 
	  // UBR and UFL can only be in position 1, 3, 4, 6
	  // DFR and DBL can only be in position 1, 3, 4, 6
	  // DBR and DFL can only be in position 0, 2, 5, 7

	  //------ Placing corners into proper tetrads
	  // Making sure 0,2,5,7 are in 0.2.5.7
	  for( int i = 12; i < 20; i++ ){
		 if( state[i] == UFR ||
			   state[i] == UBL ||
			   state[i] == DBR ||
			   state[i] == DFL ){
			id.push_back(i);
		 }
	  }

	  // Ensuring that UFR and UBL, DBR and DFL are paired
	  // Track the pairs 0,2; 5,7
	  for( int i = 12; i < 20; i++ ){
		 if( state[i] == UFR ||
			   state[i] == UBL ){
			id.push_back(i);
		 }
	  }
	  for( int i = 12; i < 20; i++ ){
		 if( state[i] == DBR ||
			   state[i] == DFL ){
			id.push_back(i);
		 }
	  }

	  for( int i = 12; i < 20; i++ ){
		 // Making sure 1,3,4,6 are in 1,3,4,6
		 if( state[i] == UBR ||
			   state[i] == UFL ||
			   state[i] == DFR ||
			   state[i] == DBL ){
			id.push_back(i);
		 }
	  } 
	  // Track the pairs 1,3; 4,6 and make sure they're paired
	  for( int i = 12; i < 20; i++ ){
		 if( state[i] == UBR ||
			   state[i] == UFL ){
			id.push_back(i);
		 }
	  }
	  for( int i = 12; i < 20; i++ ){
		 if( state[i] == DFR ||
			   state[i] == DBL ){
			id.push_back(i);
		 }
	  }  

	  //--- Ensuring even parity with tetrad pairs. 
	  int tetrad = 0;
	  for( int i=12; i<20; i++ ){
		 for( int j=i+1; j<20; j++ ){
			// If state[i] > state[j], then state[i] is in an incorrect cubie
			// location. If there are an odd number of these, then the cube is
			// unsolvable with g4 group moves. Therefore, we must ensure even
			// parity for corner tetrad pair swaps.
			if( state[i] > state[j]){
			   tetrad++;
			}
		 }
	  }
	  if( (tetrad%2) == 0 ){
		 id.push_back(tetrad);
	  }

	  //-----------------------------------------------------------
	  return id;
   }

   //--- Phase 4
   // Fix remaining cubes with proper group moves
   if( phase == 4 ){
	  return state;
   }

   return state;
}


//--- Update an input state after applying a move
//--- Input: int move - a number between 0-17, signifying one of the eighteen
//possible cube operations (R, R2, R3, L, L2, L3, ... etc).
// vc state - The state of the current cube.
//--- Output: vi - The state of the cube after applying one of eighteen moves
//to the cube.
vi applyMove(int move, vi state){

   // 0,6,12 - R[0]
   // 1,7,13 - L[1]
   // 2,8,14 - F[2]
   // 3,9,15 - B[3]
   // 4,10,16 - U[4]
   // 5,11,17 - D[5]

   // We hardcode each of the 18 moves to prevent computation during the
   // searches.

   // In these assignments, we are changing location and orientations as
   // necessary


   if( move == 0 ){ // R

	  // update edges
	  swap( state[4], state[1] );
	  swap( state[4], state[6] );
	  swap( state[4], state[9] );

	  // edge orientations only change on F/B
	  swap( state[24], state[21] );
	  swap( state[24], state[26] );
	  swap( state[24], state[29] );

	  // update corners 0,1,4,5
	  swap( state[16], state[12] );
	  swap( state[16], state[13] );
	  swap( state[16], state[17] );

	  state[36] = ( state[36] + 1 ) % 3;
	  state[32] = ( state[32] + 2 ) % 3;
	  state[33] = ( state[33] + 1 ) % 3;
	  state[37] = ( state[37] + 2 ) % 3;
	  swap( state[36], state[32] );
	  swap( state[36], state[33] );
	  swap( state[36], state[37] );

	  return state;

   }
   if( move == 1 ){ // L --- need to test
	  // update edges
	  swap( state[3], state[5] );
	  swap( state[3], state[11] );
	  swap( state[3], state[7] );

	  // edge orientations only change on F/B
	  swap( state[23], state[25] );
	  swap( state[23], state[31] );
	  swap( state[23], state[27] );

	  // update corners 0,1,4,5
	  swap( state[15], state[19] );
	  swap( state[15], state[18] );
	  swap( state[15], state[14] );

	  state[34] = ( state[34] + 2 ) % 3;
	  state[35] = ( state[35] + 1 ) % 3;
	  state[39] = ( state[39] + 2 ) % 3;
	  state[38] = ( state[38] + 1 ) % 3;
	  swap( state[35], state[39] );
	  swap( state[35], state[38] );
	  swap( state[35], state[34] );

	  return state;

   }
   if( move == 2 ){ // F --- need to test
	  // update edges
	  swap( state[0], state[4] );
	  swap( state[0], state[8] );
	  swap( state[0], state[5] );

	  // edge orientations only change on F/B
	  state[20] = ( state[20] + 1 ) % 2;
	  state[24] = ( state[24] + 1 ) % 2;
	  state[25] = ( state[25] + 1 ) % 2;
	  state[28] = ( state[28] + 1 ) % 2;
	  swap( state[20], state[24] );
	  swap( state[20], state[28] );
	  swap( state[20], state[25] ); 

	  // update corners 0,1,4,5
	  swap( state[12], state[16] );
	  swap( state[12], state[19] );
	  swap( state[12], state[15] );

	  state[0+32] = ( state[0+32] + 1 ) % 3;
	  state[4+32] = ( state[4+32] + 2 ) % 3;
	  state[7+32] = ( state[7+32] + 1 ) % 3;
	  state[3+32] = ( state[3+32] + 2 ) % 3;
	  swap( state[0+32], state[4+32] );
	  swap( state[0+32], state[7+32] );
	  swap( state[0+32], state[3+32] );

	  return state;

   }

   if( move == 3 ){ // B --- need to test
	  // update edges
	  swap( state[2], state[7] );
	  swap( state[2], state[10] );
	  swap( state[2], state[6] );

	  // edge orientations only change on F/B
	  state[2+20] = ( state[2+20] + 1 ) % 2;
	  state[7+20] = ( state[7+20] + 1 ) % 2;
	  state[6+20] = ( state[6+20] + 1 ) % 2;
	  state[10+20] = ( state[10+20] + 1 ) % 2; 
	  swap( state[2+20], state[7+20] );
	  swap( state[2+20], state[10+20] );
	  swap( state[2+20], state[6+20] );

	  // update corners 0,1,4,5
	  swap( state[1+12], state[2+12] );
	  swap( state[1+12], state[6+12] );
	  swap( state[1+12], state[5+12] );

	  state[1+32] = ( state[1+32] + 2 ) % 3;
	  state[2+32] = ( state[2+32] + 1 ) % 3;
	  state[6+32] = ( state[6+32] + 2 ) % 3;
	  state[5+32] = ( state[5+32] + 1 ) % 3;
	  swap( state[1+32], state[2+32] );
	  swap( state[1+32], state[6+32] );
	  swap( state[1+32], state[5+32] );

	  return state;

   }

   if( move == 4 ){ // U --- need to test

	  // update edges
	  swap( state[0], state[3] );
	  swap( state[0], state[2] );
	  swap( state[0], state[1] ); 

	  swap( state[0+20], state[3+20] );
	  swap( state[0+20], state[2+20] );
	  swap( state[0+20], state[1+20] );

	  // update corners 0,1,4,5
	  swap( state[0+12], state[3+12] );
	  swap( state[0+12], state[2+12] );
	  swap( state[0+12], state[1+12] );
	  swap( state[0+32], state[3+32] );
	  swap( state[0+32], state[2+32] );
	  swap( state[0+32], state[1+32] );

	  return state;

   }

   if( move == 5 ){ // D
	  // update edges
	  swap( state[8], state[9] );
	  swap( state[8], state[10] );
	  swap( state[8], state[11] );

	  swap( state[28], state[29] );
	  swap( state[28], state[30] );
	  swap( state[28], state[31] );

	  // update corners 0,1,4,5
	  swap( state[4+12], state[5+12] );
	  swap( state[4+12], state[6+12] );
	  swap( state[4+12], state[7+12] );
	  swap( state[4+32], state[5+32] );
	  swap( state[4+32], state[6+32] );
	  swap( state[4+32], state[7+32] );

	  return state;

   }


   if( move == 6 ){ // R2
	  // update edges
	  swap( state[1], state[9] );
	  swap( state[4], state[6] );

	  swap( state[1+20], state[9+20] );
	  swap( state[4+20], state[6+20] );

	  // update corners
	  swap( state[0+12], state[5+12] );
	  swap( state[1+12], state[4+12] );

	  swap( state[0+32], state[5+32] );
	  swap( state[1+32], state[4+32] );

	  return state;

   }

   if( move == 7 ){ // L2
	  // update edges
	  swap( state[3], state[11] );
	  swap( state[5], state[7] );

	  swap( state[3+20], state[11+20] );
	  swap( state[5+20], state[7+20] );

	  // update corners
	  swap( state[2+12], state[7+12] );
	  swap( state[3+12], state[6+12] );

	  swap( state[2+32], state[7+32] );
	  swap( state[3+32], state[6+32] );

	  return state;

   }
   if( move == 8 ){ // F2
	  // update edges
	  swap( state[0], state[8] );
	  swap( state[4], state[5] );
	  swap( state[0+20], state[8+20] );
	  swap( state[4+20], state[5+20] );

	  // update corners
	  swap( state[0+12], state[7+12] );
	  swap( state[3+12], state[4+12] );
	  swap( state[0+32], state[7+32] );
	  swap( state[3+32], state[4+32] );
	  return state;
   }
   if( move == 9 ){ // B2
	  // update edges
	  swap( state[2], state[10] );
	  swap( state[6], state[7] );
	  swap( state[2+20], state[10+20] );
	  swap( state[6+20], state[7+20] );

	  // update corners
	  swap( state[1+12], state[6+12] );
	  swap( state[2+12], state[5+12] );
	  swap( state[1+32], state[6+32] );
	  swap( state[2+32], state[5+32] );
	  return state;
   }
   if( move == 10 ){ // U2
	  // update edges
	  swap( state[0], state[2] );
	  swap( state[1], state[3] );
	  swap( state[0+20], state[2+20] );
	  swap( state[1+20], state[3+20] );

	  // update corners
	  swap( state[0+12], state[2+12] );
	  swap( state[1+12], state[3+12] );
	  swap( state[0+32], state[2+32] );
	  swap( state[1+32], state[3+32] );
	  return state;

   }
   if( move == 11 ){ // D2
	  // update edges
	  swap( state[8], state[10] );
	  swap( state[9], state[11] );
	  swap( state[8+20], state[10+20] );
	  swap( state[9+20], state[11+20] );

	  // update corners
	  swap( state[4+12], state[6+12] );
	  swap( state[5+12], state[7+12] );
	  swap( state[4+32], state[6+32] );
	  swap( state[5+32], state[7+32] );
	  return state;

   }
   if( move == 12 ){ // R' (R3)

	  // update edges
	  swap( state[1], state[4] );
	  swap( state[1], state[9] );
	  swap( state[1], state[6] );

	  // edge orientations only change on F/B
	  swap( state[1+20], state[4+20] );
	  swap( state[1+20], state[9+20] );
	  swap( state[1+20], state[6+20] );

	  // update corners 
	  swap( state[0+12], state[4+12] );
	  swap( state[0+12], state[5+12] );
	  swap( state[0+12], state[1+12] );

	  state[0+32] = ( state[0+32] + 2 ) % 3;
	  state[1+32] = ( state[1+32] + 1 ) % 3;
	  state[4+32] = ( state[4+32] + 1 ) % 3;
	  state[5+32] = ( state[5+32] + 2 ) % 3;
	  swap( state[0+32], state[4+32] );
	  swap( state[0+32], state[5+32] );
	  swap( state[0+32], state[1+32] );
	  return state;
   }
   if( move == 13 ){ // L' (L3)
	  // update edges
	  swap( state[3], state[7] );
	  swap( state[3], state[11] );
	  swap( state[3], state[5] );

	  // edge orientations only change on F/B
	  swap( state[3+20], state[7+20] );
	  swap( state[3+20], state[11+20] );
	  swap( state[3+20], state[5+20] );

	  // update corners 
	  swap( state[3+12], state[2+12] );
	  swap( state[3+12], state[6+12] );
	  swap( state[3+12], state[7+12] );

	  state[2+32] = ( state[2+32] + 2 ) % 3;
	  state[3+32] = ( state[3+32] + 1 ) % 3;
	  state[6+32] = ( state[6+32] + 1 ) % 3;
	  state[7+32] = ( state[7+32] + 2 ) % 3;
	  swap( state[3+32], state[2+32] );
	  swap( state[3+32], state[6+32] );
	  swap( state[3+32], state[7+32] );
	  return state;


   }
   if( move == 14 ){ // F'    
	  swap( state[0], state[5] );
	  swap( state[0], state[8] );
	  swap( state[0], state[4] );

	  // edge orientations only change on F/B
	  state[0+20] = ( state[0+20] + 1 ) % 2;
	  state[4+20] = ( state[4+20] + 1 ) % 2;
	  state[5+20] = ( state[5+20] + 1 ) % 2;
	  state[8+20] = ( state[8+20] + 1 ) % 2; 
	  swap( state[0+20], state[5+20] );
	  swap( state[0+20], state[8+20] );
	  swap( state[0+20], state[4+20] );

	  // update corners 0,1,4,5
	  swap( state[0+12], state[3+12] );
	  swap( state[0+12], state[7+12] );
	  swap( state[0+12], state[4+12] );

	  state[0+32] = ( state[0+32] + 1 ) % 3;
	  state[4+32] = ( state[4+32] + 2 ) % 3;
	  state[7+32] = ( state[7+32] + 1 ) % 3;
	  state[3+32] = ( state[3+32] + 2 ) % 3;
	  swap( state[0+32], state[3+32] );
	  swap( state[0+32], state[7+32] );
	  swap( state[0+32], state[4+32] );

	  return state;
   }
   if( move == 15 ){ // B' (B3)
	  swap( state[2], state[6] );
	  swap( state[2], state[10] );
	  swap( state[2], state[7] );

	  // edge orientations only change on F/B
	  state[2+20] = ( state[2+20] + 1 ) % 2;
	  state[6+20] = ( state[6+20] + 1 ) % 2;
	  state[7+20] = ( state[7+20] + 1 ) % 2;
	  state[10+20] = ( state[10+20] + 1 ) % 2; 
	  swap( state[2+20], state[6+20] );
	  swap( state[2+20], state[10+20] );
	  swap( state[2+20], state[7+20] );

	  // update corners 0,1,4,5
	  swap( state[1+12], state[5+12] );
	  swap( state[1+12], state[6+12] );
	  swap( state[1+12], state[2+12] );

	  state[1+32] = ( state[1+32] + 2 ) % 3;
	  state[2+32] = ( state[2+32] + 1 ) % 3;
	  state[6+32] = ( state[6+32] + 2 ) % 3;
	  state[5+32] = ( state[5+32] + 1 ) % 3;
	  swap( state[1+32], state[5+32] );
	  swap( state[1+32], state[6+32] );
	  swap( state[1+32], state[2+32] );

	  return state;
   }
   if( move == 16 ){ // U' (U3)
	  // update edges
	  swap( state[0], state[1] );
	  swap( state[0], state[2] );
	  swap( state[0], state[3] );

	  swap( state[0+20], state[1+20] );
	  swap( state[0+20], state[2+20] );
	  swap( state[0+20], state[3+20] );

	  // update corners 0,1,4,5
	  swap( state[0+12], state[1+12] );
	  swap( state[0+12], state[2+12] );
	  swap( state[0+12], state[3+12] );
	  swap( state[0+32], state[1+32] );
	  swap( state[0+32], state[2+32] );
	  swap( state[0+32], state[3+32] );

	  return state;
   }
   if( move == 17 ){ // D' (D3)
	  // update edges
	  swap( state[8], state[11] );
	  swap( state[8], state[10] );
	  swap( state[8], state[9] );

	  swap( state[8+20], state[11+20] );
	  swap( state[8+20], state[10+20] );
	  swap( state[8+20], state[9+20] );

	  // update corners 0,1,4,5
	  swap( state[4+12], state[7+12] );
	  swap( state[4+12], state[6+12] );
	  swap( state[4+12], state[5+12] );
	  swap( state[4+32], state[7+32] );
	  swap( state[4+32], state[6+32] );
	  swap( state[4+32], state[5+32] );

	  return state;
   } 
   return state;
} 

// Another method of applyMove(), inspired by Pochmann's elegant solution of
// saving line space. Tests to be done to determine if hard coding is more
// efficient and quicker than line saving, computationally heavy method as below.

//--- Defines which cubies will be affected by a particular face move
/*int affectedCubies[][8] {
  {  1,  6,  9,  4,  0,  1,  5,  4 }, // R face -- checked x1
  {  3,  5,  11, 7,  2,  3,  7,  6 }, // L face -- checked x1
  {  0,  4,  8,  5,  3,  0,  4,  7 }, // F face -- checked x1
  {  2,  7,  10, 6,  1,  2,  6,  5 }, // B face -- checked x1
  {  0,  3,  2,  1,  0,  3,  2,  1 }, // U face -- checked x1
  {  8,  9, 10, 11,  4,  5,  6,  7 }  // D face -- checked x1
  }; // 'Magic' done with locations of numbers in this array to reflect correct
// modulus for corners and edges 

vi applyMove(int move, vi state){
// 0,6,12 - R[0]
// 1,7,13 - L[1]
// 2,8,14 - F[2]
// 3,9,15 - B[3]
// 4,10,16 - U[4]
// 5,11,17 - D[5]
// Determine whether we perform 1,2, or 3 turns of a face
int turns = ( move / 6 ) + 1;
// Determine which face will have operations performed on it
int face = move%6 ;
//--- Perform each move for a specified amount of turns
while( turns ){
//--- Preserve old state for reassignment
vi oldState = state;
//--- For each of the move that affects the 8 affected cubies as defined
//by the affectedCubies[][8] as above
for( int i=0; i<8; i++){
// We update location and orientation
// First, we handle if it is an edge
if( i < 4 ){ // affectedCubies[face][i]
// handle edge location and orientation change
// Get target cubie, get new location for target cubie, and update
// corner orientation depending on face 
int target = affectedCubies[face][i];
int destination = affectedCubies[face][ ( i+1 )%4 ];
state[destination] = oldState[target];
//--- If F/B move, flip orientations 
if( ( face == 2 ) || ( face == 3 ) ){
state[destination+20] = ( oldState[target+20] + 1 ) % 2;
}
else{
state[destination+20] = oldState[target+20];
}
}
// Now, we update corners
if( i >= 4 ){ // corners
int target = affectedCubies[face][i];
int destination = affectedCubies[face][ ( (i+1)%4 ) + 4 ];
state[destination+12] = oldState[target+12];
//--- Corner orientation change. Only change if performing a move
//on the R/L/F/B face
if( ( face != 4 ) && ( face != 5 ) ){
state[destination+32] =
// +2 to orientation if originally at an even index, 
// +1 otherwise
( ( oldState[target+32] + ( (i%2 == 0) ? 2 : 1 ) ) % 3 );
}
else{
state[destination+32] = oldState[target+32];
}
}
}
--turns;
}
return state;
} */

// Applicable moves for appropriate phases
//--- 18 possible moves ---
/* 
 * R  -  0
 * L  -  1
 * F  -  2
 * B  -  3
 * U  -  4
 * D  -  5
 * R2 -  6
 * L2 -  7
 * F2 -  8
 * B2 -  9
 * U2 - 10
 * D2 - 11
 * R3 - 12
 * L3 - 13
 * F3 - 14
 * B3 - 15
 * U3 - 16
 * D3 - 17 
 */


//---------------------------------------------------------
// iterative deepening depth first search to compare memory and runtime usage
// vs a Bidirectional breadth first search

//---------------------------------------------------------
vector<vi> applicableMoves{
   { 0 }, // g0 group
	  {0,1,2,3,4,5}, // moves to transition to g1 // phase 1
	  {0,1,8,9,4,5}, // moves to transition to g2 // phase 2
	  {0,1,8,9,10,11}, // moves to transition to g3 // phase 3
	  {6,7,8,9,10,11} // moves to transition to g4 // phase 4
};

// Define the integer change to result in an inverse move upon inputting
int inverse(int move){
   if( move <= 5 ){
	  move = move+12;
	  return move;
   }
   if( move >= 12 ){
	  move = move - 12;
	  return move;
   }
   else{
	  return move;
   }
}


// Print current state in a viewable format 
void print_state(vi & state){
   cout << "<";
   for( int i = 0; i < state.size(); i++ ){
	  if( i > 11 && i < 20){
		 cout << " " << state[i] - 12;
	  }
	  else{
		 cout << " " << state[i];
	  }
	  if( i == 11 ){
		 cout << "|";
	  }
	  if( i == 19 ){
		 cout << "|";
	  }
	  if( i == 31 ){
		 cout << "|";
	  }
   }
   cout << " > " << endl;
}  

// Bidirectional Breadth First Search
vi BDBFS(vi & startState, vi goalState){

   // compute start state ID, goal state ID
   vi startID = id(startState);
   vi goalID = id(goalState);

   // initialize queues for forward and backward search
   // queue of states
   queue<vi> q;
   q.push(startState);
   q.push(goalState);

   // initialize tables for BFS
   map<vi, int> direction;
   map<vi, int> lastMove;
   map<vi, vi> predecessor;

   // initialize directions for starting states
   direction[startID] = 1;
   direction[goalID] = 2;

   // Already in phase, return
   if( startID == goalID ){
	  vi retPath;
	  return retPath;
   }

   // begin BFS for particular phase
   while(!q.empty()){

	  // get information from queue
	  vi oldState = q.front();
	  q.pop();
	  vi oldID = id(oldState);
	  int& oldDir = direction[oldID];

	  // Get appropriate moveset for group
	  vi moveSet = applicableMoves[phase];
	  for( int i = 0; i < moveSet.size(); i++ ){
		 int move = moveSet[i];
		 totalMoves++; // helpful data gathering

		 vi newState = applyMove(move, oldState);
		 vi newID = id(newState);
		 int& newDir = direction[newID];

		 //--- newDir == 0 if it is a new direction
		 //--- newDir == 1 if we have seen this from the forward search
		 //--- newDir == 2 if if we have seen this from the backward search
		 //--- if newDir > 0, and newDir != oldDir, then we have found a
		 //connecting path from forward and backward search

		 if( (newDir > 0) && (newDir != oldDir) ){

			// if newDir is 1, we are coming from backwards search
			if( newDir == 1 ){

			   vi path;
			   // rebuild path from newID -> startID
			   while( newID != startID ){
				  path.insert(path.begin(), lastMove[newID]);
				  newID = predecessor[newID];
			   }

			   // Applying connecting move
			   path.push_back(inverse(move));

			   // rebuild path from oldID -> goalID
			   while( oldID != goalID ){
				  path.push_back(inverse(lastMove[oldID]));
				  oldID = predecessor[oldID];
			   }

			   // Applying path to input starting state
			   for( int i = 0; i < path.size(); i++ ){
				  startState = applyMove(path[i], startState);
			   }
			   return path;

			}
			// we are coming from forward search
			if( newDir == 2 ){

			   vi path;

			   // rebuild path from oldID -> startID
			   while( oldID != startID ){
				  path.insert(path.begin(), lastMove[oldID]);
				  oldID = predecessor[oldID];
			   }

			   // Applying connecting move
			   path.push_back((move));

			   // rebuild path from newID -> goalID
			   while( newID != goalID ){
				  path.push_back(inverse(lastMove[newID]));
				  newID = predecessor[newID];
			   }


			   // Applying path to input starting state
			   for( int i = 0; i < path.size(); i++ ){
				  startState = applyMove(path[i], startState);
			   }
			   return path;

			}
		 }

		 // only insert into queue if we have not seen this ID
		 if( ! newDir ){
			q.push(newState);
			newDir = oldDir;
			lastMove[newID] = move;
			predecessor[newID] = oldID;
		 }
	  }
   }
   return startState;
}


// Initialize a cube's solved state
vi initialize(){
   vi state{UF,UR, UB, UL, FR, FL, BR, BL, DF, DR, DB, DL, UFR, UBR, UBL,
	  UFL, DFR, DBR, DBL, DFL};
   for(int i = 0; i < 20; i++){
	  state.push_back(0);
   }
   return state;
}


vector<string> movesString{ "R", "L", "F", "B", "U", "D", "R2", "L2", "F2", 
   "B2", "U2", "D2", "R'", "L'", "F'", "B'", "U'", "D'" };

// Add phase paths into a single string
void build_path( vi path, string & build ){
   for( int i = 0; i < path.size(); i++ ){
	  build += ( movesString[path[i]] + " " );
   }
}

// Apply random moves 0-17 to a state and return the path
vi scramble(int number_of_moves, vi & state){
   int random;
   srand(time(NULL));
   vi path;

   for( int i = 0; i < number_of_moves; i++){
	  random = rand()%18;
	  state = applyMove(random, state);
	  path.push_back(random);
   }
   return path;
}


// For a specified amount of solves, scramble a cube for a certain amount of
// moves, and return the solution 
int main(int argc, char** argv){

   int averageMovesPerformed = 0;
   int averagePathLength = 0;
   // initialize a cube in its solved state
   for( int i = 0; i < 1; i++ ){

	  totalMoves = 0;

	  // initialize a blank cube
	  vi cube = initialize();
	  vi goalCube = initialize();

	  // Scramble the cube
	  vi scramble_path = scramble(30, cube);
	  string sp;
	  build_path(scramble_path, sp);

	  // begin solving cube by iteratively going through the 4 phases
	  vi path; // intermediate phase solution
	  string build; // complete solution string

	  phase = 1;
	  path = BDBFS( cube, goalCube );
	  build_path(path, build);
	  averagePathLength += path.size();

	  phase = 2;
	  path = BDBFS( cube, goalCube );
	  averagePathLength += path.size();
	  build_path(path, build);

	  phase = 3;
	  path = BDBFS( cube, goalCube );
	  averagePathLength += path.size();
	  build_path(path, build);

	  phase = 4;
	  path = BDBFS( cube, goalCube );
	  averagePathLength += path.size();
	  build_path(path, build);

	  // Print scramble path, solution
	  cout << sp << endl;
	  cout << build << endl;
	  averageMovesPerformed += totalMoves;
   }

   // Statistics for large number of cubes solved
   //cout << "Average moves performed: " << averageMovesPerformed/1 << endl;
   //cout << "Average path length: " << averagePathLength/1 << endl;

}
