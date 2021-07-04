
#include <iostream>
#include <vector>
#include <fstream>
#include <cctype>
#include "strutils.h"

#include <chrono>

using namespace std;


struct metroStop {
	string metroStopName;
	metroStop *left;
	metroStop *right; 

	metroStop(string _metroStopName, metroStop * _left, metroStop *_right)
		:metroStopName(_metroStopName),left(_left), right(_right)
	{}

};


struct metroLine {
	string metroNumber;
	metroLine *next;  
	metroStop *metroStops;
	vector<double>costs;

	metroLine(string _metroNumber, metroLine * _next, metroStop * _metroStops, vector<double> _costs)
		:metroNumber(_metroNumber), next(_next), metroStops(_metroStops), costs(_costs)
	{}
};


void processMainMenu(metroLine * head, int maxHops);
void printmetroLinesDetailed(metroLine * head);
bool consistencyCheck(metroLine * head);
void takeMetroLines(metroLine * head, string filename);
void takeCosts(metroLine * head, string filename);
bool addmetroStop(metroLine * head);
metroLine * goToMetroLine(metroLine * head, string lineName);
void showSingleLineInfo(metroLine * line);
int alreadyHas(metroLine * line, string stopName, metroStop* &prevStop);
bool deletemetroStop(metroLine * head);
void updateTable(metroLine * head,metroLine * toBeChangedLine, metroStop* deletedStop,int indexOfDeleted);
bool pathfinder(metroLine * head, int maxHops);
int findLineAndStop(metroLine * head,metroLine * & startingLine,metroStop* &startingStop,string stopName);
bool formPath(bool & foundPath,metroLine * tmpHead,int maxHops,metroLine * startingLine,metroStop *startingStop,string destinationName,vector<string> &visitedLines,vector<vector<string>> & foundPathMatrix,int &row,int & currentHops,double & cost,int index,string direction);
int findIntersections(metroLine * tmpHead,vector<string> visitedLines,string currentStopName,metroLine * & newLine,metroStop* &newStop);



bool consistencyCheck(metroLine * head) {
	metroLine* currbl = head;
	while(currbl) {
		metroStop* currbs = currbl->metroStops;
		while(currbs) {
			metroStop* rightbs = currbs->right;
			if(rightbs && rightbs->left != currbs) {
				cout << "ýnconsistency for " 
					<< currbl->metroNumber 
					<< " " 
					<< currbs->metroStopName << endl;
				return false;
			}
			currbs = currbs->right;
		}
		currbl = currbl->next;
	}
	return true;
}

void printMainMenu() {
	cout << endl;
	cout <<"I***********************************************I"<<endl
		<<"I               0 - EXIT PROGRAM                I"<<endl
		<<"I               1 - PRINT LINES                 I"<<endl
		<<"I               2 - ADD METRO STOP              I"<<endl
		<<"I               3 - DELETE METRO STOP           I"<<endl
		<<"I               4 - PATH FINDER                 I"<<endl
		<<"I***********************************************I"<<endl
		<<">>";
	cout << endl;
}  


void processMainMenu(metroLine * head, int maxHops) {
	char input;
	do{
		if(!consistencyCheck(head)) {
			cout << "There are inconsistencies. Exit." << endl; 
			return;
		}

		printMainMenu();
		cout << "Please enter your option " << endl;
		cin >> input;
		string rest;
		getline(cin,rest); // in cases of invalid input is more than 1 character.

		switch (input) {
		case '0':
			cout << "Thanks for using our software" << endl;
			return;
		case '1':
			printmetroLinesDetailed(head);
			break;
		case '2':
			addmetroStop(head);
			break;
		case '3':
			deletemetroStop(head);
			break;
		case '4':
			pathfinder(head, maxHops);
			break;
		default:
			cout << "Invalid option: please enter again" << endl;
		}
	} while(true);
}


void printmetroLinesDetailed(metroLine * head){
	metroLine* currBL = head;
	while(currBL) {
		cout << currBL->metroNumber << ": ";
		metroStop* currBS = currBL->metroStops;
		int currentIndex = 0;
		while(currBS) {
			cout << currBS->metroStopName  ;
			if(currentIndex < currBL->costs.size()){
				cout << "<-";
				cout << currBL->costs[currentIndex++] << "->";
			}
			currBS = currBS->right;
		}

		cout << endl;
		currBL = currBL->next;
	}
}

