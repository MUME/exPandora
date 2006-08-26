#include "QOgreWidget.h"

#include <OgreNoMemoryMacros.h>
#if defined(Q_WS_WIN)
#include <windows.h>	// needed for WindowFromDC()
#else
#include <Qt/qx11info_x11.h>
#include <X11/Xlib.h>
#endif
#include <OgreMemoryMacros.h>

#include "utils.h"

Ogre::Root *QOgreWidget::theOgreRoot = 0;
//std::vector <Ogre::RenderWindow *> QOgreWidget::theRenderWindowList = std::vector <Ogre::RenderWindow*>();

QOgreWidget::QOgreWidget (QWidget* parent, Qt::WFlags f) :
    QWidget (parent, f),
    myRenderWindow (0)
{
    std::cout  << __FILE__ << ":" << __LINE__ << std::endl;
    showEvent (0);
}

QOgreWidget::~QOgreWidget()
{
    if (theOgreRoot) delete theOgreRoot;
}
void QOgreWidget::showEvent (QShowEvent *event)
{
    startTimer(60);
    if (myRenderWindow) return;

    Ogre::NameValuePairList params;

    QWidget *q_parent = dynamic_cast <QWidget *> (parent());
    assert (q_parent);

#if !defined(Q_WS_WIN)
    QX11Info xInfo = x11Info();

    /// TODO: Is this needed anymore?!
    char buf[64];
    sprintf(buf, "GLX_WINDOW=0x%lx", winId());
    putenv(buf);

    //params["externalWindowHandle"] =
    params["parentWindowHandle"] =
         Ogre::StringConverter::toString ((unsigned long)xInfo.display()) +
        ":" + Ogre::StringConverter::toString ((unsigned int)xInfo.screen()) +
        ":" + Ogre::StringConverter::toString ((unsigned long)q_parent->winId());
#else
    params["parentWindowHandle"] = Ogre::StringConverter::toString((unsigned long)q_parent->winId());
#endif
    if (!theOgreRoot) {
        theOgreRoot = new Ogre::Root();
        /// TODO: Does this really belong in the base widget?
        ///  and if it does, should I also faithfully include 
        ///  methods for setting up the scene, the camera, the viewport?
        std::cout  << __FILE__ << ":" << __LINE__ << std::endl;
        //setupResources();

        configure();
        theOgreRoot->initialise (false);
    }


    myRenderWindow = theOgreRoot->createRenderWindow (
                "View" + Ogre::StringConverter::toString((unsigned long)this), width(), height(), false, &params);
//    theRenderWindowList.push_back (myRenderWindow);

    WId window_id;
#if !defined(Q_WS_WIN)
    myRenderWindow->getCustomAttribute ("GLXWINDOW", &window_id);
#else
    myRenderWindow->getCustomAttribute ("HWND", &window_id);
#endif
    // Take over the ogre created window.
    QWidget::create (window_id);

    myRenderWindow->reposition (x(),y());

}


void QOgreWidget::timerEvent( QTimerEvent * )
{
//    std::cout << this << " timerEvent" << std::endl;
    update();
}

void QOgreWidget::update() {
//    printf("In QOgreWidget update.\r\n");
     QTime t;
     t.start();
        


    Ogre::Root::getSingletonPtr()->_fireFrameStarted();
/*    for (unsigned int i=0; i < theRenderWindowList.size(); ++i) {
        theRenderWindowList[i]->update();
    }
*/
    myRenderWindow->update();
    Ogre::Root::getSingletonPtr()->_fireFrameEnded();
    
    print_debug(DEBUG_RENDERER, "Rendering's done. Time elapsed %d ms", t.elapsed());

}



void QOgreWidget::paintEvent (QPaintEvent *e)
{
    update();
}


void QOgreWidget::configure (void) {
		if (!theOgreRoot->restoreConfig()) {
			theOgreRoot->showConfigDialog ();
		}
}
QSize QOgreWidget::minimumSizeHint() const
{
	return QSize(50, 50);
}

QSize QOgreWidget::sizeHint() const
{
	return QSize(800, 600);
}

/* vim:et:sts=4:ts=4:sw=4
*/
