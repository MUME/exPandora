//
// C++ Implementation: Regions
//
// Description: 
//
//
// Author:  <aza@alpha>, (C) 2006
//
// Copyright: See COPYING file that comes with this distribution
//
//
#include "Regions.h"
#include "utils.h"


CRegion::CRegion()
{
    name.clear();
    doors.clear();
    

}

CRegion::~CRegion()
{
    name.clear();
    doors.clear();
}

    
void CRegion::setName(QByteArray newname)
{
    name = newname;
}

QByteArray CRegion::getName()
{
    return name;
}

void CRegion::addDoor(QByteArray alias, QByteArray name)
{
    doors.insert(alias.trimmed(), name.simplified());
}

QByteArray CRegion::getDoor(QByteArray alias)
{
    if (doors.contains(alias) == true) 
        return doors[alias];        
    return "";
}

bool CRegion::removeDoor(QByteArray alias)
{
    return doors.remove(alias);
}

void CRegion::showRegion()
{
    send_to_user("-- Region: %s\r\n", (const char *) name);
    
    QMapIterator<QByteArray, QByteArray> i(doors);
    while (i.hasNext()) {
        i.next();
        send_to_user("Alias: %-15s Door: %-40s\r\n", (const char *)  i.key(), (const char *) i.value() );
    }
    send_to_user("\r\n");
}

QMap<QByteArray, QByteArray> CRegion::getAllDoors()
{
    return doors;
}

QByteArray CRegion::getAliasByDoor(QByteArray door, int dir)
{
    QByteArray fulldoor;
    
    fulldoor = door + " ";
    fulldoor.append( dirbynum(dir) );

    QMapIterator<QByteArray, QByteArray> i(doors);
    while (i.hasNext()) {
        i.next();
        printf("Checking ...%s...\r\n", (const char *) i.value() );
        if (i.value() == door || i.value() == fulldoor)
            return i.key();
    }
    return "";    
}