// Takes lines and creates linked lists from them.
void takeMetroLines(metroLine * head, string filename){
	ifstream input;
	string line;
	metroLine * tmpHead = head;
	metroLine * lastLine = head;

	input.open(filename.c_str());

	if(input.fail()){
		cout << "Could not open the file." << endl;
	}else {

		while(getline(input,line)){
			// some files can have empty lines, maybe at the end. That if block prevents that confusion.
			if(line.length() > 0){ 

				line = line.substr(0, line.length()-1);
				int index = line.find(",");
				string metroLineNumber = line.substr(0,index);

				tmpHead ->metroNumber = metroLineNumber;
				tmpHead -> metroStops = new metroStop("",nullptr,nullptr);

				metroStop * tmpStopHead = tmpHead->metroStops;

				while(index != string::npos){
					int strt = index +1 ;
					index = line.find(",",index+1);
					string stopName;
					if(index != string::npos){
						stopName = line.substr(strt, index - strt);
						tmpStopHead->metroStopName = stopName;
						tmpStopHead->right = new metroStop("",tmpStopHead, nullptr);
						tmpStopHead = tmpStopHead ->right;

					}else{
						stopName = line.substr(strt);
						tmpStopHead->metroStopName = stopName;
					}
				}
			}

			tmpHead -> next = new metroLine("",nullptr, nullptr, vector<double>());
			lastLine = tmpHead;
			tmpHead = tmpHead -> next;

		}
	}
	lastLine -> next = nullptr;
	delete tmpHead;
	tmpHead = nullptr;
}

// Adds costs to the lines
void takeCosts(metroLine * head, string filename){

	ifstream input;
	string line;
	metroLine * tmpHead = head;

	input.open(filename.c_str());

	if(input.fail()){
		cout << "Could not open the file." << endl;
	}else {

		while(getline(input,line)){
			// some files can have empty lines, maybe at the end. That if block prevents that confusion.
			if(line.length() > 0){ 
				int index = line.find(",");
				string metroLineNumber = line.substr(0,index);

				while(index != string::npos){
					int strt = index +1 ;
					index = line.find(",",index+1);

					string costStr;
					if(index != string::npos){
						costStr = line.substr(strt, index - strt);
						tmpHead ->costs.push_back(atoi(costStr));

					}else{
						costStr = line.substr(strt);
						tmpHead ->costs.push_back(atoi(costStr));
					}
				}
			}

			tmpHead = tmpHead -> next;
		}
	}
}


bool addmetroStop(metroLine * head){
	string lineName;
	cout << "Enter the name of the metro line to insert a new metro stop (0 for main menu)" << endl;
	cin >> lineName;
	ToLower(lineName);

	metroLine * toBeAddedLine = goToMetroLine(head,lineName);

	if(toBeAddedLine == nullptr){
		cout << "Metro line cannot be found. Going back to previous menu." << endl;
		return false;
	}else{
		cout << "The metro line information is shown below" << endl;
		showSingleLineInfo(toBeAddedLine);
		cout << "Enter the name of the new metro stop" << endl;
		string stopName;
		cin >> stopName;
		metroStop * prevStop = nullptr;

		if(alreadyHas(toBeAddedLine, stopName,prevStop) >= 0){
			cout << stopName << " Metro stop already exists. Going back to previous menu." << endl;
			return false;
		}

		cout << "Enter the name of the previous metro stop for the new one (0 to put the new one as the first metro stop)" << endl;
		string prevStopName;
		double cost1, cost2;
		cin >> prevStopName;
		cout << "Enter new cost 1" << endl;
		cin >> cost1;
		cout << "Enter new cost 2" << endl;
		cin >> cost2;

		if(prevStopName == "0"){
			//add to the beginning
			metroStop * addedStop = new metroStop(stopName,nullptr,toBeAddedLine->metroStops);
			toBeAddedLine->metroStops->left = addedStop;
			toBeAddedLine->metroStops = addedStop;

			(toBeAddedLine->costs).insert((toBeAddedLine->costs).begin(),cost1);
			showSingleLineInfo(toBeAddedLine);
			return true;
		}


		int indexOfPrev = alreadyHas(toBeAddedLine, prevStopName, prevStop); 
		while(indexOfPrev < 0){
			cout << "Metro stop does not exist. Typo? Enter again (0 for main menu)" << endl;
			cin >> prevStopName;
			indexOfPrev = alreadyHas(toBeAddedLine, prevStopName,prevStop); 
			if(prevStopName == "0"){
				return false;
			}
		}

		if(indexOfPrev == toBeAddedLine->costs.size()){
			metroStop * addedStop = new metroStop(stopName,prevStop,nullptr);
			prevStop->right = addedStop;

			(toBeAddedLine->costs).insert((toBeAddedLine->costs).begin()+indexOfPrev,cost1);
			showSingleLineInfo(toBeAddedLine);

			return true;
		}

		metroStop * addedStop = new metroStop(stopName,prevStop,prevStop->right);
		prevStop->right->left = addedStop;
		prevStop->right = addedStop;

		(toBeAddedLine->costs).erase(toBeAddedLine->costs.begin()+indexOfPrev);

		(toBeAddedLine->costs).insert((toBeAddedLine->costs).begin()+indexOfPrev,cost1);
		(toBeAddedLine->costs).insert((toBeAddedLine->costs).begin()+indexOfPrev+1,cost2);
		showSingleLineInfo(toBeAddedLine);
		return true;

	}


}

