#ifndef _BINARYTREE_H
#define _BINARYTREE_H

#include <iostream>
#include "Stack.h"

using namespace std;

class BinaryTree
{
public:

	void printRecursive(TreeNode* root);
	TreeNode* BinaryTree::getRoot();

	//Constructor
	BinaryTree();
	//Copy Constructor - TODO in .cpp file
	BinaryTree(const BinaryTree&);
	//Destructor
	~BinaryTree();

	// Insertion method
	void insert(int);

	// Operators
	const  BinaryTree & BinaryTree::operator = (const BinaryTree & rhs );

	BinaryTree BinaryTree::operator +(const BinaryTree & rhs );

	BinaryTree & BinaryTree::operator +=(const int & rhs );
	BinaryTree & BinaryTree::operator +=(const BinaryTree & rhs );

	bool BinaryTree::operator ==(const BinaryTree & lhs );
	bool BinaryTree::operator != (const BinaryTree & lhs );

private:
	//The root of the tree
	TreeNode* root;

	friend class Iterator;
};

class Iterator{
public:
	//Constructor
	Iterator();

	void Init(const BinaryTree& );
	bool hasNext();
	TreeNode* Next();

private:
	TreeNode* myCurrent;
	Stack* stack;
};

// Operators- free ones
ostream & operator << (ostream & os, const BinaryTree & tree);
BinaryTree operator +(const BinaryTree & lhs,const int & val );
BinaryTree  operator +(const int & val,const BinaryTree & tree ) ;


#endif
