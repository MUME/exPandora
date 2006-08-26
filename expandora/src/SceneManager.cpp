
#include "SceneManager.h"

#include <QKeyEvent>

#include "utils.h"
#include "Map.h"


using namespace Ogre;


class MySceneManager *renderer;


    MySceneManager::MySceneManager (QWidget *parent, Qt::WFlags f) :
        ExampleApplication (parent, 0)
    { 
    }

    MySceneManager ::~MySceneManager() 
    {
      
    }
    
    // Just override the mandatory create scene method
    void MySceneManager::createScene(void)
    {
        // Set ambient light
        mSceneMgr->setAmbientLight(ColourValue(0.2, 0.2, 0.2));

        theOgreRoot->setFrameSmoothingPeriod(5.0f);

        // Create a skydome
        //mSceneMgr->setSkyDome(true, "Examples/CloudySky", 5, 8);

        // Create a light
        Light* l = mSceneMgr->createLight("MainLight");
        // Accept default settings: point light, white diffuse, just set position
        // NB I could attach the light to a SceneNode if I wanted it to move automatically with
        //  other objects, but I don't
        l->setPosition(20,80,50);

        Entity *ent;

        // Define a floor plane mesh
//        Plane p;
//        p.normal = Vector3::UNIT_Y;
//        p.d = 200;
//        MeshManager::getSingleton().createPlane(
//            "FloorPlane", ResourceGroupManager::DEFAULT_RESOURCE_GROUP_NAME, 
//            p, 200000, 200000, 20, 20, true, 1, 50, 50, Vector3::UNIT_Z);

        // Create an entity (the floor)
//        ent = mSceneMgr->createEntity("floor", "FloorPlane");
//        ent->setMaterialName("Examples/RustySteel");
        // Attach to child of root node, better for culling (otherwise bounds are the combination of the 2)
//        mSceneMgr->getRootSceneNode()->createChildSceneNode()->attachObject(ent);



        // Add a head, give it it's own node
        //SceneNode* roomNode = mSceneMgr->getRootSceneNode()->createChildSceneNode();
        //ent = mSceneMgr->createEntity("room", "ogrehead.mesh");
        //roomNode->attachObject(ent);

        // Make sure the camera track this node
//        mCamera->setAutoTracking(true, headNode);

        // Create the camera node & attach camera
        SceneNode* camNode = mSceneMgr->getRootSceneNode()->createChildSceneNode();
        camNode->attachObject(mCamera);


        // Put in a bit of fog for the hell of it
        mSceneMgr->setFog(FOG_EXP, ColourValue::White, 0.0002);
    }


// ------------------------------------------------------------------------------------
    SceneNode *MySceneManager::addRoom(CRoom *room)
    {
        Entity *ent;
        
        SceneNode* roomNode = mSceneMgr->getRootSceneNode()->createChildSceneNode();
        ent = mSceneMgr->createEntity(StringConverter::toString("Room ") + StringConverter::toString(room->id), "room.mesh");
        roomNode->attachObject(ent);
        return roomNode;
    }
    
    void MySceneManager::removeRoom(CRoom *room)
    {
        Entity *ent;
                
        ent = mSceneMgr->getEntity( StringConverter::toString("Room ") + StringConverter::toString(room->id) ); 
        SceneNode* parent = ent->getParentSceneNode();
        parent->detachObject(ent);
        mSceneMgr->destroyEntity(ent);
        // entity is now destroyed, don't try to use the pointer anymore!
        
        // optionally destroy node
        mSceneMgr->destroySceneNode(parent->getName());
    }


// ------------------------------------------------------------------------------------
    // Create new frame listener
    void MySceneManager::createFrameListener(void)
    {
        mFrameListener= new ExampleFrameListener(myRenderWindow, mCamera);
        theOgreRoot->addFrameListener(mFrameListener);

    }


