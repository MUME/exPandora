#include <stdio.h>
#include <string.h>

//#include <arpa/telnet.h>
#define IAC 255

#define QT_THREAD_SUPPORT
#include <qmutex.h>


#include "defines.h"
#include "struct.h"

#include "tree.h"
#include "stacks.h"
#include "xml2.h"
#include "event.h"
#include "dispatch.h"
#include "utils.h"
#include "engine.h"

#include "userfunc.h"
#include "forwarder.h"

class Cdispatcher dispatcher;
char    last_prompt[MAX_STR_LEN];

Cdispatcher::Cdispatcher() {
  amount = 0;
  roomdesc[0]=0;
  brief_mode = 0;
  getting_desc = 0;
  leader[0] = 0;        /* no leader yet */
  following_leader = 0;
  strcpy(leader_pattern, "You now follow ");
}

void Cdispatcher::set_leaderpatter(char *line)
{
  strcpy(leader_pattern, line);
}


int Cdispatcher::check_roomname(char *line) {
    char roomname[MAX_STR_LEN];
    
    unsigned int i, z;

	
    if (strlen(line) <= (strlen(roomname_start)+strlen(roomname_end)) ) 
	return 0;
	
    if (strncmp(line, roomname_start, 5) != 0)
        return 0;
    
    
    /* as rule - roomname ends as soon as we meet ESC char */
		
    z=0;
    /* going to the next char */
    for (i=5;i<strlen(line); i++) {
		
	if (line[i]!=roomname_end[0]) {
            z++;			

        } else {
            roomname[0] = 0;
            
            strncpy(roomname, &line[5], z);
            roomname[z] = 0;
            
            print_debug(DEBUG_DISPATCHER, "ROOMNAME: %s%s%s", roomname_start, roomname, roomname_end);
            
            preRAdd(R_ROOM, roomname);
            notify_analyzer();
        
            getting_desc = 1;   /* get desc if it will be there */
            
            return 1;
        }
		
		
    }

    printf("ERROR IN PROXY - reached line end without reaching end of the room desc!\n");
    return 0;
}

int Cdispatcher::check_exits(char *line)
{
    unsigned int i;
   
    i = 0;
    
    if (strlen(exits_pattern) <= strlen(line))
	if (strncmp(line, exits_pattern, strlen(exits_pattern)) == 0) {
            preRAdd(R_EXITS, &line[ strlen(exits_pattern) ]);
            
            print_debug(DEBUG_DISPATCHER, "EXITS: %s [%i]", &line[6], strlen(&line[6]) );
            getting_desc = 0;   /* do not react on desc alike strings anymore */
            notify_analyzer();
            return 1;
	}
	
	return 0;
}

/**
 * rewrite the text so that it only contains useful information?
 */
