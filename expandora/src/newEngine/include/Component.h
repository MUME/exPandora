#ifndef COMPONENT
#define COMPONENT

#include <qobject.h>
#include <qthread.h>

#ifdef Q_WS_WIN
# define MY_EXPORT __declspec(dllexport)
#else
# define MY_EXPORT
#endif

/**
 * every component that should be available from a library should inherit Component
 * and implement a componentCreator which is available via "extern "C" MY_EXPORT ..."
 */
class Component : public QObject {
 private:
  Q_OBJECT
  
 public:
  /* set the component to a running state 
   * after passing any arguments via properties
   * will call QThread::start if the component encapsulates a thread 
   */
  virtual void start(QThread::Priority priority = QThread::InheritPriority) = 0; 

};
#endif
