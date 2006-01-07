


class stackmanager {
private:

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
  CRoom * next_first()  { return (*sb)[0]; }
  
  void swap();
  stackmanager();
  void reset();
  
  CRoom * get(unsigned int i);

  CRoom * get_next(unsigned int i);
  
  void put(unsigned int id);
  void put(CRoom *r);
  void remove_room(unsigned int id);    /* swaps */


  /* DEBUG */
  void printstacks();

  void get_current(char *);

};

extern class stackmanager stacker;