void MySceneManager::centerOnRoom(CRoom *room)
{
    Vector3 mTranslateVector;
    Radian  x, y, z;

    // Make all the changes to the camera
    // Note that YAW direction is around a fixed axis (freelook style) rather than a natural YAW (e.g. airplane)
    if (room == NULL) {
        room = Map.getRoom( 1 );
            if (room == NULL)
                return;
    }
    
    x = anglex * (3.14 / 180);
    y = angley * (3.14 / 180);
    z = anglez * (3.14 / 180);
    
    // get position of the room we are looking at
    mTranslateVector = room->getRoomNode()->getPosition();
    // add user defined shift 
    mTranslateVector -= user;
    
    
    // reset angles
    anglex = 0;
    angley = 0;
    anglez = 0;
    
//    printf("Old coordinates : anglex %f, angley %f, anglez | X %i, Y %i, Z %i\r\n");
    
    mCamera->yaw(-y);
    mCamera->pitch(x);
    mCamera->roll(z);
    mCamera->setPosition(mTranslateVector);
}

void MySceneManager::resetCamera()
{
    user.x = 0;
    user.y = 0;
    user.z = 0;    
    
    // move the camera to zero position, set up the angle 
    mCamera->setPosition(Vector3(0,0,80));
    mCamera->lookAt(Vector3(0,0,-300));
    
    // and translate it to the room we are looking at 
    centerOnRoom( NULL );
}

void MySceneManager::keyPressEvent( QKeyEvent * k) 
{
    switch ( k->key() ) {
        case Qt::Key_X :
            user.z += 1;
            break;
    
         case Qt::Key_Y:
            user.z -= 1;
            break;
    
         case Qt::Key_Q:
            user.x -= 1;
            break;
         
         case Qt::Key_W:
            user.x += 1;
            break;
         
         case Qt::Key_A:
            user.y += 1;
            break;
    
         case Qt::Key_S:
            user.y -= 1;
            break;
        case Qt::Key_Up:
          anglex += 1;
          break;
        case Qt::Key_Down:
          anglex -= 1;
          break;
        case Qt::Key_Left:
          angley -= 1;
          break;
        case Qt::Key_Right:
          angley += 1;
          break;
        case Qt::Key_PageUp:
          anglez += 1;
          break;
        case Qt::Key_PageDown:
          anglez -= 1;
          break;
        
         case Qt::Key_Escape:           
            resetCamera();
            break;				
    }
    centerOnRoom( NULL );    
    update();
}

void MySceneManager::mousePressEvent( QMouseEvent *e)
{
  
  if (e->button() == Qt::LeftButton) {
    LeftButtonPressed = true;
  } else {
    RightButtonPressed = true;
  }
  old_pos = e->pos();
}

void MySceneManager::mouseReleaseEvent( QMouseEvent *e)
{
  if (e->button() == Qt::LeftButton) {
    LeftButtonPressed = false;
  } else {
    RightButtonPressed = false;
  }
}

void MySceneManager::mouseMoveEvent( QMouseEvent *e)
{
  QPoint pos;
  int dist_x, dist_y;

  pos = e->pos();
  dist_x = pos.x() - old_pos.x();
  dist_y = pos.y() - old_pos.y();
/*  
  print_debug(DEBUG_INTERFACE, "mouseEvent. LeftMouse %s, RightMouse %s. Dist_x %i, dist_y %i.",
      ON_OFF(LeftButtonPressed), ON_OFF(RightButtonPressed), dist_x, dist_y);
*/  
  if (LeftButtonPressed) {
      user.x += (float) dist_x / 2.0;
      user.y -= (float) dist_y / 2.0;
    
      old_pos = pos;
  } else if (RightButtonPressed) {
    anglex += dist_y / 1;
    angley += dist_x / 1;
    
    old_pos = pos;
  }
  centerOnRoom( NULL );    
  update();
}

void MySceneManager::wheelEvent(QWheelEvent *e)
{
  int delta;

  delta = e->delta();

  user.z += delta / 60;
  centerOnRoom( NULL );    
  update();
}
