#pragma once
#ifndef TINYLIST
#define TINYLIST


template <class T>
class TinyList {
	public:
		TinyList();
		~TinyList();
		T * get(char pos);
		void put(char pos, T * object);
		void remove(char pos);
		
	private:
		T ** list;
		char listSize;
};



/**
 * extremely shrinked array list implementation to be used for each room
 * because we have so many rooms and want to search them really fast, we
 *	- allocate as little memory as possible
 *	- allow only 128 elements (1 per character)
 *	- only need 3 lines to access an element
 */



template <class T>
TinyList<T>::TinyList() {
	list = new(T *)[1];
	listSize = 1;
	list[0] = 0;
}

template <class T>
TinyList<T>::~TinyList() {
	delete[] list;
}


template <class T>
T * TinyList<T>::get(char c) {
	if (c < 0) c+=128;
	if (c >= listSize) return 0;
	else return list[c];
}

template <class T>
void TinyList<T>::put(char c, T * object) {	
	if (c < 0) c+=128;
	if (c >= listSize) {
		char i;
		T ** nlist = new T *[c+1];
		for (i = 0; i < listSize; i++) nlist[i] = list[i];
		listSize = c+1;
		for (; i < c; i++) nlist[i] = 0;
		delete[] list;
		list = nlist;
	}
	list[c] = object;
}


template <class T>
void TinyList<T>::remove(char c) {
	if (c < 0) c+=128;
	if (c < listSize) list[c] = 0;
}
#endif
