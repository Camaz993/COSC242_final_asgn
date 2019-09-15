#ifndef MYLIB_H_
#define MYLIB_H_

#include <stdlib.h>
#include <stdio.h>

extern void *emalloc(size_t s);
extern void *erealloc(void *, size_t);
extern int getword(char *s, int limit, FILE *stream);
int is_prime(int x);
int next_prime(int x);

#endif
