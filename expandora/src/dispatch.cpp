#define MPI		"~$#E"	/* MUME protocol introducer */
#define MPILEN		4	/* strlen(MPI) */


#include <cstdio>
#include <cstring>
#include <qregexp.h>

#include <arpa/telnet.h>
//#define IAC 255

#include "defines.h"

#include <QMutex>


#include "configurator.h"

#include "utils.h"
#include "tree.h"
#include "stacks.h"
#include "xml2.h"
#include "dispatch.h"
#include "engine.h"

#include "userfunc.h"
#include "forwarder.h"

class Cdispatcher dispatcher;

Cdispatcher::Cdispatcher() 
{
    amount = 0;
    xmlMode = false;
}



/**
*   This function was adopted from powwow code Copyright (C) 1998 by Massimiliano Ghilardi
*/
int Cdispatcher::telnetSeqLength()
{
    int i;
    unsigned char *s, *start;
    char state = T_GOTIAC;
    
    i = o_len - o_pos + 1;
    start = (unsigned char *) &o_buf[o_pos];
    for (s = start;  i; s++, i--) {
	switch (state) {
	 case T_NORMAL:
	    return (s - start);
	    break;
	    
	 case T_GOTIAC:
	    switch (*s) {
	     case WILL:
	     case WONT:
	     case DO:
	     case DONT:
		state = T_SKIP; 
		break;
	     case SB:		/* BUG (multiple connections):	*/
		state = T_GOTSB;	/* there is only one subopt buffer */
		break;
	     case IAC:
	     case GA:
	     default:
		state = T_NORMAL;
		break;
	    }
	    break;
	    
	 case T_SKIP:
	    state = T_NORMAL;
	    break;
	    
	 case T_GOTSB:
	    if (*s == IAC) {
		state = T_GOTSBIAC;
	    } 
	    break;
	    
	 case T_GOTSBIAC:
	    if (*s == IAC) {
		state = T_GOTSB;
	    } else if (*s == SE) {
		state = T_NORMAL;
	    } else {
		/* problem! I haven't the foggiest idea of what to do here.
		 * I'll just ignore it and hope it goes away. */
		state = T_NORMAL;
	    }
	    break;
	}
    }
    
     return (s - start);
}


/**
 * 
 */
