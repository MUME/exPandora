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
            blind = other.blind;
            terrain = other.terrain;
            prompt = other.prompt;
        }

        Event &operator=(const Event &other) 
        {
            if (this != &other) {  // make sure not same object
                dir = other.dir;
                name = other.name;
                desc = other.desc;
                exits = other.exits;
                blind = other.blind;
                terrain = other.terrain;
                prompt = other.prompt;
            }
            return *this;    // Return ref for multiple assignment            
        }

        void clear()    {
            dir = "";
            name = "";
            desc = "";
            exits = "";
            prompt = "";
            blind = false;
            terrain = -1;
        }

        QByteArray dir;
        QByteArray name;
        QByteArray desc;
        QByteArray exits;
        bool             blind;         /* fog, no light, blind flag */
        char             terrain;
        QByteArray prompt;
};

class CEngine {
  /* flags */
    bool mapping;                 /* mapping is On/OFF */
    bool mgoto;
    int   nameMatch;
    int   descMatch;
    
    CRegion  *users_region;
    CRegion  *last_region;
    
    QByteArray last_name;
    QByteArray last_desc;
    QByteArray last_exits;
    QByteArray last_prompt;
    char last_terrain;
  
    QQueue<Event> Pipe;

    Event        event;

    void parseEvent();
    void tryAllDirs();
    void tryDir();
    void swap();
    void resync();
    void mappingOff();
    
    bool testRoom(CRoom *room);
    
public:
    CEngine();
    ~CEngine();

    CRoom *addedroom;	/* new room, contains new data is addinrroom==1 */
    
    void addEvent(Event e);

    int parseCommandLine(char cause, char result, char *line);

    void exec();       
    
    int checkRoomDesc();
    void angryLinker(CRoom *r);
    void printStacks();
    
    QByteArray getRoomName() { return last_name; }
    QByteArray getDesc() { return last_desc; }
    QByteArray getExits() { return last_exits; }
    QByteArray getPrompt() { return last_prompt; }
    char getTerrain() { return last_terrain; }

    void setRoomname(QByteArray s) { last_name = s; }
    void setDesc(QByteArray s) { last_desc = s; }
    void setExits(QByteArray s) { last_exits = s; }
    void setPrompt(QByteArray s) { last_prompt = s; }
    void setTerrain(char c) { last_terrain = c; }

    bool isMapping() { return mapping; }
    void setMapping(bool b) { mapping = b; }
    
    void setMgoto(bool b) { mgoto = b; }
    bool isMgoto() { return mgoto; }
    bool empty();                      /* are pipes empty? */
    void clear();                      /* clears events pipes */
    
    void set_users_region(CRegion *reg);
    void set_last_region(CRegion *reg);
    CRegion *get_users_region();
    CRegion *get_last_region();

    void updateRegions();
};

extern class CEngine Engine;

#endif

