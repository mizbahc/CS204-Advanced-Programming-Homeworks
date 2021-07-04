

#include "BinaryTree.h"
#include <iostream>
#include <string>
#include <vector>

using namespace std;



//Constructor
BinaryTree::BinaryTree(){
	root = nullptr;

}

//Copy Constructor
BinaryTree::BinaryTree(const BinaryTree& rhs){
	//TODO: Implement deep copy constructor
	//You may make use of Iterator class

	if(rhs.root){
		Iterator i;
		i.Init(rhs);
		root = nullptr;
		while(i.hasNext()){
			int val = i.Next()->value;
			(*this).insert(val);
		}

	}else{
		root = nullptr;
	}

}

//Destructor
BinaryTree::~BinaryTree(){
	//TODO: Implement destructor

	Iterator i;
	i.Init(*this);

	while(i.hasNext()){
		delete i.Next();
	}

	root = nullptr;

}

void BinaryTree::insert(int num) {
	//TODO: Implement insert function for BinaryTree
	TreeNode * node = new TreeNode();
	node ->value = num;
	TreeNode** tmp = &root;

	while(*tmp){
		if((*tmp)->value == num){
			cout << "Duplicate value found in tree."<< endl;
			return;
		}else if((*tmp)->value > num){
			tmp = & ((*tmp)->left);
		}
		else{
			tmp = &((*tmp)->right);
		}
	}

	*tmp = node;
}

//Operators
//Give the implementations of operators in here either as free function or member function

// operator <<
ostream & operator << (ostream & os, const BinaryTree & tree){
	Iterator i;
	i.Init(tree);
	while (i.hasNext())
		cout << i.Next()->value << " ";

	return os;
}


// operator =
const BinaryTree & BinaryTree::operator = (const BinaryTree & rhs ){

	if(this != &rhs){

		Iterator i;
		i.Init(*this);

		while(i.hasNext()){
			delete i.Next();
		}

		root = nullptr;

		Iterator i2;
		i2.Init(rhs);

		while(i2.hasNext()){
			int val = i2.Next()->value;
			(*this).insert(val);
		}

		return *this;
	}

	return *this;

}


// operator += 
BinaryTree & BinaryTree::operator +=(const BinaryTree & rhs ){
	Iterator i;
	i.Init(rhs);

	while(i.hasNext()){
		int val = i.Next()->value;
		(*this).insert(val);
	}

	return *this;
}

// operator +=
BinaryTree & BinaryTree::operator +=(const int & rhs ){
	(*this).insert(rhs);
	return *this;
}





// operator + ---> tree + tree
BinaryTree BinaryTree::operator +(const BinaryTree & rhs ) {

	BinaryTree result(*this);	
	result += rhs;		
	return result;

}


// operator +
BinaryTree operator +(const BinaryTree & lhs,const int & val ) {

	BinaryTree result(lhs);	
	result += val;		
	return result;
}


// operator +
BinaryTree  operator + (const int & val, const BinaryTree & tree ){

	BinaryTree result(tree);	
	result += val;		
	return result;
}

// operator ==
bool BinaryTree::operator == (const BinaryTree & lhs ){

	vector<int> valuesOfCurrent;
	vector<int> valuesOfLhs;

	Iterator i;
	i.Init(*this);

	while(i.hasNext()){
		int val = i.Next()->value;
		valuesOfCurrent.push_back(val);
	}

	Iterator i2;
	i2.Init(lhs);

	while(i2.hasNext()){
		int val = i2.Next()->value;
		valuesOfLhs.push_back(val);
	}

	if(valuesOfCurrent.size() == 0){
		return false;
	}

	if(valuesOfCurrent.size() != valuesOfLhs.size()){
		return false;
	}
	for (unsigned int i = 0; i < valuesOfCurrent.size(); i++)
	{
		if(valuesOfCurrent[i] != valuesOfLhs[i]){
			return false;
		}
	}

	return true;

}

// operator !=
bool BinaryTree::operator != (const BinaryTree & lhs ){

	return !((*this)==lhs);

}





/*
*  ITERATOR CLASS
*/

//Constructor
Iterator::Iterator()
	:myCurrent(nullptr), stack(nullptr)
{}

void Iterator::Init(const BinaryTree &tree)
{
	myCurrent = tree.root;
	stack = new Stack();
}

//Returns true if tree has more nodes to explore
bool Iterator::hasNext()
{
	//TODO: Implement a function to see if there is more nodes to iterate over.


	if(myCurrent== nullptr && (*stack).isEmpty()){
		return false;
	}

	return true;
}

//Next node to explore
TreeNode* Iterator::Next()
{

	//TODO: Implement next function of the iterator
	//Note that it returns the next element

	if((*stack).isEmpty()){
		while(myCurrent){
			(*stack).push(myCurrent);
			myCurrent = myCurrent->left;
		}
	}
	TreeNode* node;
	node = (*stack).pop();
	myCurrent = node->right;


	while(myCurrent){
		(*stack).push(myCurrent);
		myCurrent = myCurrent->left;
	}

	return node;
}


