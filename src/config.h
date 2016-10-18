#ifndef _CONFIG_H_
#define _CONFIG_H_

struct app_config {
	int wordlib;
	int wordlist;
	int activeword;
};

extern struct app_config config;

extern int read_config();
extern int write_config();

#endif