void Cdispatcher::dispatch_buffer() 
{
  int l;
  char line[MAX_DATA_LEN];
  
  
  line[0] = 0;  /* set first terminator */
  amount = 0;
  l = 0;

  while (o_pos < o_len) {
    /* telnet sequences support */
    if ((unsigned char)o_buf[o_pos] == (unsigned char) IAC) {
      if (l != 0) {
        /* in case we met this damn telnet sequence right in the middle */
        /* of some other line. (which looks pretty much impossible) */
        line[l] = 0;
        memcpy(buffer[amount].line, line, l);
        buffer[amount].len = l;   
        buffer[amount].type = IS_NONE;  /* line without end - will be just coppied as is */
          
        amount++;
        l = 0;
      }
      /* ok, go on with the telnet sequence */  
      memcpy(buffer[amount].line, &o_buf[o_pos], 3);
      buffer[amount].line[3] = 0;
      buffer[amount].len = 3;
      o_pos+=3;
      buffer[amount].type = IS_TELNET;
      amount++;

      l = 0;
      continue;
    }
    
    /* for forward check buf[i+1] */
    if (o_pos <= (o_len - 1)) {
      if ((o_buf[o_pos] == 0xd) && (o_buf[o_pos + 1] == 0xa)) {
          line[l] = 0;
          o_pos += 2;

          /* ODOA lines might contain prompt in them */
          if (line[0] == '*' || line[0] == 'o' || line[0] == '!') {
              unsigned int z;
              for (z = 1; z < (strlen(line) - 1); z++) 
                  if (line[z] == '>') {
                      /* here we assume we've found prompt */
                      memcpy(buffer[amount].line, line, z + 1);
                      buffer[amount].type = IS_CRLF;
                      buffer[amount].line[z+1] = 0;
                      buffer[amount].len = z + 1;
                      printf(" fixed prompt is : ..%s..\r\n", buffer[amount].line);

                      amount++;

                      
                      strcpy(buffer[amount].line, line + z + 1);
                      buffer[amount].len = strlen(buffer[amount].line);
                      buffer[amount].type = IS_CRLF;
                      printf(" fixed line is : ..%s..\r\n", buffer[amount].line);
                      amount++;
                      
                      printf("------------------Fixed bug with prompt in line!\r\n");
                      l = 0;
                      continue;
                  }
              
          } 
          
          
          memcpy(buffer[amount].line, line, l);
          buffer[amount].type = IS_CRLF;
          buffer[amount].line[l] = 0;
          buffer[amount].len = l;
          amount++;

          l = 0;
          continue;
      }        
    
      if ((o_buf[o_pos] == 0xa) && (o_buf[o_pos + 1] == 0xd)) {
          o_pos+=2;
          memcpy(buffer[amount].line, line, l);
          buffer[amount].line[l] = 0;
          buffer[amount].type = IS_LFCR;
          buffer[amount].len = l;
          amount++;

          l = 0;
          continue;
      }
    }
    
    if (o_buf[o_pos] == 0xa) {
      o_pos+=1;
      memcpy(buffer[amount].line, line, l);
      buffer[amount].line[l] = 0;
      buffer[amount].type = IS_LF;
      buffer[amount].len = l;
      amount++;
      
      l = 0;
      continue;
    }
      
      
    
    line[l++] = o_buf[o_pos++];
  }
  /* if we are there .. then its either prompt or password line, or */
  /* or splitted packet */
  line[l] = 0;
  if (l == 0)
      return;

  for (unsigned int i=l; i!=0; i--) 
      if (line[i]=='>') {
          /* its not very smart prompt detection, but its not very complicated also ! =) */
          /* this is prompt then */

          memcpy(buffer[amount].line, line, l);
          buffer[amount].type = IS_PROMPT;
          buffer[amount].line[l] = 0;
          buffer[amount].len = l;
          amount++;
          return;
      } 
      
  /* else its splitted line */
  memcpy(buffer[amount].line, line, l);
  buffer[amount].len = l;
  buffer[amount].line[l] = 0;
                
      
  /* ???? this is some silly attempt to catch splitted lines */
  /* just some hack, i assume it works only with certain MTU, */
  /* most likely normal TCP mtu though */
  if (o_len == 1400) {
      buffer[amount].type = IS_SKIP;
  } else {
      buffer[amount].type = IS_SKIP;
  }
  amount++;
  return;	/* warning awaiting routine that buffer is over */
}

