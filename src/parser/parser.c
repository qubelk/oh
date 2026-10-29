#include "parser.h"

#include "tools/die.h"
#include <dirent.h>
#include <errno.h>
#include <stdio.h>
#include <string.h>
#include <sys/stat.h>

// Search for `Ohfile` or `ohfile` in current directory.
// @return
// 	-1: if failed to open current directory.
// 	0: if file not found.
//	1: if its `Ohfile`.
// 	2: if its `ohfile`.
//
// This returns need for pattern matching.
static inline int search_ohfile() {
	DIR* rootdir = opendir("./");
	if (rootdir == NULL) {
		return -1;
	}

	struct stat file_stat;
	struct dirent* dir;
	while ((dir = readdir(rootdir)) != NULL) {
		stat(dir->d_name, &file_stat);

		if (S_ISDIR(file_stat.st_mode)) {
			continue;
		}

		if (strcmp(dir->d_name, "Ohfile") == 0) {
			closedir(rootdir);
			return 1;
		} else if (strcmp(dir->d_name, "ohfile") == 0) {
			closedir(rootdir);
			return 2;
		}
	}

	closedir(rootdir);
	return 0;
}

int parser_init() {
	FILE* f;
	int ret = search_ohfile();

	switch (ret) {
	case -1:
		die("Failed to open current directory: %s\n", strerror(errno));
	case 0:
		die("Failed to find `Ohfile` or `ohfile`: File doesn't exists\n");
	case 1:
		f = fopen("Ohfile", "r");
		if (f == NULL) {
			die("Failed to open Ohfile: %s\n", strerror(errno));
		}
		break;
	case 2:
		f = fopen("ohfile", "r");
		if (f == NULL) {
			die("Failed to open ohfile: %s\n", strerror(errno));
		}
		break;
	}

	return 1;
}