// Helper method to find the line with the given name. So adding new stop to that line becomes more easier.
metroLine * goToMetroLine(metroLine * head, string lineName){
	metroLine * tmpHead = head;

	while(tmpHead){
		string metroNum = tmpHead ->metroNumber;
		if(LowerString(metroNum) == lineName){
			return tmpHead;
		}
		tmpHead = tmpHead -> next;
	}

	return nullptr;
}

// Printing line with its stops after adding or deleting stops.
void showSingleLineInfo(metroLine * line){
	metroStop * currBS = line->metroStops;
	cout << line->metroNumber << ": ";
	int currentIndex = 0;
	while(currBS) {
		cout << currBS->metroStopName  ;
		if(currentIndex < line->costs.size()){
			cout << "<-";
			cout << line->costs[currentIndex++] << "->";
		}
		currBS = currBS->right;
	}

	cout << endl;
}

// Checks whether the line has given stop or not.
int alreadyHas(metroLine * line, string stopName, metroStop* &prevStop){
	metroStop * currBS = line->metroStops;
	string key = stopName;
	ToLower(key);
	int index = 0;
	while(currBS) {
		string stop = currBS->metroStopName;
		ToLower(stop);
		if(key == stop){
			prevStop = currBS;
			return index;
		}
		index++;
		currBS = currBS->right;
	}
	return -1;
}


bool deletemetroStop(metroLine * head){
	string lineName;
	cout << "Enter the name of the metro line to delete a new metro stop (0 for main menu)" << endl;
	cin >> lineName;
	ToLower(lineName);

	metroLine * toBeChangedLine = goToMetroLine(head,lineName);

	if(toBeChangedLine == nullptr){
		cout << "Metro line cannot be found. Going back to previous menu." << endl;
		return false;
	}else{
		cout << "The metro line information is shown below" << endl;
		showSingleLineInfo(toBeChangedLine);

		cout << "Enter the name of the metro stop to delete (0 for main menu)" << endl;
		string stopName;
		cin >> stopName;

		if( stopName == "0"){
			return false;
		}

		metroStop * deletedStop = nullptr;
		int indexOfDeleted = alreadyHas(toBeChangedLine, stopName, deletedStop); 

		while(indexOfDeleted < 0){
			cout << "Metro stop cannot be found. Enter the name of the metro stop to delete (0 for main menu)" << endl;
			cin >> stopName;
			indexOfDeleted = alreadyHas(toBeChangedLine, stopName, deletedStop); 
			if(stopName == "0"){
				return false;
			}
		}
		updateTable(head,toBeChangedLine,deletedStop,indexOfDeleted);
		return true;
	}
}

// After deleting stops, update lines and costs.
void updateTable(metroLine * head,metroLine * toBeChangedLine, metroStop* deletedStop,int indexOfDeleted){
	metroLine * tmpHead = head;

	if(toBeChangedLine->costs.size() == 0){
		delete deletedStop;

		if(toBeChangedLine == head){
			head = head->next;
			delete toBeChangedLine;
		}else{
			while(tmpHead ->next != toBeChangedLine){
				tmpHead = tmpHead->next;
			}
			tmpHead->next = toBeChangedLine->next;
			delete toBeChangedLine;
		}
	}

	else{
		metroStop * tmpStop = toBeChangedLine->metroStops;
		if(deletedStop == tmpStop){
			toBeChangedLine->metroStops = tmpStop->right;
			toBeChangedLine->metroStops->left = nullptr;
			delete tmpStop;
			toBeChangedLine->costs.erase(toBeChangedLine->costs.begin());
		}else{
			while(tmpStop->right != deletedStop){
				tmpStop = tmpStop->right;
			}

			tmpStop->right = deletedStop->right;
			deletedStop->right->left = tmpStop;
			delete deletedStop;

			if(indexOfDeleted == toBeChangedLine->costs.size()){
				toBeChangedLine->costs.erase(toBeChangedLine->costs.begin()+indexOfDeleted -1);	
			}else{

				double total = 0;
				total += toBeChangedLine->costs[indexOfDeleted-1];
				total += toBeChangedLine->costs[indexOfDeleted];
				toBeChangedLine->costs.erase(toBeChangedLine->costs.begin()+indexOfDeleted -1);	
				toBeChangedLine->costs.erase(toBeChangedLine->costs.begin()+indexOfDeleted);
				toBeChangedLine->costs.insert((toBeChangedLine->costs).begin()+indexOfDeleted -1,total);
			}
		}

		showSingleLineInfo(toBeChangedLine);
	}
}


