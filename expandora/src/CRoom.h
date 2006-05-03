//
// C++ Interface: croom
//
// Description: 
//
//
// Author: Azazello <aza@alpha>, (C) 2005
//
// Copyright: See COPYING file that comes with this distribution
//
//
#ifndef CROOM_H
#define CROOM_H

#include "defines.h"

#define EXIT_UNDEFINED  (MAX_ROOMS+1)
#define EXIT_DEATH      (MAX_ROOMS+2)


struct room_flag_data {
  char *name;
  char *xml_name;
  unsigned int flag;
};

enum ROOMFLAG_INDEX { ROOMFLAG_UNDEFINED = 0, ROOMFLAG_DEATH, ROOMFLAG_NONE};  
extern const struct room_flag_data room_flags[];

#define MAX_N   ( MAX_LINES_DESC * 80 )
#define MAX_M   ( MAX_LINES_DESC * 80 )


class Strings_Comparator {
    private:
        int D[ MAX_N ] [MAX_M ];
    public:
        int compare(QByteArray pattern, QByteArray text);
        int compare_with_quote(QByteArray str, QByteArray text, int quote);
        int strcmp_roomname(QByteArray name, QByteArray text);
        int strcmp_desc(QByteArray name, QByteArray text);

};

extern Strings_Comparator comparator;


class CRoom {
public:
    unsigned int id; 		/* identifier */
    char *name; 			/* POINTER to the room name */ 
    char *note; 			/* note, if needed, additional info etc */
    char *desc;			/* descrition */
    char sector;                 /* terrain marker */ 		
                                /* _no need to free this one_ */
    unsigned int exits[6];	/* 0 if no connection, id if there is any */ 
    char *doors[6];		/* if the door is secret */

  
    int x, y, z;		/* coordinates on our map */
  
    CRoom();
    ~CRoom();
    void modified();
    int is_connected(int dir);
    void send_room();
    
    int add_door(int dir, char *door);
    void remove_door(int dir);
    void setx(int x);
    void sety(int x);
    void setz(int x);
    int desc_cmp(QByteArray desc);
    int roomname_cmp(QByteArray name);
    void refresh_desc(QByteArray newdesc);
    void refresh_roomname(QByteArray newname);
    void refresh_terrain(char terrain);
    void refresh_note(QByteArray note);
    void refresh_door(char dir, QByteArray door);
    
    char dirbynum(int dir);
};

#endif
