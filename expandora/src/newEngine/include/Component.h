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



class Component;
class ComponentThreader : public QThread
{
private:
  Q_OBJECT;
  Component * owner;

public:
  ComponentThreader(Component * c) : owner(c) {}
  void run();
};

class Component : public QObject
{
  friend class ComponentThreader;
private:
  Q_OBJECT
  void runInit() {init();}

protected:
  ComponentThreader * thread;
  std::map<QString, QVariant> options;
  virtual void init() {}

public:
  /* set the component to a running state
   * after passing any arguments via options the configuration
   * will call start. 
   * INIT can be overloaded to carry one-time functionality
   * init will be called by start and will run in the correct thread 
   * while start is used to figure out
   * which thread that is.
   */
  void start();

  virtual ~Component();
  virtual Qt::ConnectionType requiredConnectionType(const QString &) {return Qt::AutoCompatConnection;}
  Component::Component(bool threaded = false);
  void setOption(const QString & key, const QVariant & value) {options[key] = value;}


};

/**
 * every component that should be available from a library should inherit Component
 * and implement a componentCreator which is available via "extern "C" MY_EXPORT ..."
 */
typedef Component * (*componentCreator)();

class ComponentCreator
{
public:
  virtual Component * create() = 0;
  virtual ~ComponentCreator() {}
  static std::map<QString, ComponentCreator *> & creators();
};

template <class T>
class Initializer : public ComponentCreator
{

public:
  Initializer(QString name)
  {
    creators()[name] = this;
  }
  T * create()
  {
    return new T;
  }
};



#ifdef DMALLOC
#include <mpatrol.h>
#endif

#endif
