#include <string.h>

#include "defines.h"

#include "renderer.h"
#include "dispatch.h"
#include "forwarder.h"
#include "utils.h"
#include "stacks.h"

class stackmanager stacker;


void stackmanager::get_current(char *str)
{
  if (amount == 0) {
    sprintf(str, "NO_SYNC");
    return;
  }
  
  if (amount > 1) {
    sprintf(str, " MULT ");
    return;
  }
  
  sprintf(str, "%i", sa[1]);
  
}


void stackmanager::printstacks()
{
    unsigned int i;

    send_to_user(" Possible positions : \n");
    if (amount == 0)
	send_to_user(" Current position is unknown!\n");
    for (i = 1; i <= amount; i++) {
	send_to_user(" %i\n", sa[i]);
    }
}

void stackmanager::put(unsigned int id)
{

    if (mark[id] == 1)
	return;
    sb[++next] = id;
    mark[id] = 1;
}

unsigned int stackmanager::get(unsigned int i)
{
    return sa[i];
}

unsigned int stackmanager::get_next(unsigned int i)
{
    return sb[i];
}


stackmanager::stackmanager()
{
    reset();
}

void stackmanager::reset()
{
    amount = 0;
    next = 0;
    stacka[0] = 0;
    stackb[0] = 0;
    sa = stacka;
    sb = stackb;
    memset(mark, 0, MAX_ROOMS);
    swap();
}

void stackmanager::swap()
{
    unsigned int *t;

    memset(mark, 0, MAX_ROOMS);
    t = sa;
    sa = sb;
    sb = t;
    amount = next;
    next = 0;
  
    if (renderer_window)
      renderer_window->update_status_bar();
}
