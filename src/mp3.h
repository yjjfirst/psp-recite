#ifndef _MP3_H_
#define _MP3_H_

#include "common/datatype.h"

extern bool mp3_init();
extern void mp3_end();
extern void mp3_play(const char *file, const char *zipfile);

#endif
