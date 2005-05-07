//#pragma once
#ifndef TINYLIST
#define TINYLIST


template <class T>
class TinyList {
	public:
		TinyList();
		virtual ~TinyList();
		virtual T get(unsigned int pos);
		virtual void put(unsigned int pos, T object);
		virtual void remove(unsigned int pos);
		virtual unsigned int size();

	protected:
		T * list;
		unsigned int listSize;
};



/**
 * extremely shrinked array list implementation to be used for each room
 * because we have so many rooms and want to search them really fast, we
 *	- allocate as little memory as possible
 *	- allow only 128 elements (1 per character)
 *	- only need 3 lines to access an element
 */

template <class T>
unsigned int TinyList<T>::size() {
  return listSize;
}


template <class T>
TinyList<T>::TinyList() {
  listSize = 0;
  list = 0;
}

template <class T>
TinyList<T>::~TinyList() {
  if (list) delete[] list;
}


template <class T>
T TinyList<T>::get(unsigned int c) {
	if (c >= listSize) return 0;
	else return list[c];
}

template <class T>
void TinyList<T>::put(unsigned int c, T object) {	
	if (c >= listSize) {
		unsigned int i;
		T * nlist = new T[c+2];
		for (i = 0; i < listSize; i++) nlist[i] = list[i];
		for (; i < c; i++) nlist[i] = 0;
		if (list) delete[] list;
		list = nlist;
		listSize = c+1;
		list[listSize] = 0;
	}
	list[c] = object;
}


template <class T>
void TinyList<T>::remove(unsigned int c) {
	if (c < listSize) list[c] = 0;
}
#endif

#ifdef DMALLOC
#include <dmalloc.h>
#endif
