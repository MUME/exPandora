
#define STACKSIZE 1024


class stackmanager {
private:

  unsigned int stacka[STACKSIZE];
  unsigned int stackb[STACKSIZE];
  char mark[MAX_ROOMS];
  
  unsigned int *sa;
  unsigned int *sb;
  
public:
  unsigned int amount;
  int next;
  
  void swap();
  stackmanager();
  void reset();
  
  unsigned int get(unsigned int i);

  unsigned int get_next(unsigned int i);
  
  void put(unsigned int id);
  

  /* DEBUG */
  void printstacks();

  void get_current(char *);

};

extern class stackmanager stacker;
