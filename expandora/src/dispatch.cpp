#include <cstdio>
#include <cstring>
#include <qregexp.h>

//#include <arpa/telnet.h>
#define IAC 255

#include "defines.h"

#include <QMutex>


#include "configurator.h"

#include "utils.h"
#include "tree.h"
#include "stacks.h"
#include "xml2.h"
#include "event.h"
#include "dispatch.h"
#include "engine.h"

#include "userfunc.h"
#include "forwarder.h"

class Cdispatcher dispatcher;
char    last_prompt[MAX_STR_LEN];

Cdispatcher::Cdispatcher() {
  amount = 0;
  roomdesc[0]=0;
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
    QRegExp rx;
    unsigned int i;
    

    rx = conf.get_roomname_exp();

    if (rx.indexIn(line) >= 0) {
        /* so we got a roomname in this line */
        for (i = 0; i<strlen(line); i++) {
            int rn_start;
            int rn_end;
            int rn_len;
            
            QByteArray s = conf.get_look_col();
            rn_start = s.length();
            
            s = conf.get_end_col();
            rn_end = s.length();
            rn_len = strlen(line) - rn_start - rn_end;
            
            strncpy(roomname, &line[rn_start], rn_len); 
            roomname[ rn_len] = 0;
            print_debug(DEBUG_DISPATCHER, "ROOMNAME:..%s..%s..%s..", 
                (const char*) conf.get_look_col(),
                roomname, (const char*) conf.get_end_col() );
            
            preRAdd(R_ROOM, roomname);
            notify_analyzer();
        
            getting_desc = 1;   /* get desc if it will be there */
            
            return 1;
            
        }
        
    } 
      
    return 0;
}

int Cdispatcher::check_exits(char *line)
{
    QRegExp rx;

    rx = conf.get_exits_exp();
    if (rx.indexIn(line) >= 0) {
        preRAdd(R_EXITS, &line[ strlen( (const char*) conf.get_exits_pat() ) ]);
        print_debug(DEBUG_DISPATCHER, "EXITS: %s [%i]", &line[6], strlen(&line[6]) );
        getting_desc = 0;   /* do not react on desc alike strings anymore */
//        notify_analyzer();
        return 1;
    }
    
    return 0;
}

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
          if (line[0] == '*' || (line[0] == 'o' && line[1]!='r') || line[0] == '!' || line[0] == ')') {
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
//            notify_analyzer();
            print_debug(DEBUG_DISPATCHER, "DESC: %s", roomdesc);
            roomdesc[0] = 0;
            getting_desc = 0;   /* no more descs incoming */
        }
        
        if ((buffer[i].type == IS_LFCR) && strlen(buffer[i].line) && getting_desc) {
            
            strcat(roomdesc, buffer[i].line);
            strcat(roomdesc, "|");
            
            if (conf.get_brief_mode())
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


QByteArray Cdispatcher::get_colour_name(QByteArray str)
{
    QByteArray s;
    
    s = str.simplified();
    s.truncate(s.indexOf(" ", 0));
    return s;
}

QByteArray Cdispatcher::get_colour(QByteArray str)
{
    QByteArray s;
    int start, end;
    
    start = str.indexOf("[", 0);
    end = str.indexOf("m", start);
    s = str.mid(start, end-start+1);
    return s;
}


int Cdispatcher::check_failure(char *nline)
{
    char tmp_leader_moves[MAX_STR_LEN];
    QRegExp rx;
    vector<QByteArray>::iterator i;

    if (getting_colour_scheme) {
        rx.setPatternSyntax(QRegExp::Wildcard);
	
      	if (!collecting_colours) {
            /* check if we shall start collecting colours */
	    rx.setPattern("Use 'change colour <field> <attribute>' where <field> can be one of:");
            if (rx.indexIn(nline) >= 0) {
	        collecting_colours = true;
	        return 1;
	    }
        } else {
            rx.setPattern("or 'change colour <field> default|monochrome|none' where <field> can be one of:");
            if (rx.indexIn(nline) >= 0) {
                /* check for double colours */
                
                /* look's /roomname) colour */
                for (i = colour_data.begin(); i != colour_data.end(); ++i)
                    if (get_colour_name(*i) == "look") {
                        conf.set_look_col( get_colour( *i) );
                        printf("Look colour is now set to: %s Test. %s\r\n", (const char *) conf.get_look_col(), 
                                                                             (const char *) conf.get_end_col() );
                        i = colour_data.erase(i);
                        break;
                    }
                    
                    
                if (conf.get_look_col() == "") {
                    send_to_user("--[ WARNING. You do not have the look colour set! Mapper wont work properly!\r\n");    
                } else {
                    /* check if any other colour has the same ANSI seq */
                    for (i = colour_data.begin(); i != colour_data.end(); ++i)
                        if (get_colour(*i) == conf.get_look_col()) {
                            send_to_user("--[ WARNING: You have look and %s colours set the same, this will most likely hurt movement analyzer.\r\n", (const char *)  get_colour_name(*i) );               
                        }
                }
                
                /* prompt colour */
                for (i = colour_data.begin(); i != colour_data.end(); ++i)
                    if (get_colour_name(*i) == "prompt") {
                        conf.set_prompt_col( get_colour( *i) );
                        printf("Prompt colour is now set to: %s Test. %s\r\n", (const char *) conf.get_prompt_col(), 
                                                                             (const char *) conf.get_end_col() );
                        i = colour_data.erase(i);
                        break;
                    }
                    
                    
                if (conf.get_prompt_col() != "") {
                    /* check if any other colour has the same ANSI seq */
                    for (i = colour_data.begin(); i != colour_data.end(); ++i)
                        if (get_colour(*i) == conf.get_prompt_col()) {
                            send_to_user("--[ WARNING: You have prompt and %s colours set the same, this will most likely hurt movement analyzer.\r\n", (const char *)  get_colour_name(*i) ); 
                        }
                }

                
                /* free the colorus list and unset all the flags */
                collecting_colours = false;
                getting_colour_scheme = false;
                colour_data.clear();
            } else {
                /* collect the colour and add the item */
                colour_data.push_back(nline);
            }
	}
    }

  for (unsigned int i = 0; i < conf.patterns.size(); i++) {
    if (conf.patterns[i].rexp.indexIn(nline) >= 0 ) {
      if (  conf.patterns[i].group == E_RESULT)
        preRAdd(conf.patterns[i].type, (const char*) conf.patterns[i].data);
      else if (conf.patterns[i].group == E_CAUSE)
        preCAdd(conf.patterns[i].type, (const char*) conf.patterns[i].data);
          
//      ;
      return 1;
    }         
  }

  if (strncmp(nline, leader_pattern, strlen(leader_pattern)) == 0) {
    strcpy(leader, nline + strlen(leader_pattern));
    one_argument(leader, leader, 1);
    following_leader = 1;
    if (leader[strlen(leader)-1] == '.')
        leader[strlen(leader)-1] = 0;
    send_to_user("-- [Pandora: Following leader : '%s'\r\n", leader);
    
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
        result = userland_parser.parse_user_input_line(buffer[i].line);
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
