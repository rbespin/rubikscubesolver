"""
Name: Robert Espinoza
Email: rbespinoza96@gmail.com

The goal of this program is to solve a scrambled rubik's cube
It utilizes Morwen Thistlethwaite's algorithm, by breaking the solve up 
into four phases, each subsequent phase restricting more moves, and thus, 
restricting possible states reachable in each phase.

Phase 1 orients the edges, where an edge is defined as oriented if it can be
brought back into its solved location making use only of U,D,R,L turns. 
<R,L,F,B,U,D> moves are allowed to transition from phase 0 to phase 1

Phase 2 orients the corners and places the proper edge cubies in their M-slice. 
A corner is defined to be oriented if there is a U or D sticker on 
the L or R face. 
<R,L,F2,B2,U,D> moves are allowed to transition from phase 1 to phase 2

Phase 3 places the edge cubies in their respective E-slice and S-slice. 
As well, the corners are placed into their proper tetrads. A corner in its
correct tetrad is every position the corner can reach from a solved state
using only half turns. This program further restricts this by pairing each
corner (UFR,UBL),(UBR,UFL),(DFR,DBL),(DBR,DFL) with each other. 
This phase also ensures that the cube has overall even parity, i.e. there
are only an even amount of unsolved cubies(the cube cannot be solved using only
half turns if there are an odd number of corrections to make).
<R,L,F2,B2,U2,D2> moves are only allowed to transition from phase 2 to phase 3

Phase 4 solves the cube using only half turns.
<R2,L2,F2,B2,U2,D2> moves are only allowed to transition from phase 3 to phase 4

This program utilizes a bidirectional breadth first search to find the paths
to transition between phases. An 'id' of relevant info is calculated at each 
phase, as the entire cube need not be considered at each phase.

"""
import random

#--- Edge identifiers
UF = 0
UR = 1
UB = 2
UL = 3
FR = 4
FL = 5
BR = 6
BL = 7
DF = 8
DR = 9
DB = 10
DL = 11

#--- Corner identifiers
UFR = 12
UBR = 13
UBL = 14
UFL = 15
DFR = 16
DBR = 17
DBL = 18
DFL = 19

#--- Variable for tracking phase of thistlethwaite's algorithm
phase = 0

#--- function to convert a list into a tuple
def convertToTuple(list):
    return tuple(list)

#--- function to convert a tuple into a list
def convertToList(tuple):
    return list(tuple)

#--- gives relevant info of current state of cube given by state parameter
def id(state):
   #--- Phase 1
   #--- Fix edge orientations
    if(phase == 1):
        cubeid = []
        for x in range(20, 32):
            cubeid.append(state[x])
        return cubeid

   #--- Phase 2
   #--- Fix corner orientations, M-Slice
    if(phase == 2):
        cubeid = []
        #--- corner orientations
        for x in range(32,40):
            cubeid.append(state[x])

        #--- corner orientations
        for x in range(12, 20):
            if (state[x] == UFR or 
                state[x] == UBL or 
                state[x] == DBR or 
                state[x] == DFL):
                cubeid.append(x)

   	  #--- M-Slice
        for x in range(0,12):
            if (state[x] == 0 or 
                state[x] == 2 or 
                state[x] == 8 or 
                state[x] == 10):
                cubeid.append(x)
        return cubeid

    #--- Phase 3
    #--- E-Slice, S-Slice, corner tetrads and orbits, overall even parity
    if(phase == 3):
        cubeid = []
        #--- E-Slice
        for x in range(0,12):
            if (state[x] == 4 or 
                state[x] == 5 or 
                state[x] == 6 or 
                state[x] == 7):
                cubeid.append(x)

        #--- S-Slice cubies will naturally fall into place

        #--- 0,2,5,7 corners in correct places
        for x in range(12, 20):
            if (state[x] == UFR or 
                state[x] == UBL or 
                state[x] == DBR or 
                state[x] == DFL):
                cubeid.append(x)

        #--- Ensuring (UFR,UBL) and (DBR,DFL) are paired
        for x in range(12,20):
            if state[x] == UFR or state[x] == UBL :
                cubeid.append(x)
        for x in range(12,20):
            if state[x] == DBR or state[x] == DFL :
                cubeid.append(x)        

        #--- 1,3,4,6 corners in correct places
        for x in range(12, 20):
            if (state[x] == UBR or 
                state[x] == UFL or 
                state[x] == DFR or 
                state[x] == DBL):
                cubeid.append(x)

        #--- Ensuring (1,3) and (4,6) are paired
        for x in range(12,20):
            if state[x] == UBR or state[x] == UFL :
                cubeid.append(x)
        for x in range(12,20):
            if state[x] == DFR or state[x] == DBL :
                cubeid.append(x)

        #--- Ensuring even parity
        tetrad = 0
        for x in range(20,32):
            for y in range(x+1, 20):
                if state[x] > state[y]:
                    tetrad+=1

        if tetrad%2 == 0:
            cubeid.append(tetrad)

        return cubeid

    #--- Phase 4
    #--- Fix remaining cubies
    if(phase == 4):
        return state

    return state

