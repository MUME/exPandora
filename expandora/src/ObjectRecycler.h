//#pragma once
#ifndef OBJECTRECYCLER
#define OBJECTRECYCLER
#include "TinyList.h"


template <class T>
class ObjectRecycler : protected TinyList<T *> {
 public:
  ObjectRecycler<T>();
  void deactivate(T * obj);
  T * activate();
 protected:
  unsigned int pos;
};

template <class T>
ObjectRecycler<T>::ObjectRecycler<T>() {
  pos = 0;
}

template <class T>
void ObjectRecycler<T>::deactivate(T * obj) {
  //printf("deactivating ... old queue size: %i\n", size());

  if (pos > 127) delete(obj);
  else {
    obj->clear();
    TinyList<T *>::put(pos, obj);
    pos++;
  }

  //printf("deactivating ... new queue size: %i\n", size());
}

template <class T>
T * ObjectRecycler<T>::activate() {
  //printf("acitvating ... old queue size: %i\n", size());
  if (pos != 0) {
    pos--;
    T * ret = TinyList<T*>::get(pos);
    TinyList<T*>::remove(pos); 
    //printf("acitvating ... new queue size: %i\n", size());
    return ret;
  }
  else return new T();
}

#endif

#ifdef DMALLOC
#include <dmalloc.h>
#endif