void Cdispatcher::parse_xml() 
{
        /* types without parameters here */
        struct {
            char    *name;
            int        startType;
            int        endType;
        } TagTypes[] = {
            {"prompt", XML_START_PROMPT, XML_END_PROMPT},
            {"room", XML_START_ROOM, XML_END_ROOM},
            {"name", XML_START_NAME, XML_END_NAME},
            {"description", XML_START_DESC, XML_END_DESC},
            {"exits", XML_START_EXITS, XML_END_EXITS},
            {"terrain", XML_START_TERRAIN, XML_END_TERRAIN},
            {"", -1, -1}
        };
        
        int p;
        int i;
        char    name[MAX_STR_LEN];
        char    params[MAX_STR_LEN];
        bool    endTag = false;
        bool    endAfterTag = false;
        int       endType = -1;
        int       startType = -1;
                    
        name[0] = 0;
        params[0] = 0;
        
        p = o_pos+1;
        if (o_buf[p] == '/') {
            endTag = true;
            p++;
        }
        
        /* get tags name */
        i = 0;
        while (p < o_len && o_buf[p] != '>' && o_buf[p] != ' ' && o_buf[p] != '/') {
            name[i++] = o_buf[p];
            p++;
        }        
        name[i] = 0;
        
        if (o_buf[p] == ' ') {
            /* params are coming */
            i = 0;
            while (p < o_len && o_buf[p] != '>' && o_buf[p] != '/') {
                params[i++] = o_buf[p];
                p++;
            }        
            params[i] = 0;
        }
        
//        printf("params: ..%s.. ", params);
        
        if (o_buf[p] == '/') {
            endAfterTag = true;
            p++;
        }
        
        if (o_buf[p] != '>') {
            printf("Fault in XML parsing !\r\n");
            while (p < o_len && o_buf[p] != '>')  {
                printf("%c", o_buf[p]);
                p++;
            }        
            params[0] = 0;
            printf("\r\n");
        }
                
        /* we are done with moving around the original buffer */
        o_pos = p+1;
//        printf("new o_pos %i, next char %c\r\n", o_pos, o_buf[o_pos]);        
        
        /* now parse the tags ! */                    
        
        /* hard tags first */
        if (strcmp(name, "movement") == 0) {
  //          printf("XML TAG: %s\r\n", TagTypes[p].name);
            startType = XML_START_MOVEMENT;
            endType = XML_END_MOVEMENT;
            
            /* parse parameters */
            if (strlen(params) != 0) {
                QByteArray param = params;
                int index = param.indexOf("dir=");
                index += 4;
                switch (param[index]) {
                    case 'n' :    strcpy(params, "north");
                                        break;
                    case 'e' :    strcpy(params, "east");
                                        break;
                    case 's' :    strcpy(params, "south");
                                        break;
                    case 'w' :    strcpy(params, "west");
                                        break;
                    case 'u' :    strcpy(params, "up");
                                        break;
                    case 'd' :    strcpy(params, "down");
                                        break;
                }
            }
        } else {
            /* the easy ones */
            for (p = 0; TagTypes[p].startType != -1; p++) {
                if (strcmp(TagTypes[p].name, name) == 0) {
//                    printf("XML TAG: %s\r\n", TagTypes[p].name);
                    startType = TagTypes[p].startType;
                    endType = TagTypes[p].endType;
                }
            }
        }
        
        buffer[amount].type = IS_XML;
        strcpy(buffer[amount].line, params);
        buffer[amount].len = strlen(params);
        if (endTag)
            buffer[amount].xmlType = endType;
        else    
            buffer[amount].xmlType = startType;
        amount++;
        
        if (endAfterTag) {
            buffer[amount].type = IS_XML;
            buffer[amount].len = 0;
            buffer[amount].xmlType = endType;
            amount++;
        }
        
}
    
    

