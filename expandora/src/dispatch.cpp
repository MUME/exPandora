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
#include "dispatch.h"
#include "engine.h"

#include "userfunc.h"
#include "forwarder.h"

class Cdispatcher dispatcher;

Cdispatcher::Cdispatcher() 
{
    amount = 0;
}


int Cdispatcher::check_exits(char *line)
{
    QRegExp rx;

    rx = conf.get_exits_exp();
    if (rx.indexIn(line) >= 0) {
//        Engine.add_event(R_EXITS, 
//                         &line[ strlen( (const char*) conf.get_exits_pat() ) ]);
//        print_debug(DEBUG_DISPATCHER, "EXITS: %s [%i]", &line[6], strlen(&line[6]) );
//        getting_desc = 0;   /* do not react on desc alike strings anymore */
//        notify_analyzer();
        return 1;
    }
    
    return 0;
}


/* we know we have prompt in line and want to put the resulting, patched prompt in buf, returning the */
/* length of the dispatched prompt */
/* mode == 1 means handle the IAC ending too, rest means ignore it */
int Cdispatcher::dispatch_prompt(char *line, char *buf, int l, int mode)
{
    int rn_start;
    int rn_end;
    int len;
    QByteArray s;

//    printf("investigating the line ..%s..\r\n", line);

//    printf("Flags: IAC %i, forwardIAC %i, forwardColour %i, mode %i\r\n", 
//            conf.is_prompt_IAC(), conf.is_forward_IAC(), conf.is_forwardPromptColour(), mode);
    /* zap the prompt colour, if needed */
    if (!conf.is_forwardPromptColour() && conf.get_prompt_col() != "") {
        s = conf.get_prompt_col();
        rn_start = s.length();
                        
        if (strncmp(line, (const char *) s, rn_start) == 0) {
            s = conf.get_end_col();
            rn_end = s.length();
            len = l - rn_start - rn_end;
                        
            memcpy(buf, &line[rn_start], len);
        } else {
            memcpy(buf, line, l);
            len = l;
        }
    } else {
        memcpy(buf, line, l);
        len = l;
    }
    
    if (mode == 1) {
        if (conf.is_forward_IAC() && conf.is_prompt_IAC()) {
            buf[len] = (unsigned char) IAC ;
            buf[len+1] = (unsigned char) 0xf9;
            len = len + 2;
        } else {
            len = l;
        }
    }
        
    buf[len] = 0;
            
    return len;
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
            {"propmp", XML_START_PROMPT, XML_END_PROMPT},
            {"room", XML_START_ROOM, XML_END_ROOM},
            {"name", XML_START_NAME, XML_END_NAME},
            {"description", XML_START_DESC, XML_END_DESC},
            {"exits", XML_START_EXITS, XML_END_EXITS},
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
        
        printf("XML TAG: ");
        
        p = o_pos+1;
        if (o_buf[p] == '/') {
            printf(" (END TAG) ");
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
        
        printf("name: ..%s.. ", name);
        
        if (o_buf[p] == ' ') {
            /* params are coming */
            i = 0;
            while (p < o_len && o_buf[p] != '>' && o_buf[p] != '/') {
                params[i++] = o_buf[p];
                p++;
            }        
            params[i] = 0;
        }
        
        printf("params: ..%s.. ", params);
        
        if (o_buf[p] == '/') {
            printf("EndAfterTag ");
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
        printf("new o_pos %i, next char %c\r\n", o_pos, o_buf[o_pos]);        
        
        /* now parse the tags ! */                    
        
        /* hard tags first */
        if (strcmp(name, "movement") == 0) {
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
    
    
    

void Cdispatcher::dispatch_buffer() 
{
  int l;
  char line[MAX_DATA_LEN];
  QRegExp rx;
  int rx_pos;
  
  
  line[0] = 0;  /* set first terminator */
  amount = 0;
  l = 0;

  while (o_pos < o_len) {
    
    
    if ((unsigned char)o_buf[o_pos] == (unsigned char) IAC && 
        (unsigned char)o_buf[o_pos + 1] == (unsigned char)0xf9 &&
        conf.is_prompt_IAC() ) 
    {
        line[l] = 0;
        rx = conf.get_prompt_exp();
//        printf("IAC check RegExp: %s LINE: ..%s..\r\n", qPrintable( rx.pattern() ), line);
            
        if ((rx_pos = rx.indexIn(line)) >= 0) {
            buffer[amount].type = IS_PROMPT;
            buffer[amount].len = dispatch_prompt(line, buffer[amount].line, l, 1);
            printf("PROMPT(IAC): %s\r\n", buffer[amount].line);
            
            o_pos += 2;    /* its a match, so move the pointer futher */
            amount++;
            l = 0;
            continue;
        }
    }
    
    
    /* telnet sequences filter (or rather anti-filer, we let them go as they are */
    if ((unsigned char)o_buf[o_pos] == (unsigned char) IAC) {
      if (l != 0) {
        /* in case we met this damn telnet sequence right in the middle */
        /* of some other line. (which looks pretty much impossible) */
        line[l] = 0;
        memcpy(buffer[amount].line, line, l);
        buffer[amount].len = l;   
        buffer[amount].type = IS_NONE;  /* line without end - will be just coppied as is */
//        printf("Telnet seq ..%s..!\r\n", buffer[amount].line );
          
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

//      printf("The continuing line ..%s..!\r\n", buffer[amount].line);


      l = 0;
      continue;
    }
    
    /* XML sequences check */
    if (o_buf[o_pos] == '<') {
        parse_xml();
        continue;
    }
    
    if (o_buf[o_pos] == '&' && (o_len - o_pos) >= 4 && o_buf[o_pos+2] == 't' && o_buf[o_pos+3] == ';') {
        if (o_buf[o_pos+1] == 'g') {
                line[l++] = '>';
                o_pos += 4;
        } else   if (o_buf[o_pos+1] == 'l' ) {
                line[l++] = '<';
                o_pos += 4;
        }
        continue;
    }
    
    /* for forward check buf[i+1] */
    if (o_pos <= (o_len - 1)) {
      if ((o_buf[o_pos] == 0xd) && (o_buf[o_pos + 1] == 0xa)) {
          line[l] = 0;
          o_pos += 2;
          
    
          rx = conf.get_prompt_exp();
          if ( conf.get_prompt_col() != "" && 
               (strncmp(line, (const char *) conf.get_prompt_col(), conf.get_prompt_col_len() ) == 0) )   
            if ((rx_pos = rx.indexIn(line)) >= 0) {
              rx_pos += rx.matchedLength();
                        /* here we assume we've found prompt */
              memcpy(buffer[amount].line, line, rx_pos);
              buffer[amount].len = dispatch_prompt(line, buffer[amount].line, rx_pos, 0);
              buffer[amount].type = IS_CRLF;
              printf(" fixed prompt is : ..%s..\r\n", buffer[amount].line);
                
              amount++;
                                    
              strcpy(buffer[amount].line, line + rx_pos);
              buffer[amount].len = strlen(buffer[amount].line);
              buffer[amount].type = IS_CRLF;
              printf(" fixed line is : ..%s..\r\n", buffer[amount].line);
              amount++;
                                    
              printf("------------------Fixed bug with prompt in line!\r\n");
              l = 0;
              continue;
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


    rx = conf.get_prompt_exp();
//    printf("RegExp: %s LINE: ..%s..\r\n", qPrintable( rx.pattern() ), line);

    if ((rx_pos = rx.indexIn(line)) >= 0) {
        buffer[amount].len = dispatch_prompt(line, buffer[amount].line, l, 0);
        buffer[amount].type = IS_PROMPT;
        printf("PROMPT (outside of the cycle): %s\r\n", buffer[amount].line);
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

QByteArray Cdispatcher::cutColours(char *line)
{
    QByteArray res;
    unsigned int i;
    bool skip = false;
    
    for (i =0; i < strlen(line); i++) {
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
    
    o_buf = buf;
    o_len = *n;
    o_pos = 0;
    new_len = 0;
    Event event;
    
  
    buf[*n] = 0;
    state = STATE_NORMAL;
    awaitingRoom = false;

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
        /* XML messages parser */
        if (buffer[i].type == IS_XML) {
            if (buffer[i].xmlType == XML_START_MOVEMENT) {
                event.dir = buffer[i].line;
                awaitingRoom = true;
                continue;
            } else if (buffer[i].xmlType == XML_START_ROOM && awaitingRoom) {
                state = STATE_ROOM;                
                awaitingRoom = false;
                continue;
            } else if ((buffer[i].xmlType == XML_START_NAME) && (state == STATE_ROOM)) {
                state = STATE_NAME;
                continue;
            } else if ((buffer[i].xmlType == XML_START_DESC)  && (state == STATE_ROOM)) {
                state = STATE_DESC;
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
                SEND_EVENT_TO_ENGINE;
                state = STATE_NORMAL;
                continue;
            } else if (buffer[i].xmlType == XML_END_NAME && state == STATE_NAME) {
                state = STATE_ROOM;
                continue;
            } else if (buffer[i].xmlType == XML_END_DESC && state == STATE_DESC) {
                state = STATE_ROOM;
                continue;
            } else if (buffer[i].xmlType == XML_END_EXITS && state == STATE_NORMAL) {
                state = STATE_NORMAL;
                continue;
            } else if (buffer[i].xmlType == XML_END_PROMPT) {
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
//                                                event.exits = cutColours( buffer[i].line );
                                                break;
            case STATE_PROMPT:
                                                Engine.set_prompt(cutColours( buffer[i].line ));
                                                break;
        };
        
        
        if (buffer[i].type == IS_CRLF) {
            /* before any checks cut away the "propeller chars */
            char a_line[MAX_DATA_LEN];
            char *k, *j;
            
            a_line[0] = 0;
            for (k = buffer[i].line, j = a_line; *k; k++) {
              if (*(k+1) && *(k+1) == 0x8 && 
                    (*k == '|' || *k == '-' || *k == '\\' || *k == '/') ) {
                print_debug(DEBUG_DISPATCHER, "Fixing propeller chars in line %s", k);
                k += 2;
              }
              
              *(j++) = *k;
            }
            
            *j = 0;
            
            /*
            print_debug(DEBUG_DISPATCHER, "line after fixing: %s", a_line);
            */
          
          
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
                        printf("Spell name %s, line %s\r\n", (const char *) conf.spells[p].name, &a_line[2] );    
                        if (conf.spells[p].name == &a_line[2]) {
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

        if (buffer[i].type == IS_PROMPT) {
            spells_print_mode = false;      /* do not analyze spells anymore */
            Engine.set_prompt(buffer[i].line);
//            Engine.add_event(R_PROMPT, buffer[i].line);
//            notify_analyzer();
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
