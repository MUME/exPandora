#include <cstring>

#include "defines.h"

#ifdef Q_OS_WIN
//#include <win32config.h>
#include <libxml\parser.h>
#else 
#include "libxml/parser.h"
#endif


#include "struct.h"

#include "rooms.h"
#include "utils.h"
#include "dispatch.h"

xmlChar *strdup(xmlChar * str);
xmlNodePtr find_key(xmlNodePtr parent, char *key);


xmlChar *strdup(xmlChar * str)
{
    return (xmlChar *) strdup((char*) str);
}


xmlNodePtr find_key(xmlNodePtr parent, char *key)
{
    xmlNodePtr t, node;

    //  t = parent->xmlChildrenNode;
    t = parent;

    while (t) {
	if (!xmlStrcmp(t->name, (xmlChar *) key)) {
	    return t;
	}
	if ((t->children) && (node = find_key(t->children, key))) {
	    return node;
	}
	t = t->next;
    }
    return NULL;
}

void xml_writebase(char *filename)
{
    struct Troom *p;
    int i;
    xmlDocPtr doc;
    xmlNodePtr rootnode, node, node1, node2;
    char tmp[4028];

    print_debug(DEBUG_XML, "in xml_writebase()");
    
    p = roomer.getroom(0);	/* get the pointer to the base of array */

    doc = xmlNewDoc((xmlChar *) "1.0");

    rootnode = xmlNewDocNode(doc, NULL, (xmlChar *) "map", NULL);
    xmlAddChild((xmlNodePtr) doc, rootnode);


    while (p->next != NULL) {
	p = p->next;
        
        print_debug(DEBUG_XML, "Saving room %i", p->id);
	node = xmlNewDocNode(doc, NULL, (xmlChar *) "room", NULL);

	xmlAddChild(rootnode, node);

	sprintf(tmp, "%d", p->id);
	xmlNewProp(node, (xmlChar *) "id", (xmlChar *) tmp);

	sprintf(tmp, "%d", p->x);
	xmlNewProp(node, (xmlChar *) "x", (xmlChar *) tmp);
	sprintf(tmp, "%d", p->y);
	xmlNewProp(node, (xmlChar *) "y", (xmlChar *) tmp);
	sprintf(tmp, "%d", p->z);
	xmlNewProp(node, (xmlChar *) "z", (xmlChar *) tmp);


        sprintf(tmp, "%s", p->sector ? p->sector->desc : "NONE");

	xmlNewProp(node, (xmlChar *) "terrain", (xmlChar *) tmp);


        if (p->timestamp[0] != 0) {
          strncpy(tmp, p->timestamp, TIMESTAMP_LEN);
          tmp[TIMESTAMP_LEN] = 0;
          xmlNewProp(node, (xmlChar *) "timestamp", (xmlChar *) tmp);
        }


	sprintf(tmp, "%s", p->name);
	xmlNewTextChild(node, NULL, (xmlChar *) "roomname",
			(xmlChar *) tmp);



	if (p->desc == NULL)
	    sprintf(tmp, "%s", "");
	else
	    sprintf(tmp, "%s", p->desc);
	xmlNewTextChild(node, NULL, (xmlChar *) "desc", (xmlChar *) tmp);

	if (p->note == NULL)
	    sprintf(tmp, "%s", "");
	else
	    sprintf(tmp, "%s", p->note);
	xmlNewTextChild(node, NULL, (xmlChar *) "note", (xmlChar *) tmp);


	node1 = xmlNewDocNode(doc, NULL, (xmlChar *) "exits", NULL);
	xmlAddChild(node, node1);


	for (i = 0; i <= 5; i++) {
	    if (p->exits[i] != 0) {
		node2 = xmlNewDocNode(doc, NULL, (xmlChar *) "exit", NULL);
		xmlAddChild(node1, node2);

		sprintf(tmp, "%c", exitnames[i][0]);
		xmlNewProp(node2, (xmlChar *) "dir", (xmlChar *) tmp);
		sprintf(tmp, "%d", p->exits[i]);
		if (p->exits[i] == EXIT_UNDEFINED)
		    sprintf(tmp, "%s", "UNDEFINED");
                if (p->exits[i] == EXIT_DEATH)
		    sprintf(tmp, "%s", "DEATH");
                
		xmlNewProp(node2, (xmlChar *) "to", (xmlChar *) tmp);

                
		if (p->doors[i] == NULL)
		    sprintf(tmp, "%s", "");
		else
		    sprintf(tmp, "%s", p->doors[i]);
		xmlNewProp(node2, (xmlChar *) "door", (xmlChar *) tmp);

	    }
	}

        print_debug(DEBUG_XML, "done");
    }

    print_debug(DEBUG_XML, "writing XML document.");
    
    xmlSaveFormatFile(filename, doc, 1);
    
    xmlFreeDoc(doc);
    
    
    print_debug(DEBUG_XML, "Quiting xml_writebase()\r\n");
}


