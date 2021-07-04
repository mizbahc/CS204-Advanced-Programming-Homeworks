#ifndef _BINARYTREE_H
#define _BINARYTREE_H

#include <iostream>
#include "Stack.h"


using namespace std;

//Making friend class also template for compiler to parse it.
template <class T> class Iterator;

//BinaryTree is a template class which has one generic type named T.
template <class T>
class BinaryTree
{
 public:
  //Constructor
  BinaryTree();
  //Copy Constructor
  BinaryTree(const BinaryTree&);
  //Destructor
  ~BinaryTree();
  
  // Insertion method
  void insert(T);
  TreeNode<T>* getRoot();
  
  //operators
  const BinaryTree& operator= (const BinaryTree& rhs);
  bool operator== (const BinaryTree& rhs);
  bool operator!= (const BinaryTree& rhs);
  void operator+= (const BinaryTree& rhs);
  BinaryTree operator+ (const BinaryTree& rhs);
  void operator+= (T number);
  BinaryTree operator+(T number);
  
  //Since operator + is a free friend function, we made it templated. Compiler will parse it that way. 
  template <class T>
  friend BinaryTree<T> operator+(T number, const BinaryTree<T>&);

  //free function operator <<
  template <class T>
  friend ostream& operator<<(ostream&, const BinaryTree<T>&);

 private:
  //The root of the tree
  TreeNode<T>* root;


  friend class Iterator<T>;
};

template <class T>
class Iterator{
 public:
  //Constructor
  Iterator();

  void Init(const BinaryTree<T> & );
  bool hasNext();
  TreeNode<T>* Next(); //Actually the same with InOrderNext()
  TreeNode<T>* InOrderNext(); 
  TreeNode<T>* PreOrderNext(); 
  
 private:
  TreeNode<T>* myCurrent;
  Stack<T>* stack;
  Stack<T>* stack2;
};

//Including here so compiler can instanitate with member function definitions it needed.
#include "BinaryTree.cpp"

#endif
