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
}

bool CEventPipe::empty()
{
    return pipe.empty();
}

Event CEventPipe::pop()
{
    Event e;
    
    e = get();
    pipe.erase(pipe.begin());
    return e;
}


Event CEventPipe::get()
{
    return pipe[0];
}