void Cdispatcher::dispatch_buffer(bool Xml) 
{
  int l;
  char line[MAX_DATA_LEN];
  QRegExp rx;
  
  
  line[0] = 0;  /* set first terminator */
  amount = 0;
  l = 0;

  while (o_pos < o_len) {
    
    /* telnet sequences filter (or rather anti-filer, we let them go as they are */
    if ((unsigned char)o_buf[o_pos] == (unsigned char) IAC) {
      if (l != 0) {
        /* in case we met this telnet sequence right in the middle */
        line[l] = 0;
        memcpy(buffer[amount].line, line, l);
        buffer[amount].len = l;   
        buffer[amount].type = IS_NONE;  /* line without end - will be just coppied as is */
          
        amount++;
        l = 0;
      }
      int seqLen = telnetSeqLength();
      /* ok, go on with the telnet sequence */  
      memcpy(buffer[amount].line, &o_buf[o_pos], seqLen);
      buffer[amount].line[seqLen] = 0;
      buffer[amount].len = seqLen;
      o_pos += seqLen;
      buffer[amount].type = IS_TELNET;
      amount++;

      l = 0;
      continue;
    }
    
    if (xmlMode && Xml) {
        /* XML sequences check */
        if (o_buf[o_pos] == '<') {
            if ( l !=0 ) {      // the tag stands as ending for a line without new-line breaks, so ... 
                line[l] = 0;
                memcpy(buffer[amount].line, line, l);
                buffer[amount].type = IS_NONE;
                buffer[amount].line[l] = 0;
                buffer[amount].len = l;
                amount++;
        
                l = 0;
            }        
            
            parse_xml();
            continue;
        }
        
        if (o_buf[o_pos] == '&' && (o_pos <= (o_len-3)) ) {
            if (o_buf[o_pos+1] == 'g' && o_buf[o_pos+2] == 't' && o_buf[o_pos+3] == ';') {
                    line[l++] = '>';
                    o_pos += 4;
                    continue;
            } else   if (o_buf[o_pos+1] == 'l' && o_buf[o_pos+2] == 't' && o_buf[o_pos+3] == ';') {
                    line[l++] = '<';
                    o_pos += 4;
                    continue;
            } 
        }    
            
        if (o_buf[o_pos] == '&' && (o_pos <= (o_len-4)) ) {
            if (o_buf[o_pos+1] == 'a' && o_buf[o_pos+2] == 'm'  && o_buf[o_pos+3] == 'p' && o_buf[o_pos+4] == ';') {
                    line[l++] = '&';
                    o_pos += 4;
                    continue;
            } /*else if (o_buf[o_pos+1] == 'a' && o_buf[o_pos+2] == 'p'  && o_buf[o_pos+3] == 'o' && o_buf[o_pos+4] == 's' && o_buf[o_pos+5] == ';') {
                    line[l++] = '\\';
                    o_pos += 4;
            }else if (o_buf[o_pos+1] == 'q' && o_buf[o_pos+2] == 'u'  && o_buf[o_pos+3] == 'o' && o_buf[o_pos+4] == 't' && o_buf[o_pos+5] == ';') {
                    line[l++] = '"';
                    o_pos += 4;
            }*/
            continue;
        }
    }
        
        
    /* for forward check buf[i+1] */
    if (o_pos <= (o_len - 1)) {
      if ((o_buf[o_pos] == 0xd) && (o_buf[o_pos + 1] == 0xa)) {
          line[l] = 0;
          o_pos += 2;
          
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

  buffer[amount].type = IS_PROMPT;
  memcpy(buffer[amount].line, line, l);
  buffer[amount].len = l;
  buffer[amount].line[l] = 0;
  
  /* ???? this is some silly attempt to catch splitted lines */
  /* just some hack, i assume it works only with certain MTU, */
  /* most likely normal TCP mtu though */
  if (o_len == 1440) {
      buffer[amount].type = IS_SKIP;
  } else {
      buffer[amount].type = IS_SKIP;
  }
  amount++;
  return;	/* warning awaiting routine that buffer is over */
}

QByteArray Cdispatcher::cutColours(char *line)
{
    QByteArray res;
    unsigned int i;
    bool skip = false;
    
    for (i =0; i < strlen(line); i++) {
        if (line[i+1] == 0x8 && (line[i] == '|' || line[i] == '-' || line[i] == '\\' || line[i] == '/')) {
            i += 1; /*properller char*/
            continue;   /* and the next one and move on with the same check */
        }    
    
        if (line[i] == 0x6d && skip) {
            skip = false;
            continue;
        } 
        if (skip) 
            continue;
        if (line[i] == 0x1b && line[i+1] == 0x5b) {
            skip = true;
            continue;
        }
        res.append(line[i]);
    }
    
//    res.append('/0');
//    printf("Line: %s\r\n", line);
//    printf("
    
    return res;
}

#define SEND_EVENT_TO_ENGINE \
                    {   \
                    printf("SENDING event to analyzer!\r\n");   \
                    Engine.add_event(event);                            \
                    event.clear();                  \
                    notify_analyzer();      \
                    state = STATE_NORMAL;                       \
                    }
        

void Cdispatcher::analyze_mud_stream(char *buf, int *n) 
{
    int i;
    int new_len;
    bool awaitingData;
    
    o_buf = buf;
    o_len = *n;
    o_pos = 0;
    new_len = 0;
    Event event;
    
  
    buf[*n] = 0;
    state = STATE_NORMAL;
    mbrief_state = STATE_NORMAL;
    awaitingData = false;
        
    dispatch_buffer(true);
  
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
        /* preset flags for m-brief mode */
    
    
        /* XML messages parser */
        if (buffer[i].type == IS_XML) {
            
            if (buffer[i].xmlType == XML_START_MOVEMENT) {
                if (awaitingData) 
                    SEND_EVENT_TO_ENGINE;
                event.dir = buffer[i].line;
                continue;
            } else if (buffer[i].xmlType == XML_START_ROOM) {
                if (awaitingData) 
                    SEND_EVENT_TO_ENGINE;
                state = STATE_ROOM;                
                continue;
            } else if ((buffer[i].xmlType == XML_START_NAME) && (state == STATE_ROOM)) {
                state = STATE_NAME;
                continue;
            } else if ((buffer[i].xmlType == XML_START_DESC)  && (state == STATE_ROOM)) {
                state = STATE_DESC;
                continue;
            } else if ((buffer[i].xmlType == XML_START_TERRAIN)  && (state == STATE_ROOM)) {
                event.blind = true;                 /* BLIND detection */
                continue;
            } else if (buffer[i].xmlType == XML_START_EXITS) {
                state = STATE_EXITS;
                continue;
            } else if (buffer[i].xmlType == XML_START_PROMPT) {
                state = STATE_PROMPT;
                continue;
            } else if (buffer[i].xmlType == XML_END_MOVEMENT) {
                /* nada */
                continue;
            } else if (buffer[i].xmlType == XML_END_ROOM && state == STATE_ROOM) {
                awaitingData = true;
                state = STATE_NORMAL;
                continue;
            } else if (buffer[i].xmlType == XML_END_NAME && state == STATE_NAME) {
                state = STATE_ROOM;
                continue;
            } else if (buffer[i].xmlType == XML_END_DESC && state == STATE_DESC) {
                state = STATE_ROOM;
                continue;
            } else if (buffer[i].xmlType == XML_END_EXITS && state == STATE_EXITS) {
                SEND_EVENT_TO_ENGINE;
                awaitingData = false;
                state = STATE_NORMAL;
                continue;
            } else if (buffer[i].xmlType == XML_END_PROMPT) {
                SEND_EVENT_TO_ENGINE;
//                awaitingData = false;
                state = STATE_NORMAL;
                continue;
            } 
                    
            continue;
        }
        
        /* necessary stuff */        
        switch (state) {
            case STATE_NAME : 
                                                event.name = cutColours( buffer[i].line );
                                                break;
            case STATE_DESC :
                                                event.desc.append( cutColours(buffer[i].line) + "|");        
                                                if (conf.get_brief_mode())
                                                    continue;
                                                break;
            case STATE_EXITS:
                                                event.exits = cutColours( buffer[i].line );
                                                int index = event.exits.indexOf("Exits: ");
                                                if (index == -1) {
                                                    event.exits = "";
                                                    break;
                                                }
                                                event.exits.replace("Exits: ", "");
                                                printf("XML exits data: %s\r\n", (const char *) event.exits);
                                                break;
            case STATE_PROMPT:
                                                spells_print_mode = false;      /* do not analyze spells anymore */
                                                printf("XML prompt: %s\r\n", (const char *) cutColours( buffer[i].line ) );
                                                Engine.set_prompt(cutColours( buffer[i].line ));
                                                event.terrain = parse_terrain(cutColours( buffer[i].line ));
                                                break;
        };
        
        /* mbrief additional check (for look/scout and similar) */
        if (mbrief_state == STATE_DESC && conf.get_brief_mode()) 
            continue;
        
        if (buffer[i].type == IS_CRLF) {
            /*
            print_debug(DEBUG_DISPATCHER, "line after fixing: %s", a_line);
            */
            QByteArray a_line = cutColours( buffer[i].line );          
          
            if (!xmlMode) {
                if (a_line == "Reconnecting." || a_line =="Never forget! Try to role-play...") {
                    printf( "XML MODE IS NOW ON!\r\n");
                    xmlMode = true;
                }
            } else {
                if (a_line == "</xml>") {
                    printf( "XML MODE IS NOW OFF!\r\n");
                    xmlMode = false;
                }
            }
          
            /* now do all necessary spells checks */
            {
                unsigned int p;
            
//                printf("Checking spells on %s.\r\n", a_line);
                for (p = 0; p < conf.spells.size(); p++) {
                    if ( (conf.spells[p].up_mes != "" && conf.spells[p].up_mes == a_line) || 
                         (conf.spells[p].refresh_mes != "" && conf.spells[p].refresh_mes == a_line )) {
                        printf("SPELL %s Starting/Restaring timer.\r\n",  (const char *) conf.spells[p].name);
                        conf.spells[p].timer.start();   /* start counting */
                        conf.spells[p].up = true;        
                        break;
                    }
                    
                    /* if some spell is up - only then we check if its down */
                    if (conf.spells[p].up && conf.spells[p].down_mes != "" && conf.spells[p].down_mes == a_line) {
                        conf.spells[p].up = false;
                        printf("SPELL: %s is DOWN. Uptime: %s.\r\n", (const char *) conf.spells[p].name, 
                                                qPrintable( conf.spell_up_for(p) ) );
                        break;                                    
                    }
                }
            
                            
                            
                if (spells_print_mode && (strlen(a_line) > 3)) {
                    /* we are somewhere between the lines "Affected by:" and prompt */
                    for (p = 0; p < conf.spells.size(); p++) {
//                        printf("Spell name %s, line %s\r\n", (const char *) conf.spells[p].name, (const char*) a_line );    
                        if (a_line.indexOf(conf.spells[p].name) == 2) {
                            QString s;
                            
                            if (conf.spells[p].up)
                                s = QString("- %1 (up for %2)")
                                    .arg( (const char *)conf.spells[p].name )
                                    .arg( conf.spell_up_for(p) );
                            else 
                                s = QString("- %1 (unknown time)")
                                    .arg( (const char *)conf.spells[p].name );
 
                            memcpy(buf + new_len, qPrintable(s), s.length());
                            new_len += s.length();
                            memcpy(buf + new_len, "\r\n", 2);
                            new_len += 2;
                            
                            break;    
                        }
                    }
                    if (p != conf.spells.size())
                        continue; /* dont print this line if we got a match for it */
                        
                        
                }
            
            }
          
            if (conf.spells_pattern == a_line) {
                unsigned int spell;
                QByteArray message = "Timers:";
                
                spells_print_mode = true;   /* print the spells data */
                /* addon timers first */
                memcpy(buf + new_len, (const char *) message, message.length());
                new_len += message.length();
                memcpy(buf + new_len, "\r\n", 2);
                new_len += 2;

                for (spell = 0; spell < conf.spells.size(); spell++) 
                    if (conf.spells[spell].addon && conf.spells[spell].up) {
                        /* there is a timer ticking */
                        QString s;
                           
                        s = QString("- %1 (up for %2)")
                            .arg( (const char *)conf.spells[spell].name )
                            .arg( conf.spell_up_for(spell) );

                        memcpy(buf + new_len, qPrintable(s), s.length());
                        new_len += s.length();
                        memcpy(buf + new_len, "\r\n", 2);
                        new_len += 2;
                        break;
                    }
            
            }
		
        }            
        
        /* this is for normal prompt detection */
        if (buffer[i].type == IS_PROMPT) {
            spells_print_mode = false;      /* do not analyze spells anymore */
            Engine.set_prompt(buffer[i].line);
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


char Cdispatcher::parse_terrain(QByteArray prompt)
{
    char terrain;
        
    terrain = prompt[1];  /*second charecter is terrain*/
    if (conf.get_sector_by_pattern(terrain) == 0) 
        return -1;
    
    return terrain;
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

    dispatch_buffer(false);
  
    buf[0]=0;
    for (i = 0; i< amount; i++) {

      if (buffer[i].type == IS_CRLF || buffer[i].type == IS_LFCR ||
          buffer[i].type == IS_LF) {
        result = userland_parser.parse_user_input_line(buffer[i].line);
        if (result == USER_PARSE_SKIP) 
          continue;
        if (result == USER_PARSE_DONE) {

          buffer[i].len = strlen(buffer[i].line);
        }
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
