

#include <iostream>
#include <vector>
#include <fstream>
#include <sstream>
#include <cctype>
#include "strutils.h"

using namespace std;


/*
Method declarations without body are below.
You can find more information on method definitions.
*/

// Takes the board from the file and puts it in a matrix vector.
bool takeBoard(vector<vector<int>> & matrix, string & filename, bool &hasBiggerValue, int &biggerValue); 

//Takes constraint matrix from file.
bool takeConstraints(vector<vector<int>> & matrix, string & filename, bool &hasBiggerValue, int &biggerValue, int boardSize);

void printMatrix(vector<vector<int>> & matrix ); 
bool checkRepeatOnColumn(vector<vector<int>> & matrix);
bool checkRepeatOnRow(vector<vector<int>> & matrix);
bool mismatchConstraints(vector<vector<int>> & matrix, vector<vector<int>> & constraints);
bool solvePuzzle(vector<vector<int>> & matrix, vector<vector<int>> & constraints, bool & hasModified);
bool isSolved(vector<vector<int>> & matrix, vector<vector<int>> & constraints);
bool fillSingleSpaces(vector<vector<int>> & matrix, vector<vector<int>> & constraints, bool & hasModified);
bool fillWithConstraints(vector<vector<int>> & matrix, vector<vector<int>> & constraints, bool & hasModified);
vector<int> createValueMap(vector<vector<int>> & matrix, int row, int column, bool isRow);
vector<int> createCandidateVector(vector<vector<int>> & matrix, int row, int column);
bool hadSingleCandidate(vector<vector<int>> & matrix, int row, int column, int otherValue, bool isBigger);
bool foundACandidate(vector<vector<int>> & matrix, int x1, int y1, int x2, int y2);


//Method definitions are below.


// This method has four arguments;
// matrix is an empty board, filename is the name of the file that will be taken from the user.
// hasBiggerValue is a flag to control if matrix has a value bigger than it should have, 
// biggerValue is kept to show the big value to the user as an output.
// matrix will be filled with values in the file via ifstream.
// since it will keep board as a matrix of integers, the X values will be changed to 0.
// Output: return true if takes succesfully the board, otherwise return false.
bool takeBoard(vector<vector<int>> & matrix, string & filename, bool &hasBiggerValue, int &biggerValue){
	ifstream input;
	int sizeOfBoard = 0; // we will need to check size.
	int numOfLines = 0; // keeping number of lines to check with size of board.
	string line;

	input.open(filename.c_str());

	if(input.fail()){
		cout << "Could not open the file." << endl;
		return false;
	}else {
		while(getline(input,line)){
			// some files can have empty lines, maybe at the end. That if block prevents that confusion.
			if(line.length() > 0){ 
				istringstream stringStream(line);

				if(numOfLines == 0){   // first line has board's size as a single integer.
					stringStream >> sizeOfBoard; 
				}else{
					matrix.push_back(vector<int>()); // adds an empty row to our matrix for each line.
					string value;
					// taking each value in value variable which is a string.
					while(stringStream >> value ){
						ToUpper(value);
						int row = matrix.size()-1; // our current row is the last row.
						if(value == "X"){
							(matrix[row]).push_back(0);
						}else{
							int val = stoi(value);
							(matrix[row]).push_back(val);

							// Checking bigger values.
							if(val > sizeOfBoard){
								hasBiggerValue = true;
								biggerValue = val;
								return false;
							}
						}
					}
				}
				numOfLines++;
			}
		}
		numOfLines = numOfLines -1; // Since first line was the size, we exclude it.

		if(numOfLines != sizeOfBoard ){
			return false;
		}
		return true;
	}
}

