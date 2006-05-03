//
// C++ Implementation: CRoom
//
// Description: 
//
//
// Author: Azazello <aza@alpha>, (C) 2005
//
// Copyright: See COPYING file that comes with this distribution
//
//
#include <QByteArray>
#include <QString>

#include "forwarder.h"
#include "CRoom.h"
#include "Map.h"
#include "configurator.h"
#include "tree.h"
#include "utils.h"

Strings_Comparator comparator;


const struct room_flag_data room_flags[] = {
  {"undefined", "UNDEFINED", EXIT_UNDEFINED},
  {"death", "DEATH", EXIT_DEATH},
    
  {NULL, NULL, EXIT_UNDEFINED}
};


CRoom::CRoom()
{
    int i;
    
    id = 0;
    name = NULL;
    note = NULL;
    desc = NULL;
    x = 0;
    y = 0;
    z = 0;
    sector = 0;
  
    for (i = 0; i <= 5; i++) {
	exits[i] = 0;
	doors[i] = NULL;
    }
}


CRoom::~CRoom()
{
    int i;
    
    printf("DESTRUCTOR!\r\n");
    NameMap.delete_item(name, id);
    
    if (name)
        delete name;
    if (note)
        delete note;
    if (desc)
        delete desc;
    
    for (i = 0; i <= 5; i++) {
	if (doors[i])
	   delete  doors[i];
    }
}


void CRoom::modified()
{
  conf.set_data_mod(true);
}

void CRoom::refresh_note(QByteArray n)
{
  if (note)
    delete note;
  note = qstrdup(n);
}

void CRoom::refresh_door(char dir, QByteArray d)
{
  if (doors[(int) dir])
    delete doors[(int) dir];
  if (d.isEmpty()) 
    doors[ (int) dir ] = NULL;
  else  
    doors[(int) dir] = qstrdup(d);
  modified();
}


/* implementation of desc comparison - simple strcmp at this moment */
void CRoom::refresh_desc(QByteArray newdesc)
{
  if (desc)
    delete desc;
  desc = qstrdup(newdesc);
  modified();
}

void CRoom::refresh_roomname(QByteArray newname)
{
  NameMap.delete_item(name, id);
  delete name;
  name = qstrdup(newname);
  NameMap.addname((const char *) newname, id);
  modified();
}


void CRoom::refresh_terrain(char terrain)
{
  sector = conf.get_sector_by_pattern(terrain);
  modified();      
}


int CRoom::desc_cmp(QByteArray d)
{ 
    if (desc)
        return comparator.strcmp_desc(d, desc);
    else
        return 0;
}

int CRoom::roomname_cmp(QByteArray n)
{ 
    if (name)
        return comparator.strcmp_roomname(n, name);
    else
        return 0;
}

/* --------------- check if exit in room is connected --------------- */
int CRoom::is_connected(int dir)
{
  if ((exits[dir] == EXIT_UNDEFINED) || (exits[dir] == EXIT_DEATH))
    return 0;
  if (exits[dir] > 0)
    return 1;

  return 0;
}

/* ------------------------ add_door() ------------------------*/
int CRoom::add_door(int dir, char *d)
{

  if (exits[dir] == 0) {
    exits[dir] = EXIT_UNDEFINED;
  }
    
  if (doors[dir] != NULL) 
    delete doors[dir];

  doors[dir] = qstrdup(d);
  
  modified();
  return 1;
}

/* ------------------------ remove_door() ------------------------*/
void CRoom::remove_door(int dir)
{
  if (doors[dir] != NULL) {
      delete doors[dir];
      doors[dir] = NULL;
  }
  
  modified();
}

char CRoom::dirbynum(int dir)
{
  switch (dir) {
	case  NORTH : 
                return 'n';
		break;
	case  SOUTH :
                return 's';
		break;
	case  EAST :
                return 'e';
		break;
	case  WEST :
                return 'w';
		break;
	case  UP :
                return 'u';
		break;
	case  DOWN :
                return 'd';
		break;
  }


  return -1;
}


void CRoom::setx(int nx)
{
  x = nx;
  modified();
}

void CRoom::sety(int ny)
{
  y = ny;
  modified();
}


void CRoom::setz(int nz)
{
  Map.remove_from_plane(this);
  z = nz;
  Map.add_to_plane(this);
  modified();
}