void Cdispatcher::analyze_mud_stream(char *buf, int *n) 
{
    int i;
    int new_len;
    
    o_buf = buf;
    o_len = *n;
    o_pos = 0;
    new_len = 0;
    
  
    buf[*n] = 0;

/*
    #ifdef DEBUG
    printf("packet arrived. size : %i\n", *n);
    printf("Buffer is : \r\n%s\r\n", buf); 
    #endif
*/

    dispatch_buffer();
  
    /* broken code */
    if (buffer[amount-1].type == IS_SPLIT) {
        printf("Dispatcher : PACKET SPLIT NOTICED. Awaiting for continue.\r\n");
        send_to_user("\r\n --[Pandora: PACKET SPLIT DETECTED\r\n");
        if (amount != 1) 
            *n=0;   
            
        return;
    }
    
    buf[0]=0;
    /* else we simply recreate our buffer and parse lines */
    for (i = 0; i< amount; i++) {
        if ( (buffer[i].type != IS_LFCR) && (strlen(roomdesc) != 0) && getting_desc) {
            /* some room ended */
            preRAdd(R_DESC, roomdesc);                                                                                              
            notify_analyzer();                                                                                                                     
                    
            print_debug(DEBUG_DISPATCHER, "DESC: %s", roomdesc);
            roomdesc[0] = 0;
            getting_desc = 0;   /* no more descs incoming */
        }
        
        if ((buffer[i].type == IS_LFCR) && strlen(buffer[i].line) && getting_desc) {
            
            strcat(roomdesc, buffer[i].line);
            strcat(roomdesc, "|");
            
            if (brief_mode)
                continue;
        }
        
        if (buffer[i].type == IS_CRLF) {
            /* before any checks cut away the "propeller chars */
            char a_line[MAX_DATA_LEN];
            char *k, *j;
            
            a_line[0] = 0;
            for (k = buffer[i].line, j = a_line; *k; k++) {
              if (*(k+1) && *(k+1) == 0x8 && 
                    (*k == '|' || *k == '-' || *k == '\\' || *k == '/') ) {
                print_debug(DEBUG_DISPATCHER, "Fixing properller chars in line %s", k);
                k += 2;
              }
              
              *(j++) = *k;
            }
            
            *j = 0;
            
            /*
            print_debug(DEBUG_DISPATCHER, "line after fixing: %s", a_line);
            */
          
			//hackish side effects?
            if (check_roomname(a_line) == 0) 
                if (check_exits(a_line) == 0)
                    if (check_failure(a_line) == 0)
                    {
                        /* none */
                    }
        }            

        if (buffer[i].type == IS_PROMPT) {
            strcpy(last_prompt, buffer[i].line);
            
            preRAdd(R_PROMPT, last_prompt);                                                                                              
            notify_analyzer();                                                                                                                     
        }
        
        /* recreating this line in buffer */
        
        memcpy(buf + new_len, buffer[i].line, buffer[i].len);
        new_len += buffer[i].len;
        
        if (buffer[i].type == IS_CRLF) {
            memcpy(buf + new_len, "\r\n", 2);
            new_len += 2;
        } else if (buffer[i].type == IS_LFCR) {
            memcpy(buf + new_len, "\n\r", 2);
            new_len += 2;
        } else if (buffer[i].type == IS_LF) {
            memcpy(buf + new_len, "\n", 1);
            new_len += 1;
        }
        
        
    }
  
    amount = 0;
    *n=new_len;
}


