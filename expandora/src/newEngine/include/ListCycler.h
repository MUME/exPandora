//#pragma once
#ifndef LISTCYCLER
#define LISTCYCLER
#include "TinyList.h"

template <class T>
class ListCycler {
 public:
  ListCycler<T>() : pos(0) {;} 
  virtual void attachList(TinyList<T> * _list) {list = _list; pos = list->size();}
  virtual ~ListCycler<T>() {}
  virtual T next();
  virtual T prev();
  virtual T current();
  virtual unsigned int getPos() {return pos;}
  virtual void reset() {pos = list->size();}

 protected:
  TinyList<T> * list;
  unsigned int pos;
};


template <class T>
T ListCycler<T>::next() {
  if (pos == list->size()) pos = 0;
  else pos++;
  return list->get(pos);
}

template <class T>
T ListCycler<T>::prev() {
  if (pos == 0) pos = list->size();
  else pos--;
  return list->get(pos);
}		

template <class T>	
T ListCycler<T>::current() {
  return list->get(pos);
}
   
#endif
