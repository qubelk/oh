#define _GNU_SOURCE
#include "parser.h"

#include "tools/die.h"
#include "tools/da.h"
#include "cursor.h"
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

static int is_ident(Cursor* cursor) {
	char peek = cursor_peek(cursor);
	return isalnum(peek) || peek == '_' || peek == '.';
}

static void skip_whitespaces(Cursor* cursor) {
	if (cursor_peek(cursor) == ' ') {
		while (cursor_peek(cursor) == ' ') {
			cursor_advance(cursor);
		}
	}
}

static char* parse_ident(char* buf, Cursor* cursor) {
	size_t buf_count = 0;

	while (is_ident(cursor)) {
		if (buf_count >= 256 - 1) {
			die("Buffer overflow. Rule name cannot be over 255 characters\n");
		}

		buf[buf_count++] = cursor_peek(cursor);
		cursor_advance(cursor);
	}

	return buf;
}

static char* parse_rule_name(char* buf, Cursor* cursor) {
	parse_ident(buf, cursor);

	skip_whitespaces(cursor);

	if (cursor_peek(cursor) != ':') {
		die("Expect `:` after `%s` rule name\n", buf);
	}

	cursor_advance(cursor);

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

static FilePaths parse_rule_deps(Cursor* cursor) {
	FilePaths paths = {0};
	skip_whitespaces(cursor);

	if (cursor_peek(cursor) == '(') {
		cursor_advance(cursor);
		skip_whitespaces(cursor);

		if (is_ident(cursor)) {
			char buf[256];
			while (is_ident(cursor)) {
				parse_ident(buf, cursor);
			}

			if (!file_exists(buf)) {
				die("Expected valid file path: %s not exists\n", buf);
			}

			DA_APPEND(paths, strdup(buf));
		}

		skip_whitespaces(cursor);

		if (cursor_peek(cursor) != ')') {
			die("Expected closing paren in dependency list\n");
		}
	}

	return paths;
}

static Node* parse_rule(Cursor* cursor) {
	char buf[256];
	parse_rule_name(buf, cursor);
	FilePaths paths = parse_rule_deps(cursor);
	printf("Name: %s\n", buf);
	printf("Path: %s\n", paths.data[0]);
	DA_POINTERS_FREE(paths);
}

Nodes parser_parse(void) {
	char* line;
	size_t line_size;
	ssize_t line_length;

	while ((line_length = getline(&line, &line_size, ohfile)) != EOF) {
		for (size_t i = 0; i < line_length; ++i) {
			Cursor cursor = {line, i};
			printf("Char: %c\n", cursor_peek(&cursor));

			if (cursor_peek(&cursor) == '#') {
				break;
			}

			if (is_ident(&cursor)) {
				parse_rule(&cursor);
			}

			i += cursor.pos;
		}
	}


	free(line);
}
