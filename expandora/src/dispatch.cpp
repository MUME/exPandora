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
}

/**
 * 
 */
void Cdispatcher::parse_xml(QByteArray tag) 
{
        /* types without parameters here */
        /*
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
        
        // get tags name 
        i = 0;
        while (p < o_len && o_buf[p] != '>' && o_buf[p] != ' ' && o_buf[p] != '/') {
            name[i++] = o_buf[p];
            p++;
        }        
        name[i] = 0;
        
        if (o_buf[p] == ' ') {
            // params are coming 
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
                
        // we are done with moving around the original buffer 
        o_pos = p+1;
//        printf("new o_pos %i, next char %c\r\n", o_pos, o_buf[o_pos]);        
        
        // now parse the tags !                     
        
        // hard tags first 
        if (strcmp(name, "movement") == 0) {
  //          printf("XML TAG: %s\r\n", TagTypes[p].name);
            startType = XML_START_MOVEMENT;
            endType = XML_END_MOVEMENT;
            
            // parse parameters 
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
            // the easy ones 
            for (p = 0; TagTypes[p].startType != -1; p++) {
                if (strcmp(TagTypes[p].name, name) == 0) {
//                    printf("XML TAG: %s\r\n", TagTypes[p].name);
                    startType = TagTypes[p].startType;
                    endType = TagTypes[p].endType;
                }
            }
        }
        
        buffer[amount].line = params;
        if (endTag)
            buffer[amount].type = endType;
        else    
            buffer[amount].type = startType;
        amount++;
        
        if (endAfterTag) 
            buffer[amount++].type = endType;
*/        
}
    
    
#define STUFFING_CLEANUP  \
            {   \
                printf("Stuffing cleanup!\r\n"); \
                line.append(c.subchars);  \
                c.subchars.clear(); \
                line.append(*s);    \
                c.mainState = NORMAL; \
                c.subState = NORMAL; \
            }   
    