int Cdispatcher::check_failure(char *nline)
{
  char tmp_leader_moves[MAX_STR_LEN];
  struct TFailureData *p;
  
  p = failure_data;
  while (p) {
    if (strcmp(nline, p->pattern) == 0 ) {
      print_debug(DEBUG_DISPATCHER, "Got failure %s!", p->data);
      preRAdd(p->type, p->data);
      notify_analyzer();
      return 1;
    }
    p = p->next;
  }
  

  if (strncmp(nline, leader_pattern, strlen(leader_pattern)) == 0) {
    strcpy(leader, nline + strlen(leader_pattern));
    leader[ strlen(leader) - 1 ] = 0; /* remove the dot (... follow Duj.)" */
    following_leader = 1;
    send_to_user("-- [Pandora: Following leader : ");
    send_to_user(leader);
    send_to_user("\r\n");
    
    return 1;
  }

  if (following_leader) {
    sprintf(tmp_leader_moves, "%s leaves ", leader);
    if (strncmp(nline, tmp_leader_moves, strlen(tmp_leader_moves) ) == 0 ) {
      last_leaders_movement = nline[ strlen(tmp_leader_moves) ];
      printf("Last leaders movement %c.\r\n", last_leaders_movement);
      return 1;
    }
  
    sprintf(tmp_leader_moves, "You follow %s.", leader);
    if (strcmp(nline, tmp_leader_moves) == 0) {

      if (engine_flags.mapping) {
        engine_flags.mapping = 0;
        send_to_user("--[Pandora: Mapping is now OFF!\r\n");
      }
      
      switch (last_leaders_movement) {
        case 'n' : 
		preCAdd(C_MOVE, "north");
                break;
        case 'e' :
		preCAdd(C_MOVE, "east");
                break;
        case 's' :
		preCAdd(C_MOVE, "south");
                break;
        case 'w' :
		preCAdd(C_MOVE, "west");
                break;
        case 'u' :
		preCAdd(C_MOVE, "up");
                break;
        case 'd' :
		preCAdd(C_MOVE, "down");
                break;
        
        default:
                return 1;
                break;
      }
      

      return 1;
    }

  }
  
  if (strcmp(nline, "It is pitch black...") == 0) {
      print_debug(DEBUG_DISPATCHER, "Blinded movement detected!");
      preRAdd(R_BLIND, NULL);
      return 1;
  }

  if (strcmp(nline, "You just see a dense fog around you...") == 0) {
      print_debug(DEBUG_DISPATCHER, "Blinded movement detected!");
      preRAdd(R_BLIND, NULL);
      return 1;
  }

  
  if (strlen(nline) > 17) {
    const char scout_template[] = "You quietly scout ";
      
    if (strncmp(nline, scout_template, strlen(scout_template) ) == 0) {
      print_debug(DEBUG_DISPATCHER, "Scouting detected!");
      preCAdd(C_SCOUT, NULL);
      return 1;
    }
  }

  if ( (strlen(nline) > 26) && (strncmp(nline, "The ", 4) == 0) ) {
    unsigned int i;

    i = 4;
    while (i < strlen(nline)) {
      if (nline[i] == ' ')
        break;
      i++;
    }
    if (strcmp(&nline[i], " seems to be closed.") == 0) {
      print_debug(DEBUG_DISPATCHER, "Movement failure WALL");
      preRAdd(R_FAIL, "door");
      return 1;
    }
  }

/*
  if ( (strlen(nline) > 26) && (strncmp(nline, "ZBLAM!", 6) == 0 ) ) {
    print_debug(DEBUG_DISPATCHER, "Movement failure ZBLAM!");
    preRAdd(R_FAIL, "zblam");
    return 1;
  }
*/
  return 0;
}

/* ======================= USER LAND ============================ */


/* new user input analyzer */
void Cdispatcher::analyze_user_stream(char *buf, int *n) 
{
    int i, result;
    int new_len;
    
    o_buf = buf;
    o_len = *n;
    o_pos = 0;
    new_len = 0;
    
    buf[*n] = 0;

    dispatch_buffer();
  
    buf[0]=0;
    for (i = 0; i< amount; i++) {

      if (buffer[i].type == IS_CRLF || buffer[i].type == IS_LFCR ||
          buffer[i].type == IS_LF) {
        result = parse_user_input_line(buffer[i].line);
        if (result == USER_PARSE_DONE) 
          continue;
      }

      /* recreating this line in buffer */
      
      memcpy(buf + new_len, buffer[i].line, buffer[i].len);
      new_len += buffer[i].len;
        
      if (buffer[i].type == IS_CRLF) {
          memcpy(buf + new_len, "\r\n", 2);
          new_len += 2;
      } else if (buffer[i].type == IS_LFCR) {
          memcpy(buf + new_len, "\n\r", 2);
          new_len += 2;
      } else if (buffer[i].type == IS_LF) {
          memcpy(buf + new_len, "\n", 1);
          new_len += 1;
      }

        
    }
  
    amount = 0;
    *n=new_len;

}