// This method has four arguments;
// matrix is an empty board, filename is the name of the file that will be taken from the user.
// hasBiggerValue is a flag to control if matrix has a value bigger than it should have, 
// biggerValue is kept to show as message to the user.
// matrix will be filled with values in the file via ifstream.
bool takeConstraints(vector<vector<int>> & matrix, string & filename, bool &hasBiggerValue, int &biggerValue, int boardSize){
	ifstream input;
	int sizeOfConstraintsFile = 0; // we will need to check size.
	int numOfLines = 0; // keeping number of lines to check with size of board.
	string line;

	input.open(filename.c_str());

	if(input.fail()){
		cout << "Could not open the file." << endl;
		return false;
	}else {
		while(getline(input,line)){
			// some files can have empty lines, maybe at the end. That if block prevents that confusion.
			if(line.length() > 0){ 
				istringstream stringStream(line);

				if(numOfLines == 0){   // first line has board's size as a single integer.
					stringStream >> sizeOfConstraintsFile; 
				}else{
					matrix.push_back(vector<int>()); // adds an empty row to our matrix for each line.
					int value;
					// taking each value in value variable which is a integer.
					while(stringStream >> value ){
						int row = matrix.size()-1; // our current row is the last row.
						(matrix[row]).push_back(value);

						// Checking bigger values.
						if(value > boardSize){
							hasBiggerValue = true;
							biggerValue = value;
							return false;
						}
					}
				}
			}
			numOfLines++;
		}
	}
	numOfLines = numOfLines -1; // Since first line was the size, we exclude it.

	if(numOfLines != sizeOfConstraintsFile ){
		return false;
	}
	return true;
}


//takes matrix as only parameter and return true if it has a repeating value on a Column.
bool checkRepeatOnColumn(vector<vector<int>> & matrix){
	for (unsigned int i = 0; i < matrix.size() ; i++){
		vector<int> row = createValueMap(matrix,i,i,false);
		for (unsigned int i = 1; i < row.size(); i++){
			if(row[i] > 1){
				return true;
			}
		}
	}
	return false;
}

//takes matrix as only parameter and return true if it has a repeating value on a Row.
bool checkRepeatOnRow(vector<vector<int>> & matrix){
	for (unsigned int i = 0; i < matrix.size() ; i++){
		vector<int> row = createValueMap(matrix,i,i,true);
		for (unsigned int i = 1; i < row.size(); i++){

			if(row[i] > 1){
				return true;
			}
		}
	}
	return false;
}

// checks constarints and return true if it has any mismatch.
bool mismatchConstraints(vector<vector<int>> & matrix, vector<vector<int>> & constraints){
	int x1,y1,x2,y2;
	for (unsigned int i = 0; i < constraints.size(); i++){
		x1 = constraints[i][0];
		y1 = constraints[i][1];
		x2 = constraints[i][2];
		y2 = constraints[i][3];

		int bigValue = matrix[x1-1][y1-1]; 
		int smallValue = matrix[x2-1][y2-1];

		// if any of them is zero, that means it is not filled yet. So we won't check that one.
		if(((bigValue != 0) && (smallValue != 0)) && (bigValue < smallValue)){ 
			return true;
		}
	}
	return false;
}

// Tries to solve the puzzle according to constaints and if so returns true.
// If no modification is done and there is still X in the board, it returns false;
bool solvePuzzle(vector<vector<int>> & matrix, vector<vector<int>> & constraints, bool & hasModified){

	if(isSolved(matrix, constraints)){
		cout << "Files are in correct format" << endl;
		printMatrix(matrix);
		return true;
	}
	else if(hasModified){ // If it is true that means we made a change in the last time.
		hasModified = false; // reseting the flag.
		fillSingleSpaces(matrix,constraints, hasModified);
		fillWithConstraints(matrix, constraints, hasModified);
		return	solvePuzzle(matrix,constraints,hasModified); // since had modified, tries to solve again. 
	}
	else{
		// matrix is not solved and no modification could be done in the last turn.
		cout << "X's can have more than 1 value" << endl;
		return false;
	}
}

// Checks whether all X values are filled with corresponding values and has correct solution.
// Return true if so.
bool isSolved(vector<vector<int>> & matrix, vector<vector<int>> & constraints){

	for (unsigned int i = 0; i < matrix.size() ; i++){
		for (unsigned int j = 0; j < matrix[i].size() ; j++){
			if( matrix[i][j] == 0 ){
				return false;
			} 
		}
	}

	if(mismatchConstraints(matrix,constraints)){
		return false;
	}

	return true;
}