void Cdispatcher::dispatch_buffer(ProxySocket &c) 
{
    QByteArray line;
    unsigned char *s;
    unsigned char *stop;
  
    line.clear();
    amount = 0;

    /* put back the leftovers */
    if (c.fragment != "") {
        printf("Adding lost fragment!\r\n");
        line = c.fragment;
        amount++;
    }        

    stop = (unsigned char *) (c.buffer + c.length);
    for (s = (unsigned char *) c.buffer;  s != stop; s++) {
        printf("[ amount %i - m %i, s %i,  s %i, left %i ] ", amount, c.mainState, c.subState, s, stop - s);
	switch (c.mainState) {
	   case NORMAL :
	                            switch (*s) {
	                               case IAC:
                                                            if (line != "") {
                                                                buffer[amount++].line = line;
                                                                line.clear();
                                                            }
                                                            c.mainState = TELNET;
                                                            c.subState = T_GOTIAC;
                                                            break;                                                                
	                                       
                                        case 0x0a :  // LF 
                                                            printf("Appending %i : %c [newline] \r\n", s, *s);
                                                            line.append(*s);
                                                            buffer[amount++].line = line;
                                                            buffer[amount].type = IS_NORMAL;
                                                            line.clear();
                                                            continue;
                                      case '<'      :  // turns XML tag mode on 
                                                            if (c.isXmlMode()) {
                                                                if (line != "") {
                                                                    buffer[amount++].line = line;
                                                                    line.clear();
                                                                }
                                                                c.subchars.append('<');
                                                                c.mainState = XML;
                                                            }
                                                            continue;
                                                                                                      
                                        case '&'      :
                                                            if (c.isXmlMode()) {
                                                                c.subchars.append('&');
                                                                c.subState = AMP;
                                                                c.mainState = STUFFING;
                                                            }
                                                            continue;
	                            }
	
	                            break;
           case  XML : 
                                    switch (*s) {
                                        case '>' : 
                                                        c.subchars.append('>');
                                                        parse_xml(c.subchars);
                                                        c.mainState = NORMAL;
                                                        c.subState = NORMAL;
                                                        c.subchars.clear();
                                                        continue;
                                        default:
                                                        c.subchars.append(*s);
                                                        continue;                                                        
                                    }
                                    break;
           
           
           case STUFFING :
                                        printf("STUFFING state %i, char : %c, subchars %s \r\n", c.subState, *s, (const char *) c.subchars );
                                        switch (c.subState) {
                                            case                AMP:
                                                                        switch (*s) {
                                                                            case 'l' :
                                                                                c.subchars.append('l');
                                                                                c.subState = L;                                                                         
                                                                                continue;
                                                                            case 'g' :
                                                                                c.subchars.append('g');
                                                                                c.subState = G;                                                                         
                                                                                continue;
                                                                            case 'a' :
                                                                                c.subchars.append('a');
                                                                                c.subState = A;                                                                         
                                                                                continue;
                                                                            case 'q' :
                                                                                c.subchars.append('q');
                                                                                c.subState = Q;                                                                         
                                                                                continue;
                                                                            
                                                                            default:
                                                                                STUFFING_CLEANUP;
                                                                                continue;
                                                                        }
                                            case                 G:
                                                                        if (*s == 't') {
                                                                            c.subchars.append('t');
                                                                            c.subState = LASTCHAR;
                                                                        }  else STUFFING_CLEANUP;
                                                                        continue;
                                            case                 L:
                                                                        if (*s == 't') {
                                                                            c.subchars.append('t');
                                                                            c.subState = LASTCHAR;
                                                                        } else STUFFING_CLEANUP;
                                                                        continue;
                                            case                 A:
                                                                        if (*s == 'm') {
                                                                            c.subchars.append('m');
                                                                            c.subState = A_M;
                                                                        } else STUFFING_CLEANUP;
                                                                        continue;
                                            case                 A_M:
                                                                        if (*s == 'p') {
                                                                            c.subchars.append('p');
                                                                            c.subState = LASTCHAR;
                                                                        } else STUFFING_CLEANUP;
                                                                        continue;
                                            case                 LASTCHAR:
                                                                        if (*s == ';') {
                                                                            if (c.subchars == "&gt") 
                                                                                line.append('>');
                                                                            else if (c.subchars == "&lt") 
                                                                                line.append('<');
                                                                            else if (c.subchars == "&amp") 
                                                                                line.append('&'); 
                                                                             c.subState = NORMAL;
                                                                             c.mainState = NORMAL;
                                                                             c.subchars.clear();
                                                                        } else STUFFING_CLEANUP;
                                                                        continue;
                                        }
           case TELNET:
                                switch (c.subState) {
                                    case T_NORMAL:
                                        if (line != "") {
                                            buffer[amount++].line = line;
                                            buffer[amount].type = IS_NORMAL;
                                            line.clear();
                                        }
                                        s--;        // return to the same char we are standing at, but in proper mode
                                        c.mainState = NORMAL;
                                        c.subState = NORMAL;
                                        continue;
                                        
                                    case T_GOTIAC:
                                        buffer[amount].type = IS_DATA;
                                        switch (*s) {
                                                case WILL:
                                                case WONT:
                                                case DO:
                                                case DONT:
                                                    c.subState = T_SKIP; 
                                                    break;
                                                case SB:		// BUG (multiple connections):	
                                                    c.subState = T_GOTSB;	// there is only one subopt buffer 
                                                    break;
                                                case IAC:
                                                case GA:
                                                default:
                                                    c.subState = T_NORMAL;
                                                    break;
                                        }
                                        break;
                                        
                                    case T_SKIP:
                                        c.subState = T_NORMAL;
                                        break;
                                        
                                    case T_GOTSB:
                                        if (*s == IAC) {
                                            c.subState = T_GOTSBIAC;
                                        } 
                                        break;
                                        
                                    case T_GOTSBIAC:
                                        if (*s == IAC) {
                                            c.subState = T_GOTSB;
                                        } else if (*s == SE) {
                                            c.subState = T_NORMAL;
                                            break;
                                        } else {
                                            c.subState = T_NORMAL;
                                        }
                                        break;
                                 }
                                    
                                    break;                        	                           
        }	
        printf("Appending %i : %c\r\n", s, *s);
        line.append(*s);
    }
    
    printf("Amount : %i.\r\n", amount);
    buffer[amount++].line = line;
    
    for (int i = 0; i < amount; i++) 
        printf("Line: %s", (const char *) buffer[i].line);
    
    c.fragment.clear();
    c.subchars.clear();
}

QByteArray Cdispatcher::cutColours(QByteArray line)
{
    QByteArray res;
    unsigned int i;
    bool skip = false;
    
    for (i =0; i < strlen(line); i++) {
        if (line.at(i+1) == 0x8 && (line.at(i) == '|' || line.at(i) == '-' || line.at(i) == '\\' || line.at(i) == '/')) {
            i += 1; /*properller char*/
            continue;   /* and the next one and move on with the same check */
        }    
        
        if (line.at(i) == 0xa || line.at(i) == 0xd)
            continue;                                       /* skip newlines */
    
        if (line.at(i) == 0x6d && skip) {
            skip = false;
            continue;
        } 
        if (skip) 
            continue;
        if (line.at(i) == 0x1b && line.at( i+1 ) == 0x5b) {
            skip = true;
            continue;
        }
        res.append(line.at(i));
    }
    
    return res;
}

