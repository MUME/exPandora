#ifndef ENGINE_H
#define ENGINE_H

#include <QQueue>

#include "CRoom.h"



class Event   {
    public:
        Event() {}
        Event(const Event &other) 
        {
            dir = other.dir;
            name = other.name;
            desc = other.desc;
            exits = other.exits;
        }

        Event &operator=(const Event &other) 
        {
            if (this != &other) {  // make sure not same object
                dir = other.dir;
                name = other.name;
                desc = other.desc;
                exits = other.exits;
            }
            return *this;    // Return ref for multiple assignment            
        }

        void clear()    {
            dir = "";
            name = "";
            desc = "";
            exits = "";
        }

        QByteArray dir;
        QByteArray name;
        QByteArray desc;
        QByteArray exits;
};

class CEngine {
  /* flags */
    bool done;                    /* finish */       
    bool addingroom;              /* adding room right now */
    bool mapping;                 /* mapping is On/OFF */
    bool gettingfirstroom;        /* getting the very first room in base */
    bool mgoto;


    QByteArray last_name;
    QByteArray last_desc;
    QByteArray last_exits;
    QByteArray last_prompt;
    char last_terrain;
  
    QQueue<Event> Pipe;

    Event        event;

    void run();
    void engine_init(); /* init flags */

    void parse_event();
    void tryAllDirs();
    void tryDir();
    void swap();
    void resync();
    void applyExits();
    void applyDesc();
    void applyName();
    void mappingoff();
    
    bool testRoom(CRoom *room);
public:
    CEngine();
    ~CEngine();

    CRoom *addedroom;	/* new room, contains new data is addinrroom==1 */
    
    void add_event(Event e);

    int parse_command_line(char cause, char result, char *line);

    void exec();       
    
    int check_roomdesc();
    void angrylinker(CRoom *r);
    void printstacks();
    
    QByteArray get_roomname() { return last_name; }
    QByteArray get_desc() { return last_desc; }
    QByteArray get_exits() { return last_exits; }
    QByteArray get_prompt() { return last_prompt; }
    char get_terrain() { return last_terrain; }

    void set_roomname(QByteArray s) { last_name = s; }
    void set_desc(QByteArray s) { last_desc = s; }
    void set_exits(QByteArray s) { last_exits = s; }
    void set_prompt(QByteArray s) { last_prompt = s; }
    void set_terrain(char c) { last_terrain = c; }

    bool isMapping() { return mapping; }
    void setMapping(bool b) { mapping = b; }
    bool isAddingroom() { return addingroom; }
    void setAddingroom(bool b) { addingroom = b; }
    
    void setMgoto(bool b) { mgoto = b; }
    bool isMgoto() { return mgoto; }
    bool empty();                      /* are pipes empty? */
    void clear();                      /* clears events pipes */
    
};

extern class CEngine Engine;

#endif

