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

    char roomdesc[PROXY_BUFFER_SIZE];
    char getting_desc;          /* desc shall be incoming - just got roomname */
    QByteArray get_colour(QByteArray str);      
    QByteArray get_colour_name(QByteArray str);
    
    int check_roomname(char *line);
    int check_description(char *line);
    
    int check_exits(char *line);
    int check_failure(char *nline);
    
    int dispatch_prompt(char *line, char *buf, int l, int mode);
    
    bool spells_print_mode; /* After "Affected by:" until next prompt */
	
    enum XmlTypes { XML_START_ROOM, XML_START_NAME, XML_START_DESC,  XML_START_PROMPT, XML_START_EXITS, 
                                   XML_START_MOVEMENT,                                      
                                   XML_END_ROOM, XML_END_NAME, XML_END_DESC,  XML_END_PROMPT, XML_END_EXITS, 
                                   XML_END_MOVEMENT};


    void parse_xml();
public:
    void  analyze_mud_stream(char *buf, int *n);
    void analyze_user_stream(char *buf, int *n);
    
    void dispatch_buffer(); 
	
    Cdispatcher();
};

extern class Cdispatcher dispatcher;

#endif
