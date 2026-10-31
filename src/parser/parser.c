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

// Rule name length include null.
#define MAX_RULE_NAME_LENGTH 256

// Rule command length include null.
#define MAX_RULE_CMD_LENGTH 1024

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
static FILE* ohfile;

int parser_init(void) {
	ohfile = open_ohfile();
	if (ohfile == NULL) {
		return 0;
	}

	return 1;
}

void parser_free(Nodes nodes) {
	fclose(ohfile);
	for (size_t i = 0; i < nodes.count; ++i) {
		free(nodes.data[i].as.rule.name);
		DAP_POINTERS_FREE(nodes.data[i].as.rule.deps);
		DAP_POINTERS_FREE(nodes.data[i].as.rule.commands);
	}

	DA_FREE(nodes);
}

static int is_ident(const Cursor* cursor) {
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

static void skip_comments(Cursor* cursor) {
	if (cursor_peek(cursor) == '#') {
		while (cursor_peek(cursor) != '\n') {
			cursor_advance(cursor);
		}

		cursor_advance(cursor);
	}
}

static char* parse_ident(char* buf, Cursor* cursor) {
	size_t buf_count = 0;

	while (is_ident(cursor)) {
		if (buf_count >= MAX_RULE_NAME_LENGTH - 1) {
			die("Buffer overflow. Rule name cannot be over %d characters\n", MAX_RULE_NAME_LENGTH - 1);
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
static int file_exists(const char* path) {
	FILE* f = fopen(path, "r");
	if (f == NULL) {
		return 0;
	}

	fclose(f);
	return 1;
}

static char* parse_filename(Cursor* cursor) {
	if (!is_ident(cursor)) {
		return NULL;
	}

	char buf[MAX_RULE_NAME_LENGTH];
	while (is_ident(cursor)) {
		parse_ident(buf, cursor);
	}

	if (!file_exists(buf)) {
		die("Expected valid file path: %s not exists\n", buf);
	}

	return strdup(buf);
}

static void parse_filenames(FilePaths* paths, Cursor* cursor) {
	DA_APPEND(*paths, parse_filename(cursor));

	skip_whitespaces(cursor);

	if (cursor_peek(cursor) == '|') {
		cursor_advance(cursor);
		skip_whitespaces(cursor);
		parse_filenames(paths, cursor);
	}
}

static FilePaths* parse_rule_deps(Cursor* cursor) {
	FilePaths* paths = calloc(1, sizeof(*paths));
	skip_whitespaces(cursor);

	if (cursor_peek(cursor) == '(') {
		cursor_advance(cursor);
		skip_whitespaces(cursor);

		if (is_ident(cursor)) {
			parse_filenames(paths, cursor);
		}

		skip_whitespaces(cursor);

		if (cursor_peek(cursor) != ')') {
			die("Expected closing paren in dependency list\n");
		}

		cursor_advance(cursor);
	}

	return paths;
}

static void parse_rule_cmd(Commands* commands, Cursor* cursor) {
	// In current position, we start from '\t', so advance.
	cursor_advance(cursor);

	char buf[MAX_RULE_CMD_LENGTH];
	size_t buf_count = 0;
	while (cursor_peek(cursor) != '\n') {
		if (buf_count >= MAX_RULE_CMD_LENGTH - 1) {
			die("Buffer overflow. Rule command cannot be over %d characters\n", MAX_RULE_CMD_LENGTH - 1);
		}

		buf[buf_count++] = cursor_peek(cursor);
		cursor_advance(cursor);
	}

	DA_APPEND(*commands, strdup(buf));

	cursor_advance(cursor);

	if (cursor_peek(cursor) == '\t') {
		parse_rule_cmd(commands, cursor);
	}
}

static void parse_rule(Node* node, Cursor* cursor) {
	char name_buf[MAX_RULE_NAME_LENGTH];
	Commands* commands = calloc(1, sizeof(*commands));


	if (is_ident(cursor)) {
		parse_rule_name(name_buf, cursor);
	}

	FilePaths* paths = parse_rule_deps(cursor);

	cursor_advance(cursor);
	if (cursor_peek(cursor) == '\t') {
		parse_rule_cmd(commands, cursor);
	}

	cursor_advance(cursor);

	node->as.rule = rule_new(strdup(name_buf), paths, commands);
}

static Node parser_parse_node(Cursor* cursor) {
	Node node = node_new(NK_RULE);

	skip_whitespaces(cursor);
	skip_comments(cursor);

	parse_rule(&node, cursor);

	return node;
}

Nodes parser_parse(void) {
	Cursor cursor = {.pos = 0};
	int ch;
	Nodes nodes = {0};

	while ((ch = fgetc(ohfile)) != EOF) {
		DA_APPEND(cursor.input, ch);
	}

	while (cursor.pos < cursor.input.count) {
		Node node = parser_parse_node(&cursor);
		DA_APPEND(nodes, node);
	}

	DA_FREE(cursor.input);
	return nodes;
}
