#include <cstdlib>
#include <cstdio>
#include <cstring>

#include "defines.h"
#include "utils.h"
#include "tree.h"

class CTree NameMap;


void CTree::removeId(unsigned int id, TTree *p)
{
  vector<unsigned int>::iterator i;

  for (i = p->ids.begin(); i != p->ids.end(); i++)
      if (*i == id) {
          i = p->ids.erase(i);
          return;
      }
    
}


void CTree::calculateInfo(TTree *t, int level, int single)
{
  unsigned int i;
  int l;
  
  if (t == NULL)
    return;

  
  
  levels_data[level].nodes++;
  levels_data[level].items += t->ids.size();
  l = 0;
  
  for (i = 0; i < A_SIZE; i++)
    if (t->leads[i] != NULL) {	
      
      l++;
      
      levels_data[level].leads++;
      
      calculateInfo(t->leads[i], level+1, t->ids.size() ? single + 1 : 0);
    }
    
  if (l == 0 && single > 0) 
    debugSingles += (single+1);
  
}

void CTree::printTreeStats()
{
  unsigned int i;

  memset(levels_data, 0, sizeof(struct levels_data_type) * MAX_HASH_LEN );
  debugSingles = 0;
  
  calculateInfo(root, 0, 0);  
  
  print_debug(DEBUG_TREE, "Single rooms: %i", debugSingles);
  
  for (i = 0; levels_data[i].nodes; i++) 
    print_debug(DEBUG_TREE, "Level %-3li, nodes %-5li, leads %-5li, items %-5li.", 
          i, levels_data[i].nodes, levels_data[i].leads, levels_data[i].items);
}

void CTree::reinit()
{
    print_debug(DEBUG_TREE, "clearing the whole tree");
    deleteAll(root);

    root = new TTree;
    resetTTree(root);
}

/* recursive deletion of this element and all elements below */
void CTree::deleteAll(TTree *t) 
{
    int i;
  
    if (t == NULL) 
        return;

    for (i = 0; i < A_SIZE; i++)
        if (t->leads[i] != NULL) {	
            deleteAll(t->leads[i]);
        }

    delete t;
}

int CTree::divingDelete(TTree * p, char *part, unsigned int id)
{
    int i;

    if (strlen(part) == 0) {	/* we've found our item */
	for (i = 0; i < A_SIZE; i++)
            if (p->leads[i] != NULL) {	/* shall not delete this item */
                removeId(id, p);
                return -1;	/* return state DID NOT DELETE - its used */
            }

            removeId(id, p);
            if (p->ids.empty()) {
                delete p;
                return 1;
            }
            
            return -1;		/* did NOT delete, item is in use */
    }


    /* ending part of recursion is over */
    /* diving-cycling part */
    /* given the path exist (see delete_room func.) call is easier */
    i = divingDelete(p->leads[(int) part[0]], &part[1], id);
    if (i == -1)		/* did NOT delete the lower item */
	return -1;		/* so we do not delete this one also ! */

    /* else, we have to check this one too */

    p->leads[(int) part[0]] = NULL;	/* clear this lead */

    if (p->ids.size() != 0)
	return -1;		/* nop, still in use */

    for (i = 0; i < A_SIZE; i++)
	if (p->leads[i] != NULL)
	    return -1;		/* no still in use ! */

    /* else ! we have to delete it ... */
    delete p;
    return 1;			/* deleted ! so ... */
}

void CTree::deleteItem(const char *name, unsigned int id)
{
    TTree *p;
    unsigned int i;
    char hash[MAX_HASH_LEN];	
	
    genHash(name, hash);
    p = findByName(name);
    if (p == NULL) {
	printf("Error in TREE module - attempt to delete not existing items (at all)\n");
	return;
    }

    for (i = 0; i < p->ids.size(); i++)
        if (p->ids[i] == id) {
            if (divingDelete(root, hash, id) == 1) {
                /* meaning - occasioanly freed our ROOT element */
                CTree();	/* reinit */
                return;
            }
        }
}

void CTree::resetTTree(TTree * t)
{
    int i;

    for (i = 0; i < A_SIZE; i++)
	t->leads[i] = NULL;
}

CTree::CTree()
{
    root = new TTree;
    resetTTree(root);
}

void CTree::addName(const char *name, unsigned int id)
{
  TTree *p, *n;
  unsigned int i;
  char hash[MAX_HASH_LEN];
      
  genHash(name, hash);
  p = root;

  for (i = 0; i < strlen(hash); i++) {
    if (p->leads[(int) hash[i]] != NULL) {
    /* there is similar sequence already ... we follow it */
      p = p->leads[(int) hash[i]];
    
    } else {
      /* there is no line like this in tree yet - we have to create new lead */
      n = new TTree;
      resetTTree(n);
    
      p->leads[(int) hash[i]] = n;
      p = n;
    }
  }

  /* ok, we found totaly similar or created new entry, add id to it */
  p->ids.push_back(id);
}

TTree *CTree::findByName(const char *name)
{
  unsigned int i;
  TTree *p;
  char hash[MAX_HASH_LEN];
    
  genHash(name, hash);
    
    
  p = root;
  for (i = 0; i < strlen(hash); i++) {
    if (p->leads[(int) hash[i]] == NULL)
      return NULL;
  
      p = p->leads[(int) hash[i]];
  }
  
  return p;
}

void CTree::genHash(const char *name, char *hash)
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

