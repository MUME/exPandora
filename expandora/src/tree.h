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
  int diving_delete(TTree *p, char *part, unsigned int id);
  void genhash(const char *name, char *hash);
  void delete_all(TTree *t);
    
  /* for gathering debug info only*/
  struct levels_data_type levels_data[MAX_HASH_LEN];
  void calculate_info(TTree *t, int level, int single);
  long debug_singles;

public:
  TTree *root;


  CTree();

  void addname(const char *name, unsigned int id);	
  void reset_TTree(TTree *t);
  TTree * find_by_name(const char *name);
  void delete_item(const char *name, unsigned int id);
  void reinit();
  void print_tree_stats();
  void remove_id(unsigned int id, TTree *t);
};

extern class CTree NameMap;

#endif


