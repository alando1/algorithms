#include <iostream>
#include <fstream>
using namespace std;

long long** states;

// iteratively calculates the number of valid game states that a 
// game of chomp with r rows and c columns has.
// It loops through each index of our 2D array and changes its value
// based on the values of the indexes around it. The stored value in 
// the index represents the number of valid game states in a game of 
// size:
//      rows = row position of index
//      columns = column position of index
// starting with the smallest case of a 1x1 game and working up to a rxc sized game.
long long iterativeChomp(int r, int c)
{
	for(int i=0; i<r; i++)
		for(int j=0; j<c; j++)
		{
            // this is the top left corner of the array
            // set to 2 since a 1x1 game has 2 valid states
			if(i == 0 && j == 0)
				states[i][j] = 2;
			else
			{
                // if this is true, we are in the first row
                // adding one column to a game with one row
                // adds only one extra state to the game.
				if(i == 0)
					states[i][j] = 1 + states[i][j-1];
				else 
				{
                    // if this is true we are in the first column
                    // similar to above, adding a row to a game
                    // with only one column adds one extra state
					if(j == 0)
						states[i][j] = 1 + states[i-1][j];

                    // if this occurs we are not on the top or left
                    // edge of the game map. A game of size ixj will
                    // have a number of states equal to the sum of the
                    // index above it with the index left of it.
					else
						states[i][j] = states[i-1][j] + states[i][j-1];
				}
			}
		}
    // return the adjusted 2D array
	return states[r-1][c-1];
}

// create a 2D array to hold our game state
void buildArray(int r,int c)
{
	states = new long long*[r];
	for(int i=0; i<r; i++)
		states[i] = new long long[c];
}

// deletes the array and frees memory
void destructor(int r)
{
	for(int i=0; i<r; i++)
		delete [] states[i];
	delete [] states;
}

// set all values of the 2d array to 0
void initialize(int r, int c)
{
	for(int i=0; i<r; i++)
		for(int j=0; j<c; j++)
			states[i][j] = 0;
}

int main()
{
	int p, rows, cols;
    ifstream inputFile;
    ofstream outputFile;

    // open the in/output files
    inputFile.open("input.txt");
    outputFile.open("output.txt");
	
    // make sure we have our input
    if( !inputFile.is_open() )
        cout << "Error, no input file\n";

    // get our number of games and loop once for each game
    inputFile >> p;

	for(int i=0; i<p; i++)
	{
        // input the size of the game
		inputFile >> rows;
		inputFile >> cols;

        // build and initialize the 2d array
		buildArray(rows,cols);		
		initialize(rows,cols);

        // perform iterative calculations to find the number of valid
        // states with exactly r rows and c columns
		long long exactChomps = iterativeChomp(rows-1,cols-1);

        // perform iterative calculations to find the total number of valid states
		long long allChomps = iterativeChomp(rows,cols);

        // write the output to file
		outputFile << exactChomps << " " << allChomps << endl;

        // kill the arrays
		destructor(rows);
	}

    // close out files
    inputFile.close();
    outputFile.close();
	return 0;
}
