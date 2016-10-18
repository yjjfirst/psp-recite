#ifndef _RECITE_TIMER_H
#define _RECITE_TIMER_H

typedef int timer_id;

struct recite_timer {
	unsigned int ms;
	void (*handler)(void *data);
	timer_id id;
};


extern timer_id register_timer(struct recite_timer *timer);
extern int remove_timer(timer_id a_id);
extern int init_timer(struct recite_timer *timer, int ms, void (*handle)(void *data));
extern void check_timeout(void);

#endif

