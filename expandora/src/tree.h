#ifndef TREE_H 
#define TREE_H 

#include <vector>
using namespace std;

#define ALPHABET_SIZE	27	/*  26 letters */
#define A_SIZE		ALPHABET_SIZE
#define MAX_HASH_LEN	150	/* this caps top length of the tree thread */


struct TTree {
    TTree *leads[A_SIZE];	/* pointers to the next part letter */
    vector<unsigned int>        ids;
};

struct levels_data_type {
  unsigned long leads;
  unsigned long nodes;
  unsigned long items;
};

class CTree {
  int divingDelete(TTree *p, char *part, unsigned int id);
  void genHash(const char *name, char *hash);
  void deleteAll(TTree *t);
    
  /* for gathering debug info only*/
  struct levels_data_type levels_data[MAX_HASH_LEN];
  void calculateInfo(TTree *t, int level, int single);
  long debugSingles;

public:
  TTree *root;


  CTree();

  void addName(const char *name, unsigned int id);	
  void resetTTree(TTree *t);
  TTree * findByName(const char *name);
  void deleteItem(const char *name, unsigned int id);
  void reinit();
  void printTreeStats();
  void removeId(unsigned int id, TTree *t);
};

extern class CTree NameMap;

#endif


