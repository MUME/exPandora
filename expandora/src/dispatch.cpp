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

Cdispatcher::Cdispatcher() 
{
    amount = 0;
    roomdesc[0]=0;
    getting_desc = 0;
    leader[0] = 0;        /* no leader yet */
    following_leader = false;
    follow_leader_exp.setPattern("You now follow ");
}

int Cdispatcher::check_roomname(char *line) {
    char roomname[MAX_STR_LEN];
    QRegExp rx;
    

    rx = conf.get_roomname_exp();

    if (rx.indexIn(line) >= 0) {
        /* so we got a roomname in this line */
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
            
            Engine.add_event(R_ROOM, roomname);
            notify_analyzer();
        
            getting_desc = 1;   /* get desc if it will be there */
            
            return 1;
        
    } 
      
    return 0;
}

int Cdispatcher::check_exits(char *line)
{
    QRegExp rx;

    rx = conf.get_exits_exp();
    if (rx.indexIn(line) >= 0) {
        Engine.add_event(R_EXITS, 
                         &line[ strlen( (const char*) conf.get_exits_pat() ) ]);
        print_debug(DEBUG_DISPATCHER, "EXITS: %s [%i]", &line[6], strlen(&line[6]) );
        getting_desc = 0;   /* do not react on desc alike strings anymore */
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
            
//    printf("Resulting line ..%s..\r\n", buf);        
    getting_desc = 0;   /* if we miss exits, then prompt turns this off */ 
    spells_print_mode = false;   
    return len;
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
            
        if (rx_pos = rx.indexIn(line) >= 0) {
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
    
    /* for forward check buf[i+1] */
    if (o_pos <= (o_len - 1)) {
      if ((o_buf[o_pos] == 0xd) && (o_buf[o_pos + 1] == 0xa)) {
          line[l] = 0;
          o_pos += 2;
          
    
          rx = conf.get_prompt_exp();
          if ( conf.get_prompt_col() != "" && 
               (strncmp(line, (const char *) conf.get_prompt_col(), conf.get_prompt_col_len() ) == 0) )   
            if (rx_pos = rx.indexIn(line) >= 0) {
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

    if (rx_pos = rx.indexIn(line) >= 0) {
        buffer[amount].len = dispatch_prompt(line, buffer[amount].line, l, 0);
        buffer[amount].type = IS_PROMPT;
        printf("PROMPT: %s\r\n", buffer[amount].line);
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
            Engine.add_event(R_DESC, roomdesc);
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
                    if (conf.spells[p].up_mes == a_line || 
                        (conf.spells[p].refresh_mes == a_line && conf.spells[p].refresh_mes != "")) {
                        printf("SPELL %s Starting/Restaring timer.\r\n",  (const char *) conf.spells[p].name);
                        conf.spells[p].timer.start();   /* start counting */
                        conf.spells[p].up = true;        
                        break;
                    }
                    
                    /* if some spell is up - only then we check if its down */
                    if (conf.spells[p].up && conf.spells[p].down_mes == a_line) {
                        conf.spells[p].up = false;
                        printf("SPELL: %s is DOWN. Uptime: %s.\r\n", (const char *) conf.spells[p].name, 
                                                qPrintable( conf.spell_up_for(p) ) );
                        break;                                    
                    }
                }
            
                            
                            
                if (spells_print_mode) {
                    /* we are somewhere between the lines "Affected by:" and prompt */
                    for (p = 0; p < conf.spells.size(); p++) {
//                        printf("Spell name %s, line %s\r\n", (const char *) conf.spells[p].name, &a_line[2] );    
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
		
             		
            
            if (check_roomname(a_line) == 0) 
                if (check_exits(a_line) == 0)
                    if (check_failure(a_line) == 0)
                        {
                            /* none */
                        }
                        
                        
                        
        }            

        if (buffer[i].type == IS_PROMPT) {
            Engine.set_prompt(buffer[i].line);
            Engine.add_event(R_PROMPT, buffer[i].line);
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
    QRegExp rx;
    vector<QByteArray>::iterator i;

    if (getting_colour_scheme) {
        rx.setPatternSyntax(QRegExp::Wildcard);
	
      	if (!collecting_colours) {
            /* check if we shall start collecting colours */
	    rx.setPattern("Use 'change colour <field> <attribute>' where <field>");
            if (rx.indexIn(nline) >= 0) {
	        collecting_colours = true;
	        return 1;
	    }
        } else {
            rx.setPattern("or 'change colour <field> default|monochrome|none' where <field>");
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
        Engine.add_event(conf.patterns[i].event.type, (const char*) conf.patterns[i].event.data);
      return 1;
    }         
  }


  if (follow_leader_exp.indexIn(nline) >= 0) {
    unsigned int j;
        
    for (j = follow_leader_exp.matchedLength(); j < MAX_STR_LEN; j++) {
        if (nline[j] == '\0' || nline[j] == '.')
            break;
        if (nline[j] == '(') {
            j--;
           break;
       }
    }
    leader = QByteArray(nline).mid(follow_leader_exp.matchedLength(), j - follow_leader_exp.matchedLength());
        

    you_follow_exp.setPattern("You follow (" + leader + "|Someone)");                                         

    QByteArray r = "";
    r.append( UPPER(leader[0]) );
    leader.replace(0, 1, r);  /* capitalize the first letter */
    
    leader_moves_exp.setPattern(leader + ".* leaves (north|east|south|west|up|down)" );
                                         
    following_leader = 1;
    send_to_user("-- [Pandora: Following leader : '%s'\r\n", (const char *) leader);
    send_to_user("---[ Pattern: %s  Pattern: %s\r\n", qPrintable( leader_moves_exp.pattern() ), 
                            qPrintable( you_follow_exp.pattern() )  );

    return 1;
  }

  if (following_leader) {
    
    if (leader_moves_exp.indexIn(nline) >= 0) {
        unsigned int j;
        for (j = leader_moves_exp.matchedLength() - 1; j > 0; j--) 
            if (nline[j] == ' ')
                break;
      
        last_leaders_movement = nline[ j + 1 ];
        print_debug(DEBUG_DISPATCHER, "Saved leaders movement %c.", last_leaders_movement);
        return 1;
    }
    if (you_follow_exp.indexIn(nline) >= 0) {

      print_debug(DEBUG_DISPATCHER, "Effective leaders movement %c.", last_leaders_movement);
      if ( Engine.isMapping() ) {
          Engine.setMapping(false);
          send_to_user("--[Pandora: Mapping is now OFF!\r\n");
      }
      
      switch (last_leaders_movement) {
        case 'n' : Engine.add_event(C_MOVE, "north");
                break;
        case 'e' : Engine.add_event(C_MOVE, "east");
                break;
        case 's' : Engine.add_event(C_MOVE, "south");
                break;
        case 'w' : Engine.add_event(C_MOVE, "west");
                break;
        case 'u' : Engine.add_event(C_MOVE, "up");
                break;
        case 'd' : Engine.add_event(C_MOVE, "down");
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
