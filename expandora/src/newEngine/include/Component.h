#ifndef COMPONENT
#define COMPONENT

#include <qobject.h>

class Component : public QObject {
 private:
  Q_OBJECT
  
 public:
  /* set the component to a running state 
   * after passing any arguments via properties
   * will call QThread::run if the component encapsulates a thread 
   */
  void start(); 

};
#endif
