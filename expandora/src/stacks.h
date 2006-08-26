#ifndef STACKERFILE_H 
#define STACKERFILE_H 



class stackmanager {
  vector<CRoom *> stacka;
  vector<CRoom *> stackb;
  
  vector<CRoom *> *sa;
  vector<CRoom *> *sb;
  
  unsigned int  mark[MAX_ROOMS];
  unsigned int  turn;
public:
  unsigned int amount() { return sa->size(); }
  unsigned int next()    { return sb->size(); }
  
  CRoom * first()  { return (*sa)[0]; }
  CRoom * nextFirst()  { return (*sb)[0]; }
  
  void swap();
  stackmanager();
  void reset();
  
  CRoom * get(unsigned int i);

  CRoom * getNext(unsigned int i);
  
  void put(unsigned int id);
  void put(CRoom *r);
  void removeRoom(unsigned int id);    /* swaps */


  /* DEBUG */
  void printStacks();

  void getCurrent(char *);

};

extern class stackmanager stacker;


#endif 