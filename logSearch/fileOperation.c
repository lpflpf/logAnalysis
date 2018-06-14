#include "fileOperation.h"

unsigned long long
log_next(unsigned long long position, FILE * fp, char *buf)
{
	fseek(fp, position, SEEK_SET);
	fgets(buf, LINE_MAX, fp);
	return ftell(fp);
}

unsigned long long
log_prev(unsigned long long position, FILE * fp, char **buf)
{
	if (position - LINE_MAX < 0) {
		fseek(fp, 0, SEEK_SET);
	} else {
		fseek(fp, position - LINE_MAX, SEEK_SET);
	}
	fgets(*buf, LINE_MAX, fp);

	int size = 0;
	unsigned long long bufSize = 0;

	while (size < LINE_MAX) {
		bufSize = strlen(*buf);
		if (bufSize + size >= LINE_MAX) {
			return LINE_MAX - size;
		}
		size += bufSize;
		fgets(*buf, LINE_MAX, fp);
	}

	return 0;
}

void
log_echo(unsigned long long begin, unsigned long long end, FILE * fp)
{
	char *buf = (char *) malloc(sizeof (char) * LINE_MAX);
	fseek(fp, begin, SEEK_SET);
	do {
		fgets(buf, LINE_MAX, fp);
		printf("%s", buf);
	} while (ftell(fp) < end);
}

void
log_getline(FILE * fp, unsigned long long position, char **buf)
{
	if (position <= LINE_MAX) {
		if (fgets(*buf, LINE_MAX, fp) == NULL) {
			perror("getline error  \n");
		}
		return;
	}

	log_prev(position, fp, buf);
}
