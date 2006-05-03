#ifndef CDISPATCHER_H
#define CDISPATCHER_H


#include <vector>
#include <QString>
#include "forwarder.h"
#include "engine.h"

using namespace std;

struct Tincoming_lines {
    QByteArray              line;
    unsigned short int   type;
    unsigned short int   xmlType;
};


class Cdispatcher
{
    struct Tincoming_lines buffer[1024];
    int amount;
    char    commandBuffer[MAX_DATA_LEN];
    
    int       xmlState;          /* desc shall be incoming - just got roomname */
    int       mbrief_state;
    bool     awaitingData;
    Event  event;

    enum dispatcherStates { STATE_NORMAL = 0, 
                                               STATE_ROOM, 
                                               STATE_DESC, 
                                               STATE_NAME, 
                                               STATE_PROMPT, 
                                               STATE_EXITS };
    
    bool spells_print_mode; /* After "Affected by:" until next prompt */
	
    enum LineTypes {IS_NORMAL    = 0,
                                   IS_XML,
                                   IS_SKIP,
                                   IS_DATA,
                                   IS_PROMPT,
                                   };

    enum XmlTypes {
                                   XML_START_ROOM = 1, 
                                   XML_START_NAME = 2, 
                                   XML_START_DESC = 3,  
                                   XML_START_PROMPT = 4, 
                                   XML_START_EXITS = 5, 
                                   XML_START_MOVEMENT = 6, 
                                   XML_START_TERRAIN = 7,                                       
                                   XML_END_ROOM = 11, 
                                   XML_END_NAME = 12, 
                                   XML_END_DESC = 13,  
                                   XML_END_PROMPT = 14, 
                                   XML_END_EXITS = 15, 
                                   XML_END_MOVEMENT = 16, 
                                   XML_END_TERRAIN = 17
                                 };

    enum MainStates {
        NORMAL = 0,
        XML, 
        TELNET,
        STUFFING
    };
    
    
    
    enum SubStatesNormal {
        CR,
        LF,
    };
    
    enum StuffingStates {
        AMP,
        G,
        L,
        A,
        A_M,
        Q,
        Q_U,
        Q_U_O,
        LASTCHAR
     };
     
     
     enum SubStatesTelnet {
        T_NORMAL, 
        T_GOTIAC, 
        T_GOTWILL, 
        T_GOTWONT, 
        T_GOTDO, 
        T_GOTDONT, 
        T_GOTSB, 
        T_GOTSBIAC,
        T_SKIP,
     };

    char parse_terrain(QByteArray prompt);


    void parse_xml(QByteArray tag);
public:
    
    int  analyze_mud_stream(ProxySocket &c);
    int  analyze_user_stream(ProxySocket &c);
    
    QByteArray cutColours(QByteArray line);
    QByteArray get_colour(QByteArray str);      
    QByteArray get_colour_name(QByteArray str);

    void  dispatch_buffer(ProxySocket &c);

    
    Cdispatcher();
};

extern class Cdispatcher dispatcher;

#endif
