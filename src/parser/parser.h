#ifndef OH_PARSER_H_
#define OH_PARSER_H_

// Search and open `ohfile` or `Ohfile` in root directory which `oh` cli called.
// Now its die in any error.
// TODO: Make error handling by return value.
int parser_init();

#endif
