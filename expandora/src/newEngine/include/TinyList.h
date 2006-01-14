#ifndef TINYLIST
#define TINYLIST
/**
 * extremely shrinked array list implementation to be used for each room
 * because we have so many rooms and want to search them really fast, we
 *	- allocate as little memory as possible
 *	- allow only 128 elements (1 per character)
 *	- only need 3 lines to access an element
 */

template <class T>
class TinyList
{
public:
  TinyList() : list(0), listSize(0) {}

  virtual ~TinyList()
  {
    if (list) delete[] list;
  }

  virtual T get(unsigned int c)
  {
    if (c >= listSize) return 0;
    else return list[c];
  }
  
  virtual void put(unsigned int c, T object)
  {
    if (c >= listSize)
    {
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
  
  virtual void remove(unsigned int c)
  {
    if (c < listSize) list[c] = 0;
  }
  
  virtual unsigned int size()
  {
    return listSize;
  }

protected:
  T * list;
  unsigned int listSize;
};






#ifdef DMALLOC
#include <mpatrol.h>
#endif
#endif
