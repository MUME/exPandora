
#define ALPHABET_SIZE	27	/* 0 + 26 letters */
#define A_SIZE		ALPHABET_SIZE
#define MAX_HASH_LEN	150	/* this caps top length of the tree thread */

	

struct Ttree {
    Ttree *leads[A_SIZE];	/* pointers to the next part letter */
    unsigned int amount;	/* amount of rooms with this name (sequence) */
    unsigned int size;	/* current max size of ids array */ 
    unsigned int *ids;
};

struct levels_data_type {
  unsigned long leads;
  unsigned long nodes;
  unsigned long items;
};

class Ctree {
  int diving_delete(Ttree *p, char *part, unsigned int id);
  void genhash(char *name, char *hash);
  void delete_id(unsigned int id, Ttree *t);
  void delete_all(Ttree *t);
    
  /* for debug */
  struct levels_data_type levels_data[MAX_HASH_LEN];
  void calculate_info(Ttree *t, int level);
public:
  Ttree *root;

  Ctree();

  void addname(char *name, unsigned int id);	
  void reset_ttree(Ttree *t);
  Ttree * find_by_name(char *name);
  void delete_item(char *name, unsigned int id);
  void print_element(Ttree *t);
  void reinit();
  void print_tree_stats();
};

extern class Ctree namer;
