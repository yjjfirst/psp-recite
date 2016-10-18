#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>
#include <pspkernel.h>
#include "common/log.h"
#include "config.h"

#define CONFIG_FILE "config.dat"

struct app_config config;

int default_config()
{
	config.wordlib = 0;
	config.activeword = 0;
	return 0;
}

int read_config()
{
	char file[256];
	char appdir[256];
	int fd = -1;

	getcwd(appdir, 256);
	strcat(appdir,"/");
	strcpy(file, appdir);
	strcat(file, CONFIG_FILE);

	if ( (fd = sceIoOpen(file, PSP_O_RDONLY, 0777)) < 0) {
		default_config();
		log_msg("conf", "Can not config file %s.\n", file);
		return 0;
	}
	
        sceIoRead(fd, &config, sizeof(config));
        sceIoClose(fd);
	return 0;
}

int write_config()
{
	char file[256];
	char appdir[256];
	int fd = -1;

	getcwd(appdir, 256);
	strcat(appdir,"/");
	strcpy(file, appdir);
	strcat(file, CONFIG_FILE);

	if ( (fd = sceIoOpen(file, PSP_O_WRONLY | PSP_O_CREAT | PSP_O_TRUNC, 0777)) < 0) {
		log_msg("conf", "Can not config file %s.\n", file);
		return 0;
	}

	sceIoWrite(fd, &config, sizeof(config));
	sceIoClose(fd);
	return 0;
}