/* Loads a char, returns char struct or NULL */
void xml_readbase(char *filename)
{
    xmlChar *tmp;
    xmlDocPtr doc;
    xmlNodePtr rootnode, node;
    int i;
    struct Troom *r;
    struct room_sectors_data *p;

    doc = xmlParseFile(filename);
    if (!doc || !(rootnode = xmlDocGetRootElement(doc))) {
	printf("XML: Error while parsing %s, bad xml or empty tree \n", base_file);
	return;
    }

 
    rootnode = find_key((xmlNodePtr) doc, "map")->children;

    
    while (rootnode) {

	r = new Troom;
        reset_room(r);
      
	if (r == NULL) {
	    printf("XML: Error while allocating memory in readbase function!");
	    exit(1);
	}

      
        tmp = xmlGetProp(rootnode, (xmlChar *) "id");
	if (!tmp) {
	    rootnode = rootnode->next;
	    continue;
	}
	r->id = atoi((char *) tmp);

        free(tmp);

	tmp = xmlGetProp(rootnode, (xmlChar *) "x");
	if (!tmp) {
	    rootnode = rootnode->next;
	    continue;
	}
	r->x = atoi((char *) tmp);

	free(tmp);

	tmp = xmlGetProp(rootnode, (xmlChar *) "y");
	if (!tmp) {
	    rootnode = rootnode->next;
	    continue;
	}
	r->y = atoi((char *) tmp);

        free(tmp);

	tmp = xmlGetProp(rootnode, (xmlChar *) "z");

        if (!tmp) {
	    rootnode = rootnode->next;
	    continue;
	}
	r->z = atoi((char *) tmp);
	free(tmp);

      	tmp = xmlGetProp(rootnode, (xmlChar *) "terrain");
	if (!tmp) {
	    rootnode = rootnode->next;
	    continue;
	}
      	r->sector = NULL;
        p = room_sectors;
        while (p) {
          if (strcmp(p->desc, (char *) tmp) == 0)
            r->sector = p;
          
          p = p->next;
        }
	free(tmp);

      
	tmp = xmlGetProp(rootnode, (xmlChar *) "timestamp");
        if (tmp) {
	  strcpy(r->timestamp, (char*) tmp);
          free(tmp);
        }

	node = find_key(rootnode, "roomname");
        if (node) {
	  tmp = xmlNodeListGetString(doc, node->children, 1);
  	  r->name = (char *) strdup(tmp ? tmp : (xmlChar *) "");
          if (!r->name) {
	    printf("XML: Error while allocating memory in readbase function!");
	    exit(1);
          }
	  free(tmp);

          }
        
	node = find_key(rootnode, "desc");
	if (node) {
          tmp = xmlNodeListGetString(doc, node->children, 1);
          if (tmp == NULL) {
              r->desc = NULL;
          } else {
              r->desc = (char *) strdup(tmp ? tmp : (xmlChar *) "");
              if (!r->desc) {
                  printf("XML: Error while allocating memory in readbase function!");
                  exit(1);
              }
          
          }  

          free(tmp);
        }
        
	node = find_key(rootnode, "note");
	if (node) {
          tmp = xmlNodeListGetString(doc, node->children, 1);
          if (tmp == NULL) {
              r->note = NULL;
          } else {
              r->note = (char *) strdup(tmp ? tmp : (xmlChar *) "");
              if (!r->note) {
                  printf("XML: Error while allocating memory in readbase function!");
                  exit(1);
              }
  
          }

          free(tmp);
        }

	for (i = 0; i <= 5; i++) {
	    r->exits[i] = 0;
	    r->doors[i] = NULL;
	}

        node = find_key(rootnode, "exits");
        if (node) {
          node = node->children;
          while (node) {
              unsigned int dir;
              unsigned int to;
  
              tmp = xmlGetProp(node, (xmlChar *) "dir");
              if (!tmp) {
                  node = node->next;
                  continue;
              }
              dir = numbydir(tmp[0]);

            free(tmp);
  
              tmp = xmlGetProp(node, (xmlChar *) "to");
              if (!tmp) {
                  node = node->next;
                  continue;
              }
              
              i = 0;
              to = 0;
              while (room_flags[i].name) { 
                if (strcmp((char *) tmp, room_flags[i].xml_name) == 0) {
                  to = room_flags[i].flag;
                }
                i++;
              }
              if (!to)
                to = atoi( (char *) tmp);
              
              free(tmp);
  
              r->exits[dir] = to;
  
              tmp = xmlGetProp(node, (xmlChar *) "door");
              if (!tmp) {
                  node = node->next;
                  continue;
              }
              if (tmp[0] != 0) {
                  r->doors[dir] = (char *) strdup(tmp);
                  if (!r->doors[dir]) {
                    printf("XML: Error while allocating memory in readbase function!");
                    exit(1);
                  }
              
              }

              free(tmp);
  
              node = node->next;
          }
          
        }
        
	roomer.addroom_nonsorted(r);	/* tada! */
	rootnode = rootnode->next;
    }				/* long while ends */

    roomer.resort_rooms();
    xmlFreeDoc(doc);
}
