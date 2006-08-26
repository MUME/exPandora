
#ifndef __ExampleApplication_H__
#define __ExampleApplication_H__

#include "QOgreWidget.h"

//#include "ExampleFrameListener.h"

#include <Ogre.h>
#include <OgreConfigFile.h>
///-----------------------------------------------------------------------------
/// C++ includes
//#include <iostream>
//#include <string>
//#include <map>
//#include <cstdlib>

#include <OgreNoMemoryMacros.h>

///-----------------------------------------------------------------------------
/// Some windows includes for when runnning on windows.
#ifdef G_OS_WIN32
#define WIN32_LEAN_AND_MEAN 1
#include <windows.h>
#endif
#include <OgreMemoryMacros.h>


#include "CRoom.h"

class ExampleFrameListener: public Ogre::FrameListener {
protected:
    int mSceneDetailIndex ;
    Ogre::Real mMoveSpeed;
    Ogre::Degree mRotateSpeed;
    Ogre::Overlay* mDebugOverlay;

    void updateStats(void);

public:
    // Constructor takes a RenderWindow because it uses that to determine input context
    ExampleFrameListener(Ogre::RenderWindow* win, Ogre::Camera* cam);
    virtual ~ExampleFrameListener();

    void showDebugOverlay(bool show);
    
    // Override frameStarted event to process that (don't care about frameEnded)
    bool frameStarted(const Ogre::FrameEvent& evt);
    bool frameEnded(const Ogre::FrameEvent& evt);
    void moveCamera();
	
protected:
    Ogre::Camera* mCamera;

    Ogre::Vector3 mTranslateVector;
    Ogre::RenderWindow* mWindow;
    Ogre::Radian mRotX, mRotY;

    bool mStatsOn;
//    unsigned int mNumScreenShots;
};



class ExampleApplication : public QOgreWidget
{
public:
    /// Standard constructor
    ExampleApplication (QWidget *parent = 0, Qt::WFlags f = 0);
    /// Standard destructor
    virtual ~ExampleApplication();
    /// Override the show  method to initialize our stuff.
    /// we should only do this once.
    virtual void show(void);
    
protected:
    Ogre::Camera* mCamera;
    Ogre::Viewport *m_vp;
    Ogre::SceneManager* mSceneMgr;
    ExampleFrameListener* mFrameListener;
    bool isInit;

    void resizeEvent (QResizeEvent *event);
    // These internal methods package up the stages in the startup process
    virtual void chooseSceneManager(void);
    virtual void createCamera(void);
    
    virtual void createFrameListener(void);


    virtual void createScene(void) = 0;    // pure virtual - this has to be overridden

    virtual void destroyScene(void){}    // Optional to override this

    virtual void createViewports(void);

    /// Method which will define the source of resources (other than current folder)
    virtual void setupResources(void);

	/// Optional override method where you can create resource listeners (e.g. for loading screens)
    virtual void createResourceListener(void);

	/// Optional override method where you can perform resource group loading
	/// Must at least do ResourceGroupManager::getSingleton().initialiseAllResourceGroups();
    virtual void loadResources(void);



};


#endif

