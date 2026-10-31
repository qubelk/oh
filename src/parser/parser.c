#define _GNU_SOURCE
#include "parser.h"

#include "tools/die.h"
#include "tools/da.h"
#include <ctype.h>
#include <dirent.h>
#include <errno.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/stat.h>

// This enum is returned value of ohfile search.
enum {
	OH_ERR_FAILED_TO_OPEN_DIRECTORY,
	OH_ERR_FILE_NOT_FOUND,
	OH_OHFILE_UPPER,
	OH_OHFILE_LOWER,
};

// Search for `Ohfile` or `ohfile` in current directory.
// @return
// 	OH_ERR_FAILED_TO_OPEN_DIRECTORY: if failed to open current directory.
// 	OH_ERR_FILE_NOT_FOUND: if file not found.
//	OH_OHFILE_UPPER: if its `Ohfile`.
// 	OH_OHFILE_LOWER: if its `ohfile`.
//
// This returns need for pattern matching.
static int search_ohfile(void) {
	DIR* rootdir = opendir("./");
	if (rootdir == NULL) {
		return OH_ERR_FAILED_TO_OPEN_DIRECTORY;
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
			return OH_OHFILE_UPPER;
		} else if (strcmp(dir->d_name, "ohfile") == 0) {
			closedir(rootdir);
			return OH_OHFILE_LOWER;
		}
	}

	closedir(rootdir);
	return OH_ERR_FILE_NOT_FOUND;
}

static FILE* open_ohfile(void) {
	int ret = search_ohfile();

	switch (ret) {
		case OH_ERR_FAILED_TO_OPEN_DIRECTORY: {
			perror("Failed to open current directory");
			return NULL;
		}

		case OH_ERR_FILE_NOT_FOUND: {
			fprintf(stderr, "Failed to find `Ohfile` or `ohfile`: File doesn't exists\n");
			return NULL;
		}

		case OH_OHFILE_UPPER: {
			FILE* f = fopen("Ohfile", "r");
			if (f == NULL) {
				perror("Failed to open Ohfile");
				return NULL;
			}

			return f;
		}

		case OH_OHFILE_LOWER: {
			FILE* f = fopen("ohfile", "r");
			if (f == NULL) {
				perror("Failed to open ohfile");
				return NULL;
			}

			return f;
		}
	}
}

// Ohfile opening in parser_init, main function checks parser_init for fail, and die if its.
// So no need to check this pointer to NULL.
FILE* ohfile;

int parser_init(void) {
	ohfile = open_ohfile();
	if (ohfile == NULL) {
		return 0;
	}

	return 1;
}

void parser_free(void) {
	fclose(ohfile);
}

static int is_ident(char* line, size_t index) {
	return isalnum(line[index]) || line[index] == '_' || line[index] == '.';
}

static void skip_whitespaces(char* line, size_t* index) {
	size_t i = *index;
	if (line[i] == ' ') {
		while (line[i] == ' ') {
			++i;
		}
	}

	*index = i;
}

static char* parse_ident(char* buf, char* line, size_t* index) {
	size_t i = *index;
	size_t buf_count = 0;

	while (is_ident(line, i)) {
		if (buf_count >= 256 - 1) {
			die("Buffer overflow. Rule name cannot be over 255 characters\n");
		}

		buf[buf_count++] = line[i];
		++i;
	}

	*index = i;

	return buf;
}

static char* parse_rule_name(char* buf, char* line, size_t* index) {
	parse_ident(buf, line, index);

	size_t i = *index;

	skip_whitespaces(line, &i);

	if (line[i] != ':') {
		die("Expect `:` after `%s` rule name\n", buf);
	}

	++i;

	*index = i;

	return buf;
}

// Check exists file or not.
// @return
// 	0: if not.
// 	1: if exists.
static int file_exists(char* path) {
	FILE* f = fopen(path, "r");
	if (f == NULL) {
		return 0;
	}

	fclose(f);
	return 1;
}

static FilePaths parse_rule_deps(char* line, size_t* index) {
	size_t i = *index;
	FilePaths paths = {0};
	skip_whitespaces(line, &i);

	if (line[i] == '(') {
		++i;
		skip_whitespaces(line, &i);

		if (is_ident(line, i)) {
			char buf[256];
			while (is_ident(line, i)) {
				parse_ident(buf, line, &i);
			}

			if (!file_exists(buf)) {
				die("Expected valid file path: %s not exists\n", buf);
			}

			DA_APPEND(paths, strdup(buf));
		}

		skip_whitespaces(line, &i);

		if (line[i] != ')') {
			die("Expected closing paren in dependency list\n");
		}
	}

	*index = i;

	return paths;
}

static Node* parse_rule(char* line, size_t* i) {
	char buf[256];
	parse_rule_name(buf, line, i);
	FilePaths paths = parse_rule_deps(line, i);
	printf("Name: %s\n", buf);
	DA_POINTERS_FREE(paths);
}

Nodes parser_parse(void) {
	char* line;
	size_t line_size;
	ssize_t line_length;

	while ((line_length = getline(&line, &line_size, ohfile)) != EOF) {
		for (size_t i = 0; i < line_length; ++i) {
			printf("Char: %c\n", line[i]);

			if (line[i] == '#') {
				break;
			}

			if (is_ident(line, i)) {
				parse_rule(line, &i);
			}
		}
	}


	free(line);
}
