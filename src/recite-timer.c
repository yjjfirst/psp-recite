#include "recite-timer.h"
#include "common/datatype.h"
#include "common/log.h"

#define MAX_RECITE_TIMER 10
struct recite_timer recite_timers[MAX_RECITE_TIMER];
/**
 * 
 */
timer_id register_timer(struct recite_timer *timer)
{
	int i;

	for (i = 0; i < MAX_RECITE_TIMER; i ++) {
		if (recite_timers[i].id != 0) continue;   
		recite_timers[i].ms = timer->ms / 2;
		recite_timers[i].handler = timer->handler;
		recite_timers[i].id = i + 1;
		timer->id = i + 1;
		return i;
	}

	return -1;
}

int remove_timer(timer_id a_id)
{
	recite_timers[a_id - 1].id = 0; 
	recite_timers[a_id - 1].ms = 0;
	recite_timers[a_id - 1].handler = NULL;
	return 0;
}

int init_timer(struct recite_timer *timer, int ms, void (*handle)(void *data))
{
	timer->ms = ms;
	timer->handler = handle;

	return 0;
}

void check_timeout(void) 
{
	int i;

	for (i = 0; i < MAX_RECITE_TIMER; i ++) {		
		if (recite_timers[i].id == 0) continue;

		recite_timers[i].ms -= 50;
		if (recite_timers[i].ms > 0) continue; 

		recite_timers[i].id = 0;
		recite_timers[i].handler(&i);  

	}
}
