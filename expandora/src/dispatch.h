#ifndef CDISPATCHER_H
#define CDISPATCHER_H


#include <vector>
#include <QString>
using namespace std;

#define IS_PROMPT       1
#define IS_CRLF         2
#define IS_LFCR         3
#define IS_LF           4       /* only linefeed lines */
#define IS_SPLIT        5
#define IS_SKIP         6       
#define IS_TELNET       7       /* telnet protocol commands */
#define IS_NONE         8       /* is not an ascii line */
#define IS_XML          9

struct Tincoming_lines {
    char line[MAX_DATA_LEN];
    unsigned short int  type;
    short                        xmlType;
    unsigned short int  len;
};


class Cdispatcher
{
    char *o_buf;
    int o_len;
    int o_pos;
	
    struct Tincoming_lines buffer[600];
    int amount;
    
    bool    xmlMode;
    int       state;          /* desc shall be incoming - just got roomname */
    int       mbrief_state;
    enum dispatcherStates { STATE_NORMAL = 0, 
                                               STATE_ROOM, 
                                               STATE_DESC, 
                                               STATE_NAME, 
                                               STATE_PROMPT, 
                                               STATE_EXITS };
    
    bool spells_print_mode; /* After "Affected by:" until next prompt */
	
    enum XmlTypes { XML_START_ROOM = 1, 
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

    enum telnetStates {T_NORMAL, 
                                       T_ALTNORMAL, 
                                       T_GOT_N, 
                                       T_GOT_R, 
                                       T_GOTIAC, 
                                       T_GOTWILL, 
                                       T_GOTWONT, 
                                       T_GOTDO, 
                                       T_GOTDONT, 
                                       T_GOTSB, 
                                       T_GOTSBIAC,
                                       T_SKIP};

    int telnetSeqLength();
    int process_message(char *buf, int len);


    void parse_xml();
public:
    void  analyze_mud_stream(char *buf, int *n);
    void analyze_user_stream(char *buf, int *n);
    QByteArray cutColours(char *line);
    QByteArray get_colour(QByteArray str);      
    QByteArray get_colour_name(QByteArray str);

    void dispatch_buffer(bool Xml); 
    
    void setXmlMode(bool b)     {xmlMode = b; }
    bool getXmlMode()               { return xmlMode; }

    
	
    Cdispatcher();
};

extern class Cdispatcher dispatcher;

#endif
