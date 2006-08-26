#ifndef QTOGREWIDGET_H
#define QTOGREWIDGET_H


#include <OgreNoMemoryMacros.h>
#include <QtGui>
#include <OgreMemoryMacros.h>
//#include <vector>

#include <Ogre.h>

/** A QWidget specialised for embedding an Ogre window. */
class QOgreWidget : public QWidget
{
	Q_OBJECT

///--QWidget Methods------------------------------------------------------------
public:

    QOgreWidget (QWidget *parent = 0, Qt::WFlags f = 0);
    virtual ~QOgreWidget ();

    void update();

    QSize minimumSizeHint () const;
    QSize sizeHint () const;

protected:
    void showEvent (QShowEvent *event);
    void timerEvent (QTimerEvent *);
    void paintEvent (QPaintEvent *);


///--Ogre Methods-------------------------------------------------------------
    virtual void setupResources (void) = 0;
    virtual void configure (void);

///--Member Variables-----------------------------------------------------------
    static Ogre::Root *theOgreRoot;
//    static std::vector <Ogre::RenderWindow *> theRenderWindowList;
    Ogre::RenderWindow *myRenderWindow;
};


#endif // QTOGREWIDGET_H