// creates a vector of int which is showing each value's repeat time in a single row or column.
// e.g: if there is two 6 values on a row, valuesMap[6] will be 2;
// It will help to find single poosible values can be put in X's place.
vector<int> createValueMap(vector<vector<int>> & matrix, int row, int column, bool isRow){
	int size = matrix.size();
	vector<int> valuesMap(size+1,0);

	for (int i = 0; i < size; i++){ 
		if(isRow){
			valuesMap[matrix[row][i]]++;
		}else{
			valuesMap[matrix[i][column]]++;
		}
	}
	return valuesMap;
}


// Finds single empty space on a row or column and put correct missing value there.
// after filling a space retries to find another one with a do-while loop.
// Returns true if made any modification.
bool fillSingleSpaces(vector<vector<int>> & matrix, vector<vector<int>> & constraints, bool &hasModified){
	bool didChange = false; // flag to keep track of that a change has been done.

	do
	{
		didChange = false; // reseting flag.

		for (unsigned int i = 0; i < matrix.size() ; i++){
			for (unsigned int j = 0; j < matrix.size() ; j++){
				if(matrix[i][j] == 0){
					vector<int> candidateVector = createCandidateVector(matrix,i,j);
					if(candidateVector.size() == 1){
						matrix[i][j] = candidateVector[0];

						if(!mismatchConstraints(matrix,constraints)){
							hasModified = true;
							didChange = true;
						}else{
							matrix[i][j] = 0;

						}

					}
				}
			}
		}

	} while (didChange);

	if(hasModified){
		return true;
	}
	return false;
}


// Check each constraint and if a space can be filled according to it, fills that space.
// Returns true if any modification has been made. False otherwise.
bool fillWithConstraints(vector<vector<int>> & matrix, vector<vector<int>> & constraints, bool & hasModified){

	bool didChange = false; // Flag to track modification.
	int size = matrix.size();

	do
	{
		didChange = false; // reseting flag.

		int x1,y1,x2,y2; // coordinates will be taken from constraint matrix.
		for (unsigned int i = 0; i < constraints.size(); i++){
			x1 = constraints[i][0];
			y1 = constraints[i][1];
			x2 = constraints[i][2];
			y2 = constraints[i][3];

			int bigValue = matrix[x1-1][y1-1]; // since index starts from 0, subtracted 1 from coordinate.
			int smallValue = matrix[x2-1][y2-1];

			if(bigValue == 0 && smallValue == 0){
				if(foundACandidate(matrix,x1-1, y1-1, x2-1, y2-1)){
					hasModified = true;
					didChange = true;
				}

			}
			else if(bigValue == 0){
				// If small value is the secong biggest value, than that X(bigValue) is biggest value matrix can have.
				if(smallValue == size-1 ){
					matrix[x1-1][y1-1] = size;
					hasModified = true;
					didChange = true;
				}else if(hadSingleCandidate(matrix, x1-1, y1-1, smallValue, true)){
					hasModified = true;
					didChange = true;
				}

			}
			else if(smallValue == 0){
				// If bigValue is 2 than that X(smallValue) is 1.
				if(bigValue == 2){
					matrix[x2-1][y2-1] = 1;
					hasModified = true;
					didChange = true;
				}else if(hadSingleCandidate(matrix, x2-1, y2-1, bigValue, false)){
					hasModified = true;
					didChange = true;
				}
			}
		}
	}while(didChange);

	if(hasModified){
		return true;
	}
	return false;
}


// In the case of one of the constraints is zero, tries to find the other one.
// For a X value, tries to find possible candidates by checking its row and column.
// Parameter otherValue is the value our X can be smaller or bigger according to constraints.
// Parameter isBigger is a flag if our X is bigger or not bigger than the otherValue according to constraints.
// If a single possible value is found to replace the X, than modifies matrix and returns true, otherwise returns false.
bool hadSingleCandidate(vector<vector<int>> & matrix, int row, int column, int otherValue, bool isBigger){

	vector<int> rowMap = createValueMap(matrix, row,column, true);
	vector<int> columnMap = createValueMap(matrix,row, column, false);

	vector<int> candidateValues;

	if(isBigger){
		for (unsigned int i = otherValue + 1; i < rowMap.size(); i++){
			if(rowMap[i] == 0 && columnMap[i] == 0){
				candidateValues.push_back(i);
			}
		}
	}else{
		for (unsigned int i = 1 + 1; i < otherValue; i++){
			if(rowMap[i] == 0 && columnMap[i] == 0){
				candidateValues.push_back(i);
			}
		}

	}

	// a single possible value is found.
	if(candidateValues.size() == 1){
		matrix[row][column] = candidateValues[0];
		return true;
	}
	return false;
}


