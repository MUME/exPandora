#include <stdlib.h>
#include <stdio.h>
#include <string.h>

#include "defines.h"
#include "tree.h"
#include "utils.h"

class Ctree namer;


void Ctree::calculate_info(Ttree *t, int level)
{
  unsigned int i;

  if (t == NULL)
    return;

  levels_data[level].nodes++;
  levels_data[level].items += t->amount;

  for (i = 0; i < A_SIZE; i++)
    if (t->leads[i] != NULL) {	
      /* shall not delete this item */
      
      levels_data[level].leads++;
      
      calculate_info(t->leads[i], level+1);
    }
}

void Ctree::print_tree_stats()
{
  unsigned int i;

  bzero(levels_data, sizeof(struct levels_data_type) * MAX_HASH_LEN );
  
  calculate_info(root, 0);  
  
  for (i = 0; levels_data[i].nodes; i++) 
    print_debug(DEBUG_TREE, "Level %-3li, nodes %-5li, leads %-5li, items %-5li.", 
          i, levels_data[i].nodes, levels_data[i].leads, levels_data[i].items);
}

void Ctree::reinit()
{
    print_debug(DEBUG_TREE, "clearing the whole tree");
    delete_all(root);

    root = new Ttree;
    reset_ttree(root);
}

/* recursive deletion of this element and all elements below */
void Ctree::delete_all(Ttree *t) 
{
    int i;
    
  
    if (t == NULL) 
        return;

    for (i = 0; i < A_SIZE; i++)
        if (t->leads[i] != NULL) {	
            delete_all(t->leads[i]);
        }

    free(t->ids);
    free(t);
    t = NULL;
}

void Ctree::delete_id(unsigned int id, Ttree *t)
{
  unsigned int i;
  unsigned int *p;
  
  p = t->ids;
  for (i=0; i <= t->amount; i++) {
    if (*(p+i) == id) {
      *(p+i) = *(p + (t->amount-1));
      t->amount--;
      if (t->amount == 0)
        free(t->ids);
        
    }
  }
}

int Ctree::diving_delete(Ttree * p, char *part, unsigned int id)
{
    int i;

    if (strlen(part) == 0) {	/* we've found our item */
	for (i = 0; i < A_SIZE; i++)
            if (p->leads[i] != NULL) {	/* shall not delete this item */
                delete_id(id, p);
                return -1;	/* return state DID NOT DELETE - its used */
            }


            if (p->amount == 1) {
                p->amount = 0;
                free(p);
                p = NULL;
                return 1;		/* return state DELETED THE ITEM - useless */
            }


            delete_id(id, p);

            return -1;		/* did NOT delete, item is in use */
    }


    /* ending part of recursion is over */
    /* diving-cycling part */
    /* given the path exist (see delete_room func.) call is easier */
    i = diving_delete(p->leads[(int) part[0]], &part[1], id);
    if (i == -1)		/* did NOT delete the lower item */
	return -1;		/* so we do not delete this one also ! */

    /* else, we have to check this one too */

    p->leads[(int) part[0]] = NULL;	/* clear this lead */

    if (p->amount != 0)
	return -1;		/* nop, still in use */

    for (i = 0; i < A_SIZE; i++)
	if (p->leads[i] != NULL)
	    return -1;		/* no still in use ! */

    /* else ! we have to delete it ... */
    free(p);
    p = NULL;
    return 1;			/* deleted ! so ... */

}

void Ctree::delete_item(char *name, unsigned int id)
{
    Ttree *p;
    unsigned int *i;
    char hash[MAX_HASH_LEN];	
	
    genhash(name, hash);
    p = find_by_name(name);
    if (p == NULL) {
	printf("Error in TREE module - attempt to delete not existing items (at all)\n");
	return;
    }


    i = p->ids;
    for (i = p->ids; i <= (p->ids + (p->amount-1)); i++) {
	if (*i == id) {
	    if (diving_delete(root, hash, id) == 1) {
	    	/* meaning - occasioanly freed our ROOT element */
		Ctree();	/* reinit */
		return;
	    }

	    return;
	}
    
    
    }

    printf("Error in TREE module - attempt to delete not existing item (wrong ID)\n");
}

void Ctree::reset_ttree(Ttree * t)
{

    int i;

    t->amount = 0;

    for (i = 0; i < A_SIZE; i++)
	t->leads[i] = NULL;

    t->ids = NULL;	/* without freeing - might create a memory leak, but like */
					/* once per total tree deletion :p */
    t->size = 0;
}

Ctree::Ctree()
{
    root = new Ttree;
    reset_ttree(root);
}

void Ctree::addname(char *name, unsigned int id)
{
  Ttree *p, *n;
  unsigned int i;
  char hash[MAX_HASH_LEN];
      
  genhash(name, hash);
  p = root;

  for (i = 0; i < strlen(hash); i++) {
    if (p->leads[(int) hash[i]] != NULL) {
    /* there is similar sequence already ... we follow it */
      p = p->leads[(int) hash[i]];
    
    } else {
      /* there is no line like this in tree yet - we have to create new lead */
      n = new Ttree;
      reset_ttree(n);
    
      p->leads[(int) hash[i]] = n;
      p = n;
    }
  }

  /* ok, we found totaly similar or created new */
  if (p->ids == NULL) {
    p->ids = (unsigned int *) malloc(sizeof(unsigned int));
    p->amount = 0; 
    p->size = 1;
  }
  
  if (p->amount == p->size) {		/* double the amount of space for ids */
    /*printf("Had to double the ids space.\r\n");
    */
    p->size *= 2;
    p->ids = (unsigned int *) realloc(p->ids, sizeof(unsigned int) * p->size);
  }

      
  *(p->ids + p->amount) = id;

  p->amount++;
}

Ttree *Ctree::find_by_name(char *name)
{
  unsigned int i;
  Ttree *p;
  char hash[MAX_HASH_LEN];
    
  genhash(name, hash);
    
    
  p = root;
  for (i = 0; i < strlen(hash); i++) {
    if (p->leads[(int) hash[i]] == NULL)
      return NULL;
  
      p = p->leads[(int) hash[i]];
  }
  
  return p;
}

void Ctree::genhash(char *name, char *hash)
{
  unsigned int i;
  unsigned int z;
  
  z=0;
  for (i=0; i<strlen(name); i++) {
    if ( (name[i]>='A') && (name[i]<='Z') ) {
      hash[z++] = LOWER(name[i]);
      continue;						
    }
    /* not lower case letters gets cut out */
    if ( (name[i]>='a') && (name[i]<='z') ) {
      hash[z++]=name[i];
      continue;
    }
  }
  hash[z] = 0;
  
  for (i=0; i<strlen(hash); i++)
    hash[i]=hash[i]-'a'+1;
}

void Ctree::print_element(Ttree *t)
{
	unsigned int *i;
	unsigned int k, z;
	
	printf("-----------------\n");
	printf("Element address: %i, ids amount : %i, ids array size : %i\n", (int) t, t->amount, t->size);
	i=t->ids;
		
	printf("Ids: ");
	for (i = t->ids; i <= (t->ids + (t->amount-1)); i++) 
	  printf("%i, ", (unsigned int) *(i) );

	printf("\n");
	
	k=0;
	for (z=0; z<A_SIZE; z++)
		if (t->leads[z]!=NULL)
			k++;
		
	printf("Amount of non-empty leads : %i\n", k);
	printf("Namely (hash-items) : ");
		
	k=0;
	for (z=0; z<A_SIZE; z++)
		if (t->leads[z]!=NULL)
			printf("%c, ", z+'a'-1);
	printf("\n");
	printf("-----------------\n");
		
}
