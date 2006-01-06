#ifndef COMPONENT
#define COMPONENT

#include <qobject.h>
#include <qthread.h>
#include <QVariant>
#include <iostream>
#include <map>

#ifdef Q_WS_WIN
# define MY_EXPORT __declspec(dllexport)
#else
# define MY_EXPORT
#endif


/**
 * every component that should be available from a library should inherit Component
 * and implement a componentCreator which is available via "extern "C" MY_EXPORT ..."
 */
class ComponentThreader;

class Component : public QObject {
 private:
  Q_OBJECT
	
protected:
	ComponentThreader * thread;
	std::map<QString, QVariant> options;
	  
 public:
  /* set the component to a running state 
   * after passing any arguments via properties
   * will call thread->start if the component encapsulates a thread 
   * can be overloaded to carry one-time functionality like in Configuration
   */
  virtual void start();
  virtual ~Component();
  virtual Qt::ConnectionType requiredConnectionType(const char *) {return Qt::AutoCompatConnection;}
  Component::Component(bool threaded = false);
  void setOption(QString & key, QVariant & value) {options[key] = value;}
  
};

class ComponentThreader : public QThread {
	private:
		Q_OBJECT;
		Component * component;

	public:
		ComponentThreader(Component * comp) : component(comp) {}
		void run() {exec();}				
};

typedef Component * (*componentCreator)();

#endif