/* ------------------------------ prints the given room --------------------*/
void CRoom::send_room()
{
    unsigned int i, pos;
    char line[MAX_STR_LEN];
    
    send_to_user(" Id: %i, Flags: %s, Coord: %i,%i,%i\r\n", id,
	    (const char *) conf.sectors[sector].desc, x, y, z);
    send_to_user(" [32m%s[0m\n",name);

    line[0] = 0;
    pos = 0;
    if (!(proxy.isMudEmulation() && conf.get_brief_mode() ) ) {
      for (i = 0; i <= strlen(desc); i++)
	if (desc[i] == '|') {
	    line[pos] = 0;
	    send_to_user("%s\r\n", line);
	    line[0] = 0;
	    pos = 0;
	} else {
	    line[pos++] = desc[i];
	}
    }
    send_to_user(" note: %s\n", note);

    
    sprintf(line, "Doors:");
    for (i = 0; i <= 5; i++) {
      if (doors[i]) {
        sprintf(line + strlen(line), " %c: %s", dirbynum(i), doors[i]);
      }
    
    }
    send_to_user("%s\r\n", line);

    
    if (conf.get_brief_mode() && proxy.isMudEmulation()) {
      sprintf(line, "Exits: ");
      for (i = 0; i <= 5; i++)
          if (exits[i] > 0) {
              if (exits[i] == EXIT_UNDEFINED) {
                  sprintf(line + strlen(line), " #%s#", exitnames[i]);
                  continue;
              }
              if (exits[i] == EXIT_DEATH) {
                  sprintf(line + strlen(line), " !%s!", exitnames[i]);
                  continue;
              }
              if (doors[i] != NULL) {
                  if (strcmp("exit", doors[i]) == 0) {
                      sprintf(line + strlen(line), " (%s)", exitnames[i]);
                  } else {
                      sprintf(line + strlen(line), " +%s+", exitnames[i]);
                  }
              } else {
                  sprintf(line + strlen(line), " %s", exitnames[i]);
              }
          }
    
      
      
    } else {
      
      line[0] = 0;
      sprintf(line, " exits:");
  
      for (i = 0; i <= 5; i++)
          if (exits[i] > 0) {
              if (exits[i] == EXIT_UNDEFINED) {
                  sprintf(line + strlen(line), " #%s#", exitnames[i]);
                  continue;
              }
              if (exits[i] == EXIT_DEATH) {
                  sprintf(line + strlen(line), " !%s!", exitnames[i]);
                  continue;
              }
              if (doors[i] != NULL) {
                  if (strcmp("exit", doors[i]) == 0) {
                      sprintf(line + strlen(line), " (%s)", exitnames[i]);
                  } else {
                      sprintf(line + strlen(line), " +%s+", exitnames[i]);
                  }
              } else {
                  sprintf(line + strlen(line), " %s", exitnames[i]);
              }
              sprintf(line + strlen(line), " -[to %i]-", exits[i]);
          }
      
      
    }

    send_to_user("%s\r\n", line);
}



int Strings_Comparator::compare(QByteArray pattern, QByteArray text)
{
  int n, m, i, j;
  int cost;
  
  n = pattern.length();
  m = text.length();

  /* initialization */
  for (i = 0; i <= n; i++)
    D[i][0] = i;

  for (i = 0; i <= m; i++)
    D[0][i] = i;

  /* recurence */
  for (i = 1; i <= n; i++) 
    for (j = 1; j <= m; j++) 
    {
      cost = D[i - 1][j - 1];
      if (pattern[i] != text[j])
        cost += 1;
    
      D[ i ][ j ] = MIN( cost, MIN( D[i - 1][j ] + 1 , D[i][j - 1] + 1) );
    }
  
    
//  print_debug(DEBUG_ROOMS, "result of comparison : %i", D[n][m]);

  return D[n][m];
}


int Strings_Comparator::compare_with_quote(QByteArray str, QByteArray text, int quote)
{
    int n;
    int allowed_errors;
    int result;
    
    n = str.length();
    allowed_errors = (int) ( (double) quote / 100.0  * (double) n );
    
    result = compare(str, text);
    
    if (result == 0) 
        return 0;       /* they match ! */
    
    if (result <= allowed_errors)
        return result;  /* we are a little bit different from the strcmp function */
    
    return -1;  /* else the strings do not match */
}



int Strings_Comparator::strcmp_roomname(QByteArray name, QByteArray text)
{
    return compare_with_quote(name, text, conf.get_name_quote());
}


int Strings_Comparator::strcmp_desc(QByteArray name, QByteArray text)
{
    return compare_with_quote(name, text, conf.get_desc_quote());
}
