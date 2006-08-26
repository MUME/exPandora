#include "ExampleApplication.h"
#include <OgreEventListeners.h>
#include <OgreStringConverter.h>
#include <OgreException.h>

#include "utils.h"

using namespace Ogre;



ExampleApplication::ExampleApplication  (QWidget *parent, Qt::WFlags f) :
     QOgreWidget (parent, f)
{
        isInit = false;
}
    
    
    ExampleApplication::~ExampleApplication()
    {
        if (mFrameListener)
            delete mFrameListener;
    }

void ExampleApplication::show(void)
    {
        if (isInit) return;
        isInit = true;

                


        setupResources();
        chooseSceneManager();
        createCamera();
        createViewports();

        // Set default mipmap level (NB some APIs ignore this)
        TextureManager::getSingleton().setDefaultNumMipmaps(5);

        // Create any resource listeners (for loading screens)
        createResourceListener();
        // Load resources
        loadResources();

        // Create the scene
        createScene();

        createFrameListener();
        std::cout << __FILE__ << ":" << __LINE__ << std::endl;
    }

void ExampleApplication:: resizeEvent (QResizeEvent *event) {
        printf("In resizeEvent.\r\n");
        
        if (myRenderWindow) {
            myRenderWindow->resize (width(), height());
            myRenderWindow->windowMovedOrResized();
        } 
        if (mCamera)
            mCamera->setAspectRatio(Ogre::Real(m_vp->getActualWidth()) / Ogre::Real(m_vp->getActualHeight()));
        
        update();    
    }


    // These internal methods package up the stages in the startup process
    void ExampleApplication::chooseSceneManager(void)
    { // Create the SceneManager, in this case a generic one
        mSceneMgr = theOgreRoot->createSceneManager(ST_GENERIC, "ExampleSMInstance" + Ogre::StringConverter::toString ((unsigned long)this));
    }
    
    void ExampleApplication::createCamera(void)
    {
        // Create the camera
        mCamera = mSceneMgr->createCamera("PlayerCam" + Ogre::StringConverter::toString ((unsigned long)this));

        // Position it at 500 in Z direction
        mCamera->setPosition(Vector3(0,0,80));
        // Look back along -Z
        mCamera->lookAt(Vector3(0,0,-300));
        mCamera->setNearClipDistance(5);
        mCamera->setFarClipDistance(1000);
    }
    
    void ExampleApplication::createFrameListener(void)
    {
        mFrameListener= new ExampleFrameListener(myRenderWindow, mCamera);
        mFrameListener->showDebugOverlay(true);
        theOgreRoot->addFrameListener(mFrameListener);
    }


    void ExampleApplication::createViewports(void)
    {
        // Create one viewport, entire window
        m_vp = myRenderWindow->addViewport(mCamera);
        m_vp->setBackgroundColour(ColourValue(0,0,0));

        // Alter the camera aspect ratio to match the viewport
        mCamera->setAspectRatio(
            Real(m_vp->getActualWidth()) / Real(m_vp->getActualHeight()));
    }

    /// Method which will define the source of resources (other than current folder)
    void ExampleApplication::setupResources(void)
    {
        std::cout << __FILE__ << ":" << __LINE__ << std::endl;
        // Load resource paths from config file
        ConfigFile cf;
        cf.load("resources.cfg");

        // Go through all sections & settings in the file
        ConfigFile::SectionIterator seci = cf.getSectionIterator();

        String secName, typeName, archName;
        while (seci.hasMoreElements())
        {
            secName = seci.peekNextKey();
            ConfigFile::SettingsMultiMap *settings = seci.getNext();
            ConfigFile::SettingsMultiMap::iterator i;
            for (i = settings->begin(); i != settings->end(); ++i)
            {
                typeName = i->first;
                archName = i->second;
                ResourceGroupManager::getSingleton().addResourceLocation(
                    archName, typeName, secName);
            }
        }
    }

	/// Optional override method where you can create resource listeners (e.g. for loading screens)
	void ExampleApplication::createResourceListener(void)
	{

	}

	/// Optional override method where you can perform resource group loading
	/// Must at least do ResourceGroupManager::getSingleton().initialiseAllResourceGroups();
	void ExampleApplication::loadResources(void)
	{
		// Initialise, parse scripts etc
		ResourceGroupManager::getSingleton().initialiseAllResourceGroups();

	}


//#######################################################


//#-----------------------------------------------
//#
//#
//
//
//
//
//
//
//
//
//
//
//
//


    void ExampleFrameListener::updateStats(void)
    {
        static String currFps = "Current FPS: ";
        static String avgFps = "Average FPS: ";
        static String bestFps = "Best FPS: ";
        static String worstFps = "Worst FPS: ";
        static String tris = "Triangle Count: ";

        // update stats when necessary
        try {
            OverlayElement* guiAvg = OverlayManager::getSingleton().getOverlayElement("Core/AverageFps");
            OverlayElement* guiCurr = OverlayManager::getSingleton().getOverlayElement("Core/CurrFps");
            OverlayElement* guiBest = OverlayManager::getSingleton().getOverlayElement("Core/BestFps");
            OverlayElement* guiWorst = OverlayManager::getSingleton().getOverlayElement("Core/WorstFps");

            const RenderTarget::FrameStats& stats = mWindow->getStatistics();

            guiAvg->setCaption(avgFps + StringConverter::toString(stats.avgFPS));
            guiCurr->setCaption(currFps + StringConverter::toString(stats.lastFPS));
            guiBest->setCaption(bestFps + StringConverter::toString(stats.bestFPS)
                +" "+StringConverter::toString(stats.bestFrameTime)+" ms");
            guiWorst->setCaption(worstFps + StringConverter::toString(stats.worstFPS)
                +" "+StringConverter::toString(stats.worstFrameTime)+" ms");

            OverlayElement* guiTris = OverlayManager::getSingleton().getOverlayElement("Core/NumTris");
            guiTris->setCaption(tris + StringConverter::toString(stats.triangleCount));

            OverlayElement* guiDbg = OverlayManager::getSingleton().getOverlayElement("Core/DebugText");
            guiDbg->setCaption(mWindow->getDebugText());
        }
        catch(...)
        {
            // ignore
        }
    }

    ExampleFrameListener::ExampleFrameListener(RenderWindow* win, Camera* cam)
    {
        mDebugOverlay = OverlayManager::getSingleton().getByName("Core/DebugOverlay");

        mCamera = cam;
        mWindow = win;
        mStatsOn = true;
	mTranslateVector = Vector3::ZERO;

        showDebugOverlay(true);
    }
    
    ExampleFrameListener::~ExampleFrameListener()
    {
    }


    void ExampleFrameListener::moveCamera()
    {

            // Make all the changes to the camera
            // Note that YAW direction is around a fixed axis (freelook style) rather than a natural YAW (e.g. airplane)
            mCamera->yaw(mRotX);
            mCamera->pitch(mRotY);
            mCamera->moveRelative(mTranslateVector);
    }

    void ExampleFrameListener::showDebugOverlay(bool show)
    {
        if (mDebugOverlay)
        {
            if (show)
            {
                mDebugOverlay->show();
            }
            else
            {
                mDebugOverlay->hide();
            }
        }
    }

    // Override frameStarted event to process that (don't care about frameEnded)
    bool ExampleFrameListener::frameStarted(const FrameEvent& evt)
    {
        if(mWindow->isClosed())
            return false;

	moveCamera();
	return true;
    }

    bool ExampleFrameListener::frameEnded(const FrameEvent& evt)
    {
        updateStats();
        return true;
    }