#define SEND_EVENT_TO_ENGINE \
                    {   \
                    printf("SENDING event to analyzer!\r\n");   \
                    Engine.add_event(event);                            \
                    event.clear();                  \
                    notify_analyzer();      \
                    xmlState = STATE_NORMAL;                       \
                    }
        

int Cdispatcher::analyze_mud_stream(ProxySocket &c) 
{
    int i;
    bool awaitingData;
    int new_len;
    char *buf;
    
    
    Event event;
    
    xmlState = STATE_NORMAL;
    mbrief_state = STATE_NORMAL;
    awaitingData = false;
    
    printf("---------- mud input -----------\r\n");
        
        
    printf("Buffer size %i\r\n", c.length);
    dispatch_buffer(c);
    
    buf = c.buffer;
    new_len = 0;
    
    // else we simply recreate our buffer and parse lines 
    for (i = 0; i< amount; i++) {
    
    
        //XML messages parser 
        if (buffer[i].type == IS_XML) {
            
            if (buffer[i].xmlType == XML_START_MOVEMENT) {
                if (awaitingData) 
                    SEND_EVENT_TO_ENGINE;
                event.dir = buffer[i].line;
                continue;
            } else if (buffer[i].xmlType == XML_START_ROOM) {
                if (awaitingData) 
                    SEND_EVENT_TO_ENGINE;
                xmlState = STATE_ROOM;                
                continue;
            } else if ((buffer[i].xmlType == XML_START_NAME) && (xmlState == STATE_ROOM)) {
                xmlState = STATE_NAME;
                continue;
            } else if ((buffer[i].xmlType == XML_START_DESC)  && (xmlState == STATE_ROOM)) {
                xmlState = STATE_DESC;
                continue;
            } else if ((buffer[i].xmlType == XML_START_TERRAIN)  && (xmlState == STATE_ROOM)) {
                event.blind = true;                 // BLIND detection 
                continue;
            } else if (buffer[i].xmlType == XML_START_EXITS) {
                xmlState = STATE_EXITS;
                continue;
            } else if (buffer[i].xmlType == XML_START_PROMPT) {
                xmlState = STATE_PROMPT;
                continue;
            } else if (buffer[i].xmlType == XML_END_MOVEMENT) {
                // nada 
                continue;
            } else if (buffer[i].xmlType == XML_END_ROOM && xmlState == STATE_ROOM) {
                awaitingData = true;
                xmlState = STATE_NORMAL;
                continue;
            } else if (buffer[i].xmlType == XML_END_NAME && xmlState == STATE_NAME) {
                xmlState = STATE_ROOM;
                continue;
            } else if (buffer[i].xmlType == XML_END_DESC && xmlState == STATE_DESC) {
                xmlState = STATE_ROOM;
                continue;
            } else if (buffer[i].xmlType == XML_END_EXITS && xmlState == STATE_EXITS) {
                SEND_EVENT_TO_ENGINE;
                awaitingData = false;
                xmlState = STATE_NORMAL;
                continue;
            } else if (buffer[i].xmlType == XML_END_PROMPT) {
                SEND_EVENT_TO_ENGINE;
//                awaitingData = false;
                xmlState = STATE_NORMAL;
                continue;
            } 
                    
            continue;
        }
        
        switch (xmlState) {
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
                                                    break;
                                                }
                                                event.exits.replace("Exits: ", "");
                                                printf("XML exits data: %s\r\n", (const char *) event.exits);
                                                break;
            case STATE_PROMPT:
                                                spells_print_mode = false;      // do not analyze spells anymore 
                                                printf("XML prompt: %s\r\n", (const char *) cutColours( buffer[i].line ) );
                                                Engine.set_prompt(cutColours( buffer[i].line ));
                                                event.terrain = parse_terrain(cutColours( buffer[i].line ));
                                                break;
        };
        
        // mbrief additional check (for look/scout and similar) 
        if (mbrief_state == STATE_DESC && conf.get_brief_mode()) 
            continue;
        
        if (buffer[i].type == IS_NORMAL) {
            QByteArray a_line = cutColours( buffer[i].line );          
          
            if (!c.isXmlMode()) {
                if (a_line == "Reconnecting." || a_line =="Never forget! Try to role-play...") {
                    printf( "XML MODE IS NOW ON!\r\n");
                    c.setXmlMode( true );
                }
            } else {
                if (a_line == "</xml>") {
                    printf( "XML MODE IS NOW OFF!\r\n");
                    c.setXmlMode( false );
                }
            }
          
            // now do all necessary spells checks 
            {
                unsigned int p;
            
//                printf("Checking spells on %s.\r\n", a_line);
                for (p = 0; p < conf.spells.size(); p++) {
                    if ( (conf.spells[p].up_mes != "" && conf.spells[p].up_mes == a_line) || 
                         (conf.spells[p].refresh_mes != "" && conf.spells[p].refresh_mes == a_line )) {
                        printf("SPELL %s Starting/Restaring timer.\r\n",  (const char *) conf.spells[p].name);
                        conf.spells[p].timer.start();   // start counting 
                        conf.spells[p].up = true;        
                        break;
                    }
                    
                    // if some spell is up - only then we check if its down 
                    if (conf.spells[p].up && conf.spells[p].down_mes != "" && conf.spells[p].down_mes == a_line) {
                        conf.spells[p].up = false;
                        printf("SPELL: %s is DOWN. Uptime: %s.\r\n", (const char *) conf.spells[p].name, 
                                                qPrintable( conf.spell_up_for(p) ) );
                        break;                                    
                    }
                }
            
                            
                            
                if (spells_print_mode && (strlen(a_line) > 3)) {
                    // we are somewhere between the lines "Affected by:" and prompt 
                    for (p = 0; p < conf.spells.size(); p++) {
//                        printf("Spell name %s, line %s\r\n", (const char *) conf.spells[p].name, (const char*) a_line );    
                        if (a_line.indexOf(conf.spells[p].name) == 2) {
                            QString s;
                            
                            if (conf.spells[p].up)
                                s = QString("- %1 (up for %2)\r\n")
                                    .arg( (const char *)conf.spells[p].name )
                                    .arg( conf.spell_up_for(p) );
                            else 
                                s = QString("- %1 (unknown time)\r\n")
                                    .arg( (const char *)conf.spells[p].name );
 
                            memcpy(buf + new_len, qPrintable(s), s.length());
                            new_len += s.length();
                            
                            break;    
                        }
                    }
                    if (p != conf.spells.size())
                        continue; // dont print this line if we got a match for it 
                        
                        
                }
            
            }
          
            if (conf.spells_pattern == a_line) {
                unsigned int spell;
                QByteArray message = "Timers:";
                
                spells_print_mode = true;   // print the spells data 
                // addon timers first 
                memcpy(buf + new_len, (const char *) message, message.length());
                new_len += message.length();
                memcpy(buf + new_len, "\r\n", 2);
                new_len += 2;

                for (spell = 0; spell < conf.spells.size(); spell++) 
                    if (conf.spells[spell].addon && conf.spells[spell].up) {
                        // there is a timer ticking 
                        QString s;
                           
                        s = QString("- %1 (up for %2)\r\n")
                            .arg( (const char *)conf.spells[spell].name )
                            .arg( conf.spell_up_for(spell) );

                        memcpy(buf + new_len, qPrintable(s), s.length());
                        new_len += s.length();
                        break;
                    }
            
            }
		
        }            
        
        if (buffer[i].type == IS_PROMPT) {
            spells_print_mode = false;      // 
            Engine.set_prompt(buffer[i].line);
        }
        
        // recreating this line in buffer 
        memcpy(buf + new_len, buffer[i].line, buffer[i].line.length());
        new_len += buffer[i].line.length();
    }
  
    printf("New length: %i\r\n", new_len);
    return new_len;
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
int Cdispatcher::analyze_user_stream(ProxySocket &c) 
{
    int i, result;
    int new_len;
    char *buf;
    
    buf = c.buffer;
    new_len = 0;
    
    printf("---------- user input -----------\r\n");
    printf("Buffer size %i\r\n", c.length);
    printf("the buffer. %s\r\n", c.buffer);
    printf("        MainState %i, subState %i   \r\n", c.mainState, c.subState);
    dispatch_buffer(c);
    
    for (i = 0; i< amount; i++) {
        if (buffer[i].type == IS_NORMAL) {
            result = userland_parser.parse_user_input_line(buffer[i].line);
            if (result == USER_PARSE_SKIP) 
                continue;
        }
            
        // recreating this line in buffer 
        memcpy(buf + new_len, buffer[i].line, buffer[i].line.length());
        new_len += buffer[i].line.length();
    }
        
    printf("Resulting buffer length: %i\r\n", new_len);
        
    return new_len;
}
