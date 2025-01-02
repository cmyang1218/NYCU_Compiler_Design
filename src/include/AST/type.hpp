#ifndef __AST_TYPE_H
#define __AST_TYPE_H

enum TypeEnum : int {
    TYPE_VOID,
    TYPE_INT,
    TYPE_REAL,
    TYPE_STR,
    TYPE_BOOL
};
extern const char *typeStr[];

#endif