#ifndef LISTSTACK
#define LISTSTACK

template <class T>
class ListStack {
 public:
  ListStack() : tos(0), list(0) {}
  void attachList(TinyList<T> * _list) {list = _list; tos = 0;}
  void push(T obj);
  T pop();
  unsigned int size() {return tos;}
 protected:
  unsigned int tos;
  TinyList<T> * list;
};

template <class T>
void ListStack<T>::push(T obj) {
  list->put(tos, obj);
  tos++;
}

template <class T>
T ListStack<T>::pop() {
  if (tos != 0) tos--;
  T ret = list->get(tos);
  list->remove(tos);
  return ret;
}


#ifdef DMALLOC
#include <dmalloc.h>
#endif
#endif