bool pathfinder(metroLine * head, int maxHops){


	metroLine * tmpHead = head;
	string startingStopStr, destinationStopStr;
	cout << "Where are you now?" << endl;
	cin >> startingStopStr;
	cout << "Where do you want to go?" << endl;
	cin >> destinationStopStr;

	metroLine * startingLine = nullptr;
	metroStop * startingStop = nullptr;

	int indexForDest = findLineAndStop(head,startingLine,startingStop,destinationStopStr);
	int indexForStart = findLineAndStop(head,startingLine,startingStop,startingStopStr);

	if(indexForStart < 0 || indexForDest < 0 ){
		cout << "Metro stop does not exist in the table. Going back to previous menu." << endl;
		return false;
	}

	auto start = chrono::steady_clock::now();
	

	double minCost = double(INT_MAX);
	vector<vector<string>> finalPathMatrix;
	bool isSolved = false;
	int finalHops = 0;

	metroLine* currBL = head;
	while(currBL) {
		vector<string> visitedLines;
		vector<vector<string>> foundPathMatrix;
		foundPathMatrix.push_back(vector<string>());
		
		visitedLines.push_back(startingLine->metroNumber);
		int row = 0;
		int currentHops = 0;
		double cost = 0;
		string direction = "";

		isSolved = false;

		metroStop* currBS = currBL->metroStops;
		int currentIndex = 0;
		while(currBS) {
			string currBSName = currBS->metroStopName;
			ToLower(currBSName);
			string key = startingStopStr;
			ToLower(key);

			if(currBSName == key){
				startingLine = currBL;
				startingStop = currBS;
				indexForStart = currentIndex;

				foundPathMatrix[0].push_back(startingLine->metroNumber + ": ");

				if(formPath(isSolved,tmpHead,maxHops,startingLine,startingStop,destinationStopStr,visitedLines,foundPathMatrix,row,currentHops,cost,indexForStart,direction)){
					if(cost < minCost){
						minCost = cost;
						finalPathMatrix = foundPathMatrix;

					}
					finalHops = currentHops;
					isSolved = true;
				}

				break;
			}
			currBS = currBS->right;
			currentIndex++;
		}
		currBL = currBL->next;
	}
	auto end = chrono::steady_clock::now();
	chrono::duration<double> totalTime = end - start;

	 

	if(minCost < INT_MAX ){
		cout << "Best cost path:" << endl;
		for (unsigned int i = 0; i < finalPathMatrix.size() ; i++){
			for (unsigned int j = 0; j < finalPathMatrix[i].size() ; j++){
				cout << finalPathMatrix[i][j] << " ";
			}
			cout << endl;
		}

		cout << "Hops: " << finalHops << endl;
		cout << "Cost: " << minCost << endl;
		cout << "Elapsed time: " <<totalTime.count() << endl;


		return true;

	}else{
		cout << "No path found between "<< startingStopStr<<" and "<< destinationStopStr  << endl;
		cout << "Elapsed time: " <<totalTime.count() << endl;
		return false;
	}


}


// Helper method to go to the kine and stop with given names.
int findLineAndStop(metroLine * head,metroLine * & startingLine,metroStop* & startingStop,string stopName){
	metroLine* currBL = head;
	startingLine = nullptr;
	startingStop= nullptr;

	while(currBL) {
		metroStop* currBS = currBL->metroStops;
		int currentIndex = 0;
		while(currBS) {
			string currBSName = currBS->metroStopName;
			ToLower(currBSName);
			string key = stopName;
			ToLower(key);

			if(currBSName == key){
				startingLine = currBL;
				startingStop = currBS;
				return currentIndex;
			}

			currentIndex++;
			currBS = currBS->right;
		}
		currBL = currBL->next;
	}

	return -1;
}


