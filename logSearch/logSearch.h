/**
 *  logSearch Tools
 *
 *  Nginx access log maybe very large, give begin/end time, it will be output log in this range by order.
 *
 *
 */
#ifndef __LOG_SEARCH_H
#define __LOG_SEARCH_H
#include "common.h"

/**
 * Search Struct.
 *
 *  "@timestamp":"2018-04-23T21:30:00+08:00","host":"10.3.17.181","clientip":"10.5.21.233",
 *                ^                       ^ 
 *                p_start                 p_end
 */
struct search {
	ull p_start;
	ull p_end;
	ull fileSize;
	ull t_start;		// filter time begin.
	ull t_end;		// filter time end.
	char *time_format;
	char *filename;
	FILE *fp;
} search;

// get file size
ull get_filesize(char *filename);
// parse time from line string.
ull sub_search(ull begin, ull end, ull flag);
ull get_timestamp(ull position);
void echo_log(ull begin, ull end);
int parse_params(int argc, char *argv[]);
void help();
ull parse_time(char *line);
void log_search();

#endif