#--- defines which cubies will be affected by which move
affectedCubies = []
affectedCubies.append([1,6,9,4,0,1,5,4])   #--- R face
affectedCubies.append([3,5,11,7,2,3,7,6])  #--- L face
affectedCubies.append([0,4,8,5,3,0,4,7])   #--- F face
affectedCubies.append([2,7,10,6,1,2,6,5])  #--- B face
affectedCubies.append([0,3,2,1,0,3,2,1])   #--- U face
affectedCubies.append([8,9,10,11,4,5,6,7]) #--- D face

#--- function to apply move to a given cube state
#--- move - an integer indication which move to perform
#--- state - a list of integers indicating the state of the cube
def applyMove(move, inputstate):
    state = inputstate.copy()
    turns = (move//6) + 1
    face = move%6
    while turns > 0:
        oldState = state.copy()
        for x in range(0,8):
            #--- Edges and orientations
            if(x < 4):
                target = affectedCubies[face][x]
                destination = affectedCubies[face][(x+1)%4]
                state[destination] = oldState[target]
            if(face == 2 or face == 3):
                state[destination+20] = (oldState[target+20]+1)%2
            else:
                state[destination+20] = oldState[target+20]

            #--- Corners and orientations
            if(x >= 4):
                target = affectedCubies[face][x]
                destination = affectedCubies[face][((x+1)%4)+4]
                state[destination+12] = oldState[target+12]
                if(face!=4 and face!=5):
                    if(x%2==0):
                        state[destination+32] = (oldState[target+32] + 2)%3
                    else:
                        state[destination+32] = (oldState[target+32] + 1)%3
                else:
                    state[destination+32] = oldState[target+32]
        turns-=1
    return state

#--- list of applicable moves at each phase of the algorithm
applicableMoves = [
        [0],
        [0,1,2,3,4,5], 
        [0,1,8,9,4,5], 
        [0,1,8,9,10,11], 
        [6,7,8,9,10,11]
]

#--- performs the inverse of a given move
def inverse(move):
    if(move<=5):
        move +=12
        return move
    if(move>=12):
        move-=12
        return move
    else:
        return move

#--- initializes and returns a solved cube state
def initialize():
    #--- 12 edges, 8 corners, 12 edge orientations, 8 corner orientations
    state = [UF,UR, UB, UL, FR, FL, BR, BL, DF, DR, DB, DL, UFR, UBR, UBL,
      UFL, DFR, DBR, DBL, DFL]
    for x in range(0,20):
        state.append(0)
    return state

#--- scrambles a given cube state using random moves
def scramble(numberOfMoves, state):
    path = []
    for x in range(0, numberOfMoves):
        randmove = random.randint(0,17)
        applyMoveStatic(randmove, state)
        path.append(randmove)
    return path

#--- converts a numerical moves string into alphabetical move string
movesString = ["R", "L", "F", "B", "U", "D", "R2", "L2", "F2", 
   "B2", "U2", "D2", "R'", "L'", "F'", "B'", "U'", "D'"]
def convertpath(path):
    letterpath = ''
    for x in range(0, len(path)):
        letterpath += (movesString[path[x]]) + " "
    return letterpath

#--- Bidirectional breadth first search to transition into subsequent phase
def BDBFS(startState, goalState):
    
    #--- ID's of start and goal states
    startID = id(startState)
    goalID = id(goalState)

    #--- queue for forward and backward searches
    q = []
    q.append(startState)
    q.append(goalState)

    #--- hash maps for our BFS
    direction = {}
    lastMove = {}
    predecessor = {}

    #--- converting list to tuple to store in hash map
    startID = convertToTuple(startID)
    goalID = convertToTuple(goalID)

    direction[startID] = 1  #--- 1 is forward search
    direction[goalID] = 2   #--- 2 is backward search

    #--- path of moves to transition to next phase
    path = []

    #--- already in phase, return
    if startID == goalID :
        print("already in phase")
        return path

    counter = 0;

    #--- begin BDBFS
    while( len(q) != 0 ):

        #--- getting old state and id
        #oldStateCopy = q.pop(0)
        #oldState = oldStateCopy.copy()
        oldState = q.pop(0)
        oldID = id(oldState)
        oldID = convertToTuple(oldID)
        #--- we are guaranteed to have this in hash map at this point
        oldDir = direction[oldID]

        moveSet = applicableMoves[phase]

        #--- performing all moves on old state, putting back in queue
        #--- if we have found an intersection, we have found a solution
        for x in range(0, len(moveSet)):

            move = moveSet[x]

            newState = applyMove(move, oldState)
            newID = id(newState)
            newID = convertToTuple(newID)
            #newDir = direction[newID]
           # newDir = id(direction[newID])

            #--- if we have seen this state, and we are coming from a new 
            #--- direction, we have found an intersection
            if( (newID in direction) and (direction[newID] != oldDir)):

                print("intersection found")

                #--- if newDir is 1, we are coming from backward search.
                #--- we rebuild path accordingly
                if(direction[newID] == 1):
                    print("newDir is 1")
                    while(newID != startID):
                        path.insert(0, lastMove[newID])
                        newID = predecessor[newID]

                    path.append(inverse(move))

                    while(oldID != goalID):
                        path.append(inverse(lastMove[oldID]))
                        oldID = predecessor[oldID]

                    for x in range (0, len(path)):
                        startState = applyMove(path[x], startState)

                    return path
                if(direction[newID] == 2):
                    print("newDir is 2")

                    while(oldID != startID):
                        path.insert(0, lastMove[oldID])
                        oldID = predecessor[oldID]

                    path.append(move)

                    while(newID != goalID):
                        path.append(inverse(lastMove[newID]))
                        newID = predecessor[newID]

                    for x in range(0, len(path)):
                        startState = applyMove(path[x], startState)
                        print(startState)

                    return path

            if(not newID in direction):
                q.append(newState)
                direction[newID] = oldDir
                lastMove[newID] = move
                predecessor[newID] = oldID

    return startState



#--- Main
"""
cube = initialize()
cubetwo = initialize()

path = scramble(10, cube)
letterpath = convertpath(path)
print("Scramble path: ")
print(letterpath)

print()
print(path)

path.reverse()

print(path)
print(cube)
for x in range(0,len(path)):
    applyMoveStatic(path[x],cube)
    print("applying move: ", path[x])
    print(cube)

print(cube)

phase = 1
#path = BDBFS(cube, cubetwo)
#print(letterpath)

#path = BDBFS(cube, cubetwo)

#print(path)

#newpath = convertpath(path)
#print(newpath)
"""

"""if newID in direction:
    print("key is inside")
else:
    print("key is not inside") """

cube = initialize()
cuberef = cube
cube2 = initialize()

cube = applyMove(3,cube)
print(cube)
cube = applyMove(15,cube)
print(cube)






