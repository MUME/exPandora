#ifndef OBJECTRECYCLER
#define OBJECTRECYCLER
#include "TinyList.h"


template <class T>
class ObjectRecycler : protected TinyList<T *>
{
public:
  ObjectRecycler() :pos(0) {}


  void deactivate(T * obj)
  {
    if (pos > 127) delete obj;
    else
    {
      obj->clear();
      TinyList<T *>::put(pos, obj);
      pos++;
    }
  }
  
  
  T * activate()
  {
    if (pos != 0)
    {
      pos--;
      T * ret = TinyList<T*>::get(pos);
      TinyList<T*>::remove(pos);
      return ret;
    }
    else return new T;
  }
  
  
protected:
  unsigned int pos;
};


#ifdef DMALLOC
#include <mpatrol.h>
#endif
#endif
