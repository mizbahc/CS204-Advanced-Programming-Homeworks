#ifndef _STACK_H
#define _STACK_H

#include <iostream>

//TreeNode is a template struct with one generic type.
template <class T>
struct TreeNode
{
  T value;
  TreeNode *left;
  TreeNode *right;

  TreeNode(){
    left = nullptr;
    right = nullptr;
  }
};

//StackNode is a template struct with one generic type.
template <class T>
struct StackNode
{
  TreeNode<T>* value;
  StackNode *next;
};

//Stack is a template class with one generic type.
template <class T>
class Stack
{
private:
  StackNode<T> *top;
  StackNode<T>* GetTopPointer(Stack myStack);
  
public:
  Stack(void);
  void push(TreeNode<T>* );
  TreeNode<T>* pop();
  bool isEmpty();
};

//Including here so compiler can instanitate with member function definitions it needed.
#include "Stack.cpp"

#endif
