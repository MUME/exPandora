#include "defines.h"
#include "event.h"

std::vector<TEvent> Events;

CEventPipe::CEventPipe()
{
    pipe.clear();
}

CEventPipe::~CEventPipe()
{
    pipe.clear();
}

void CEventPipe::push(TEvent event)
{
    pipe.push_back(event);    
/*    printf("Pushing the event: %s %s, queue length is now %i.\r\n", 
                (const char *) Events[event.type].data ,(const char *) event.data, pipe.size() );
*/
}

bool CEventPipe::empty()
{
    return pipe.empty();
}

TEvent CEventPipe::pop()
{
    TEvent e;
    
    e = get();
    pipe.erase(pipe.begin());
    return e;
}


TEvent CEventPipe::get()
{
    TEvent e;
    
    if (pipe.empty()) {
       e.type = 0;
       e.data = "";
       return e;
    }
    e = pipe[0];
    return e;
}

QByteArray CEventPipe::print()
{
    QByteArray s;
    unsigned int i;

    pipe_lock.lock();    

    
    s = " Elements: ";
    if (pipe.empty()) {
       s += "(null)";
       pipe_lock.unlock();    
       return s;               
    }
    for (i = 0; i < pipe.size(); i++) {
        s += "[type ";
        s += Events[pipe[i].type].data;
	    s += ", data ";
	    s += pipe[i].data;
	    s += "] ";
        if (s.length() > MAX_STR_LEN/2) {
            if (i < pipe.size() - 1)
                s += "...";
            pipe_lock.unlock();    
            return s;
        }
    }
    pipe_lock.unlock();    
    
    return s;
}

char get_event_type(QByteArray name)
{
    unsigned int i;
       
    for (i = 0; i < Events.size(); i++)
        if (name == Events[i].data)
           return Events[i].type;
    return -1; 
}

int get_event_group(QByteArray name)
{
    unsigned int i;
       
    for (i = 0; i < Events.size(); i++)
        if (name == Events[i].data)
           return Events[i].group;
    return -1; 
}

int get_event_group(char type)
{
    return Events[type].group;       
}
