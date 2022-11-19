#pragma once


// https://stackoverflow.com/questions/19193413/pathfinding-in-a-grid-system
// Mehul Nirala wrote most of this


#include <utility>
#include <stack>
#include <set>
#include <iostream>
#include <vector>
using namespace std;


#define ROW 8 // this is dependent on the board.vox file
#define COL 8

// Creating a shortcut for int, int pair type
typedef pair<int, int> Pair;

// Creating a shortcut for pair<int, pair<int, int>> type
typedef pair<double, pair<int, int> > pPair;

// A structure to hold the neccesary parameters
struct cell
{
    // Row and Column index of its parent
    // Note that 0 <= i <= ROW-1 & 0 <= j <= COL-1
    int parent_i, parent_j;
    // f = g + h
    double f, g, h;
};

// A Utility Function to check whether given cell (row, col)
// is a valid cell or not.
bool isValid(int row, int col);

// A Utility Function to check whether the given cell is
// blocked or not
bool isUnBlocked(int grid[][COL], int row, int col);

// A Utility Function to check whether destination cell has
// been reached or not
bool isDestination(int row, int col, Pair dest);

// A Utility Function to calculate the 'h' heuristics.
double calculateHValue(int row, int col, Pair dest);

// A Utility Function to trace the path from the source
// to destination
void tracePath(cell cellDetails[][COL], Pair dest, vector<pair<int, int>>& output_path);

// A Function to find the shortest path between
// a given source cell to a destination cell according
// to A* Search Algorithm
bool aStarSearch(int grid[][COL], Pair src, Pair dest, vector<pair<int, int>>& output_path);