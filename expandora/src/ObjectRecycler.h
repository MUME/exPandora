#pragma once
#ifndef OBJECTRECYCLER
#define OBJECTRECYCLER
#include <queue>
using namespace std;

template <class T>
class ObjectRecycler : private queue<T *> {
	public:
		void deactivate(T * obj);
		T * activate();
};

template <class T>
void ObjectRecycler<T>::deactivate(T * obj) {
	//printf("deactivating ...\n");
	obj->clear();
	push(obj);
}

template <class T>
T * ObjectRecycler<T>::activate() {
	//printf("acitvating ...\n");
	if (!empty()) {
		T * ret = front();
		pop();
		return ret;
	}
	else return new T();
}

#endif
