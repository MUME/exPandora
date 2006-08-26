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
#include "engine.h"
#include "CRoom.h"
#include "Map.h"
#include "configurator.h"
#include "tree.h"
#include "utils.h"

Strings_Comparator comparator;


const struct room_flag_data room_flags[] = {
  {"undefined", "UNDEFINED", CRoom::EXIT_UNDEFINED},
  {"death", "DEATH", CRoom::EXIT_DEATH},
    
  {"", "", CRoom::EXIT_NONE}
};


CRoom::CRoom(unsigned int newid)
{
    int i;
    
    id = newid;
    name = NULL;
    note = NULL;
    desc = NULL;
    sector = 0;
    region = NULL;
    flags = 0;
  
    for (i = 0; i <= 5; i++) {
        exits[i] = NULL;
	exitFlags[i] = 0;
	doors[i].clear();
    }
    
    node = renderer->addRoom( this );
}


CRoom::~CRoom()
{
    int i;
    
    NameMap.deleteItem(name, id);
    renderer->removeRoom( this );
    
    for (i = 0; i <= 5; i++) {
        doors[i].clear();
    }
}

void CRoom::setRoomNode(Ogre::SceneNode *newNode)
{
    node = newNode;
}

Ogre::SceneNode *CRoom::getRoomNode()
{
    return node;
}


void CRoom::setModified(bool b)
{
    if (b)
        conf.set_data_mod(true);
}

int CRoom::descCmp(QByteArray d)
{ 
    if (desc.isEmpty() != true)
        return comparator.strcmp_desc(d, desc);
    else
        return 0;
}

int CRoom::roomnameCmp(QByteArray n)
{ 
    if (name.isEmpty() != true)
        return comparator.strcmp_roomname(n, name);
    else
        return 0;
}



/* --------------- check if exit in room is connected --------------- */
bool CRoom::isConnected(int dir)
{
    if (isExitUndefined(dir)  || isExitDeath(dir) )
        return false;
    if (exits[dir] != NULL)
        return true;

    return false;
}


/* is there anything at all in this direction ? */
bool CRoom::isExitPresent(int dir)
{
    if (exits[dir] != NULL)
        return true;
    if (isExitUndefined(dir)  || isExitDeath(dir) )
        return true;
    return false;    
}

bool CRoom::isExitLeadingTo(int dir, CRoom *room)
{
    if (exits[dir] == NULL)
        return false;
    if (exits[dir]->id == room->id)
        return true;
    return false;
}


/* ------------------------ add_door() ------------------------*/
int CRoom::setDoor(int dir, QByteArray d)
{
    if (exits[dir] == NULL) {
        exitFlags[dir] = EXIT_UNDEFINED;
    }
    
    doors[dir] = d;
  
    setModified(true);
    return 1;
}

/* ------------------------ remove_door() ------------------------*/
void CRoom::removeDoor(int dir)
{
    doors[dir].clear();
    setModified(true);
}

QByteArray CRoom::getDoor(int dir)
{
    return doors[dir];
}

