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
	//printf("deactivating ... old queue size: %i\n", size());
	obj->clear();
	push(obj);
	//printf("deactivating ... new queue size: %i\n", size());
}

template <class T>
T * ObjectRecycler<T>::activate() {
	//printf("acitvating ... old queue size: %i\n", size());
	if (!empty()) {
		T * ret = front();
		pop();
		//printf("acitvating ... new queue size: %i\n", size());
		return ret;
	}
	else return new T();
}

#endif
