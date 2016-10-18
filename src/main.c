/*
 * PSP Software Development Kit - http://www.pspdev.org
 * -----------------------------------------------------------------------
 * Licensed under the BSD license, see LICENSE in PSPSDK root for details.
 *
 * main.c - Basic Input demo -- reads from control pad and indicates button
 *          presses.
 *
 * Copyright (c) 2005 Marcus R. Brown <mrbrown@ocgnet.org>
 * Copyright (c) 2005 James Forshaw <tyranid@gmail.com>
 * Copyright (c) 2005 John Kelley <ps2dev@kelley.ca>
 * Copyright (c) 2005 Donour Sizemore <donour@uchicago.edu>
 *
 * $Id: main.c,v 1.1.1.1 2007/12/24 07:41:16 yjjfirst Exp $
 */
#include <pspkernel.h>
#include <pspdebug.h>
#include <pspctrl.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <stdio.h>
#include <pspgu.h>
#include <pspgum.h>

#include "display.h"
#include "mainwin.h"
#include "fat.h"
#include "ctrl.h"
#include "charsets.h"
#include "wordlist.h"
#include "wordlib.h"
#include "mp3.h"
#include "./common/log.h"
#include "ttf.h"
#include "recite-timer.h"
#include "config.h"

/* Define the module info section */
PSP_MODULE_INFO("CONTROLTEST", 0, 1, 1);

/* Define the main thread's attribute value (optional) */
PSP_MAIN_THREAD_ATTR(THREAD_ATTR_USER | THREAD_ATTR_VFPU);

/* Define printf, just to make typing easier */
#define printf	pspDebugScreenPrintf

static unsigned int __attribute__((aligned(16))) list[262144];
static char appdir[256];

void dump_threadstatus(void);

int done = 0;

/* Exit callback */
int exit_callback(int arg1, int arg2, void *common)
{
	wordlist_end();
	wordlib_end();
	write_config();

        fat_free();
        disp_free_font();
#ifdef _DEBUG
	log_close();
#endif
	mp3_end();
	done = 1;
	return 0;

}

/* Callback thread */
int CallbackThread(SceSize args, void *argp)
{
	int cbid;

	cbid = sceKernelCreateCallback("Exit Callback", exit_callback, NULL);
	sceKernelRegisterExitCallback(cbid);
	sceKernelSleepThreadCB();

	return 0;
}

/* Sets up the callback thread and returns its thread id */
int SetupCallbacks(void)
{
	int thid = 0;

	thid = sceKernelCreateThread("update_thread", CallbackThread,
				     0x11, 0xFA0, 0, 0);
	if(thid >= 0)
	{
		sceKernelStartThread(thid, 0, 0);
	}

	return thid;
}

void gui_init() 
{
        sceGuInit();

        sceGuStart(GU_DIRECT,list);
        sceGuDrawBuffer(GU_PSM_8888,(void*)0,BUF_WIDTH);
        sceGuDispBuffer(SCR_WIDTH,SCR_HEIGHT,(void*)0x88000,BUF_WIDTH);
        sceGuDepthBuffer((void*)0x110000,BUF_WIDTH);
        sceGuOffset(2048 - (SCR_WIDTH/2),2048 - (SCR_HEIGHT/2));
        sceGuViewport(2048,2048,SCR_WIDTH,SCR_HEIGHT);
        sceGuDepthRange(0xc350,0x2710);
        sceGuScissor(0,0,SCR_WIDTH,SCR_HEIGHT);
        sceGuEnable(GU_SCISSOR_TEST);
        sceGuDepthFunc(GU_GEQUAL);
        sceGuEnable(GU_DEPTH_TEST);
        sceGuFrontFace(GU_CCW);
        sceGuShadeModel(GU_SMOOTH);
        sceGuEnable(GU_CULL_FACE);
        sceGuFinish();
        sceGuSync(0,0);

        sceDisplayWaitVblankStart();
        sceGuDisplay(GU_TRUE);

}

void load_font()
{
	char efontfile[256], cfontfile[256];	
	
	getcwd(appdir, 256);
	strcat(appdir,"/");
	strcpy(efontfile, appdir);
	strcpy(cfontfile, appdir);
	strcat(efontfile, "fonts/ASC16");
	strcat(cfontfile, "fonts/GBK16");
	disp_load_font(efontfile, cfontfile);

}

#ifdef _DEBUG
void log_init()
{
        char logfile[256];

        getcwd(appdir, 256);
        strcat(appdir,"/");
        strcpy(logfile, appdir);
        strcat(logfile, "recite.log");
        log_open(logfile);

}
#endif

int main(void)
{
	SceCtrlData pad;
#ifdef _DEBUG
	log_init();
#endif
	disp_init();
	load_font();

	ttf_init();

	SetupCallbacks();

	sceCtrlSetSamplingCycle(0);
	sceCtrlSetSamplingMode(PSP_CTRL_MODE_ANALOG);

	fat_init();
	read_config();
	wordlib_init();
	wordlist_init();	

	gui_init();
	mp3_init();
	window_init();
	
	while (!done) {
		sceCtrlReadBufferPositive(&pad, 1); 
		dword key = ctrl_read();
		active_win->handle_event(key);
		sceKernelDelayThread(50000);
		check_timeout();

	}

	sceGuTerm();
	sceKernelExitGame();
	return 0;
}
