#ifndef REGIONS_H
#define REGIONS_H

#include <QMap>
#include <QByteArray>

class CRegion {
    QByteArray                      name;
    QMap<QByteArray, QByteArray> doors;
public:
    CRegion();
    ~CRegion();
    
    void setName(QByteArray newname);
    QByteArray getName();
    
    void addDoor(QByteArray alias, QByteArray name);
    QByteArray getDoor(QByteArray alias);
    bool removeDoor(QByteArray alias);
    
    QMap<QByteArray, QByteArray> getAllDoors();
    
    QByteArray getAliasByDoor(QByteArray door, int dir);
    
    void showRegion();
};


#endif
