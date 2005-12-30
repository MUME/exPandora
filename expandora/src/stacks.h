


class stackmanager {
private:

  vector<Croom *> stacka;
  vector<Croom *> stackb;
  
  vector<Croom *> *sa;
  vector<Croom *> *sb;
  
  unsigned int  mark[MAX_ROOMS];
  unsigned int  turn;
public:
  unsigned int amount() { return sa->size(); }
  unsigned int next()    { return sb->size(); }
  
  Croom * first()  { return (*sa)[0]; }
  Croom * next_first()  { return (*sb)[0]; }
  
  void swap();
  stackmanager();
  void reset();
  
  Croom * get(unsigned int i);

  Croom * get_next(unsigned int i);
  
  void put(unsigned int id);
  void put(Croom *r);
  void remove_room(unsigned int id);    /* swaps */


  /* DEBUG */
  void printstacks();

  void get_current(char *);

};

extern class stackmanager stacker;

#ifdef DMALLOC
#include <dmalloc.h>
#endif