// In the case of both constraints are 0, tries to find a single candidate.
// If update board with a possible value returns true, otherwise false.  
bool foundACandidate(vector<vector<int>> & matrix, int x1, int y1, int x2, int y2){

	vector<int> candidatesForBig = createCandidateVector(matrix,x1,y1);
	vector<int> candidatesForSmall = createCandidateVector(matrix, x2,y2);

	int numOfCandidates = 0;
	int candidateValueForBig = 0;
	int candidateValueForSmall = 0;

	for (unsigned int i = 0; i < candidatesForBig.size() ; i++){
		for (unsigned int j = 0; j < candidatesForSmall.size() ; j++){
			if(candidatesForBig[i] > candidatesForSmall[j]){
				numOfCandidates++;
				candidateValueForBig = candidatesForBig[i];
				candidateValueForSmall = candidatesForSmall[j];

			}

		}

	}

	if(numOfCandidates == 1){
		matrix[x1][y1] = candidateValueForBig;
		matrix[x2][y2] = candidateValueForSmall;
		return true;
	}
	return false;

}


// Creates and returns a vector consisting all possible values can be put into a space on board with row and column values.
vector<int> createCandidateVector(vector<vector<int>> & matrix, int row, int column){
	vector<int> rowMap = createValueMap(matrix, row,column, true);
	vector<int> columnMap = createValueMap(matrix, row,column, false);
	vector<int> candidates;

	for (unsigned int i = 1 ; i < rowMap.size(); i++){
		if(rowMap[i] == 0 && columnMap[i] == 0){
			candidates.push_back(i);
		}
	}

	return candidates;

}


//Takes a 2D vector of int as its single parameter and prints like a matrix
void printMatrix(vector<vector<int>> & matrix ){
	for (unsigned int i = 0; i < matrix.size() ; i++){
		for (unsigned int j = 0; j < matrix[i].size() ; j++){
			cout << matrix[i][j] << " ";
		}
		cout << endl;
	}
	cout << endl;
}



int main(){

	vector<vector<int>> board;
	vector<vector<int>> constraints;

	string boardFile;
	string constraintFile;


	bool hasBiggerValue = false; // flag to control if board has a big value than size.
	int biggerBoardValue = 0;
	int biggerConstValue = 0;
	bool hasModified = true; // flag used while trying to solve the puzzle.

	// taking file names as input.
	cout << "Enter the board file:";
	cin >> boardFile;
	cout << "Enter the constraint file:";
	cin >> constraintFile;

	bool isCorrectBoardSize = takeBoard(board,boardFile, hasBiggerValue, biggerBoardValue);
	bool isCorrectConstSize = takeConstraints(constraints, constraintFile, hasBiggerValue, biggerConstValue, board.size());


	if(hasBiggerValue){
		cout << "Value " << biggerBoardValue <<" is out of matrix size" << endl;
	}
	else if(!isCorrectBoardSize ){
		cout << "File contains more lines than size" << endl;
	}
	else if(!isCorrectConstSize){
		cout << "There are more constraints then previously stated" << endl;
	}
	else if(checkRepeatOnRow(board)){
		cout << "There is a value repeating in same row" << endl;
	}
	else if(checkRepeatOnColumn(board)){
		cout << "There is a value repeating in same column" << endl;
	}
	else if(mismatchConstraints(board,constraints)){
		cout << "Constraints do not match board" << endl;
	}

	else{
		solvePuzzle(board,constraints,hasModified);
	}


	return 0;
}