bool CRoom::isDoorSet(int dir)
{
    if (doors[dir].isEmpty())
        return false;
    else 
        return true;
        
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


void CRoom::setX(int nx)
{
    Ogre::Vector3 pos;
    
    pos = node->getPosition();
    pos.x = nx;
    node->setPosition(pos);
    setModified(true);
}

void CRoom::setY(int ny)
{
    Ogre::Vector3 pos;
    
    pos = node->getPosition();
    pos.y = ny;
    node->setPosition(pos);
    setModified(true);
}


void CRoom::setZ(int nz)
{
    Ogre::Vector3 pos;
    
    pos = node->getPosition();
    pos.z = nz;
    node->setPosition(pos);
    setModified(true);
}



int CRoom::getX()
{
    return node->getPosition().x;
}

int CRoom::getY()
{
    return node->getPosition().y;
}

int CRoom::getZ()
{
    return node->getPosition().z;
}


QByteArray CRoom::getName()
{
    return name;
}


QByteArray CRoom::getDesc()
{
    return desc;
}


char CRoom::getTerrain()
{
    return sector;
}


QByteArray CRoom::getNote()
{
    return note;
}

void CRoom::setDesc(QByteArray newdesc)
{
    desc = newdesc;
    setModified(true);    
}
      
QByteArray CRoom::getSecretsInfo()
{
    int i;
    QByteArray res;
    QByteArray alias;
    
    res.clear();
    
    for (i = 0; i <= 5; i++) 
        if (isDoorSecret( i ) == true) {
            res.append(dirbynum( i ));
            res = res + ": " + doors[i];
            alias = Engine.get_users_region()->getAliasByDoor(doors[i], i);
            if (alias.isEmpty() == false) 
                res += "[" + alias + "]";
        }

    return res;
}

void CRoom::setName(QByteArray newname)
{
    NameMap.deleteItem(name, id);
    name = newname;
    NameMap.addName(newname, id);
    setModified(true);
}


void CRoom::setTerrain(char terrain)
{
    sector = conf.get_sector_by_pattern(terrain);
    setModified(true);
}

void CRoom::setSector(char val) 
{
    sector = val;
}


void CRoom::setNote(QByteArray newnote)
{
    note = newnote;
}


void CRoom::setExit(int dir, CRoom *room)
{
    exits[dir] = room;
    exitFlags[dir] = EXIT_NONE;
}

void CRoom::setExit(int dir, unsigned int value)
{
    exits[dir]=Map.getRoom(value);
    exitFlags[dir] = EXIT_NONE;
}

/*
CRoom *CRoom::getExit(int dir)
{
    return exits[dir];
}
*/

bool CRoom::isExitUndefined(int dir)
{
    if (exitFlags[dir] == EXIT_UNDEFINED)
        return true;
    else
        return false;

}

void CRoom::setExitUndefined(int dir)
{
    exits[dir] = NULL;
    exitFlags[dir] = EXIT_UNDEFINED;  
//    SET_BIT(exitFlags[dir], EXIT_UNDEFINED);
}



bool CRoom::isExitDeath(int dir)
{
    if (exitFlags[dir] == EXIT_DEATH)
        return true;
    else
        return false;

}

bool CRoom::isExitNormal(int dir)
{
    if (isExitUndefined(dir)  || isExitDeath(dir) )
        return false;
    else
        return true;

}


void CRoom::setExitFlags(int dir, unsigned char flag)
{
//    SET_BIT(exitFlags[dir], flag);
    exitFlags[dir] = flag;
    
    setModified(true);
}

void CRoom::setExitDeath(int dir) 
{
    exitFlags[dir] = EXIT_DEATH;
    exits[dir] = NULL;    
    setModified(true);
}



bool CRoom::anyUndefinedExits()
{
    int i;
    
    for (i = 0; i <= 5; i++) 
        if (exitFlags[i] == EXIT_UNDEFINED) {
            i = 1000;
            break;
        }
        
    if (i == 5) 
        return true;     /* no need to try and link this room - there are no undefined exits */
   
    return false;
}

bool CRoom::isEqualNameAndDesc(CRoom *room)
{
    if ((desc == room->getDesc()) && (name == room->getName()))
        return true;
    return false;
}


bool CRoom::isDescSet()
{
    if (desc.isEmpty() == true)
        return false;
    return true;
}

bool CRoom::isNameSet()
{
    if (name.isEmpty() == true)
        return false;
    return true;
}


bool CRoom::isDoorSecret(int dir)
{
    if (doors[dir].isEmpty() != true && doors[dir] != "exit")
        return true;
    else 
        return false;
}

QByteArray CRoom::getRegionName()
{
    return region->getName();
}

void CRoom::setRegion(QByteArray name)
{
    if (name == "")
        setRegion(Map.getRegionByName("default"));
    else 
        setRegion(Map.getRegionByName(name)); 
}

void CRoom::setRegion(CRegion *reg)
{
    if (reg != NULL)
        region = reg;
}
    
CRegion *CRoom::getRegion()
{
    return region;
}

void CRoom::disconnectExit(int dir)
{
    exitFlags[dir] = EXIT_NONE;
    exits[dir] = NULL;
}

void CRoom::removeExit(int dir)
{
    exitFlags[dir] = EXIT_NONE;
    exits[dir] = NULL;
    doors[dir].clear();
}


/* ------------------------------ prints the given room --------------------*/
void CRoom::sendRoom()
{
    unsigned int i, pos;
    char line[MAX_STR_LEN];
    
    send_to_user(" Id: %i, Flags: %s, Region: %s, Coord: %i,%i,%i\r\n", id,
	    (const char *) conf.sectors[sector].desc, 
	    (const char *) region->getName(),
	    node->getPosition().x, node->getPosition().y, node->getPosition().z);
    send_to_user(" [32m%s[0m\n", (const char *) name);

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
    send_to_user(" note: %s\n", (const char *) note);

    
    sprintf(line, "Doors:");
    for (i = 0; i <= 5; i++) {
      if (doors[i].isEmpty() != true) {
        sprintf(line + strlen(line), " %c: %s", dirbynum(i), (const char *) doors[i]);
      }
    
    }
    send_to_user("%s\r\n", line);

    
    if (conf.get_brief_mode() && proxy.isMudEmulation()) {
      sprintf(line, "Exits: ");
      for (i = 0; i <= 5; i++)
          if (isExitPresent(i) == true) {
              if ( isExitUndefined(i) ) {
                  sprintf(line + strlen(line), " #%s#", exitnames[i]);
                  continue;
              }
              if ( isExitDeath(i) ) {
                  sprintf(line + strlen(line), " !%s!", exitnames[i]);
                  continue;
              }
              if (doors[i].isEmpty() != true) {
                  if (doors[i] == "exit")  {
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
          if (isExitPresent(i) == true) {
              if (isExitUndefined(i) ) {
                  sprintf(line + strlen(line), " #%s#", exitnames[i]);
                  continue;
              }
              if (isExitDeath(i)) {
                  sprintf(line + strlen(line), " !%s!", exitnames[i]);
                  continue;
              }
              if (doors[i].isEmpty() == false) {
                  if (strcmp("exit", doors[i]) == 0) {
                      sprintf(line + strlen(line), " (%s)", exitnames[i]);
                  } else {
                      sprintf(line + strlen(line), " +%s+", exitnames[i]);
                  }
              } else {
                  sprintf(line + strlen(line), " %s", exitnames[i]);
              }
              sprintf(line + strlen(line), " -[to %i]-", exits[i]->id );
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


