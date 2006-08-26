#ifndef __SCENEMANAGER_H__
#define __SCENEMANAGER_H__

#include "ExampleApplication.h"

class MySceneManager : public ExampleApplication
{
    Q_OBJECT
    
   bool          LeftButtonPressed;
   bool          RightButtonPressed;
   QPoint        old_pos;


public:
    MySceneManager(QWidget *parent=0, Qt::WFlags f=0);
    ~MySceneManager();
    
    Ogre::Vector3   user;
    float                  anglex;
    float                  angley;
    float                  anglez;

    Ogre::SceneNode* addRoom(CRoom *room);
    void resetCamera();
    void removeRoom(CRoom *room);
    void centerOnRoom(CRoom *room);
protected:
//    Ogre::SceneNode* mFountainNode;

    
    // Just override the mandatory create scene method
    void createScene(void);

    // Create new frame listener
    void createFrameListener(void);
    
public slots:
   void keyPressEvent( QKeyEvent * );
    
protected slots:
   void mousePressEvent( QMouseEvent *e);
   void mouseReleaseEvent( QMouseEvent *e);
   void mouseMoveEvent( QMouseEvent *e);
   void wheelEvent(QWheelEvent *e);
};

extern class MySceneManager *renderer;


#endif