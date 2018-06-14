#ifndef __FILE_OPERATION
#define __FILE_OPERATION
#include "common.h"

void log_echo(unsigned long long begin, unsigned long long end, FILE * fp);
void log_getline(FILE * fp, unsigned long long position, char **buf);
unsigned long long log_prev(unsigned long long position, FILE * fp, char **buf);
unsigned long long log_next(unsigned long long position, FILE * fp, char *buf);
#endif