// Creates a path from starting point to destination in a recursive way to be used in pathfinder method.
// First checks the right side of the starting point. And if any transfer is possible goes to that new line. 
// Then checks the left side of it.
// Returns true if it finds the path.
bool formPath(bool & foundPath,metroLine * tmpHead,int maxHops,metroLine * startingLine,metroStop *startingStop,string destinationName,vector<string> &visitedLines,vector<vector<string>> & foundPathMatrix,int &row,int & currentHops,double & cost,int indexOfCost,string direction){

	//visitedLines.push_back(startingLine->metroNumber);
	foundPathMatrix[row].push_back(startingStop->metroStopName);
	double currentCost= 0;

	if(direction != "right" && startingStop->right){
		string nextStop = startingStop->right->metroStopName;
		ToLower(nextStop);
		currentCost = startingLine->costs[indexOfCost];
		cost += currentCost;
		//indexOfCost++;
		if(nextStop == LowerString(destinationName)){
			foundPath = true;
			foundPathMatrix[row].push_back(startingStop->right->metroStopName);
			return true;

		}else{
			formPath(foundPath,tmpHead,maxHops,startingLine,startingStop->right,destinationName,visitedLines,foundPathMatrix,row,currentHops,cost,indexOfCost+1,"left");
		}
	} 

	if(!foundPath && direction != "left" && startingStop->left){
		string nextStop = startingStop->left->metroStopName;
		ToLower(nextStop);

		currentCost = startingLine->costs[indexOfCost-1];
		cost += currentCost;
		//indexOfCost--;

		if(nextStop == LowerString(destinationName)){
			foundPath = true;
			foundPathMatrix[row].push_back(startingStop->left->metroStopName);

			return true;
		}else{
			formPath(foundPath, tmpHead,maxHops,startingLine,startingStop->left,destinationName,visitedLines,foundPathMatrix,row,currentHops,cost,indexOfCost-1,"right");
		}
	}

	if(!foundPath && currentHops < maxHops){
		currentHops++;

		metroLine * newLine = nullptr;
		metroStop * newStop = nullptr;
		int newIndex = findIntersections(tmpHead,visitedLines,startingStop->metroStopName,newLine,newStop);



		while(newIndex >= 0){
			row++;
			foundPathMatrix.push_back(vector<string>());
			foundPathMatrix[row].push_back(newLine->metroNumber + ": ");
			visitedLines.push_back(newLine->metroNumber);

			if(formPath(foundPath,tmpHead,maxHops,newLine,newStop,destinationName,visitedLines,foundPathMatrix,row,currentHops,cost,newIndex,"")){
				foundPath = true;
				return true;
			}else{
				foundPathMatrix.pop_back();
				row--;
				
				
			}

			newIndex = findIntersections(tmpHead,visitedLines,startingStop->metroStopName,newLine,newStop);
		}

		if(!foundPath){
			currentHops--;
		}

	}

	if(!foundPath && direction != "right" && direction != ""){
		currentCost = startingLine->costs[indexOfCost-1];
		indexOfCost--;


	} else if(!foundPath && direction != "left" && direction != ""){
		currentCost = startingLine->costs[indexOfCost];
		indexOfCost++;

	}

	if(!foundPath){
		cost -= currentCost;
		foundPathMatrix[row].pop_back();
	}

	return foundPath;
}


//Helper method to find possible transfer in a stop.
int findIntersections(metroLine * tmpHead,vector<string> visitedLines,string currentStopName,metroLine * & newLine,metroStop* &newStop){
	metroLine* currBL = tmpHead;
	int index = -1;

	while(currBL) {
		bool lineVisited = false;
		string currBLName = currBL->metroNumber;
		ToLower(currBLName);

		for (int i = 0; i < visitedLines.size(); i++)
		{
			if(currBLName == LowerString(visitedLines[i])){
				lineVisited = true;
			}
		}

		if(!lineVisited){
			metroStop* currBS = currBL->metroStops;
			int currentIndex = 0;
			while(currBS) {
				string currBSName = currBS->metroStopName;
				ToLower(currBSName);
				string key = currentStopName;
				ToLower(key);

				if(currBSName == key){
					newLine = currBL;
					newStop = currBS;
					index = currentIndex;
					break;
				}
				currBS = currBS->right;
				currentIndex++;
			}

		}

		currBL = currBL->next;
	}	

	return index;

}



int main() {
	metroLine * head = new metroLine("",nullptr,nullptr,vector<double>());
	string metroLineFile, costsFile;
	int maxHops;

	cout << "Enter the name of the metro line data file:" << endl;
	cin >> metroLineFile;
	cout << "Enter the name of costs file:" << endl;
	cin >> costsFile;
	cout << "Enter maximum number of hops:" << endl;
	cin >> maxHops;

	takeMetroLines(head,metroLineFile);
	takeCosts(head,costsFile);
	
	processMainMenu(head,maxHops);

}
