#include "logSearch.h"
#include "fileOperation.h"

static struct search gSearch;
static ull fileSize = 0;

int main(int argc, char *argv[])
{
	parse_params(argc, argv);
	log_search();
	return 0;
}

ull
parse_time(char *line)
{
	if (line[0] == '\0') {
		perror("get time error\n");
	}

	struct tm timeinfo;

	char time[TIME_MAX];
	strncpy(time, line + gSearch.p_start, gSearch.p_end - gSearch.p_start);
	strptime(time, gSearch.time_format, &timeinfo);
	return (unsigned long) mktime(&timeinfo);
}

ull
get_timestamp(ull position)
{
	if (position == fileSize) {
		return -1;
	}
	if (fseek(gSearch.fp, position, SEEK_SET)) {
		perror("fseek error\n");
	}

	char *buf = (char *) malloc(sizeof (buf) * LINE_MAX);
	log_getline(gSearch.fp, position, &buf);
	ull timestamp = parse_time(buf);
	free(buf);
	return timestamp;
}

ull
sub_search(ull begin, ull end, ull flag)
{
	ull middle = (begin + end) / 2;

	if (begin == middle || end == middle) {
		return middle;
	}

	ull timestamp = get_timestamp(middle);
	if (timestamp < flag) {
		return sub_search(middle + 1, end, flag);
	}

	if (timestamp > flag) {
		return sub_search(begin, middle - 1, flag);
	}

	return middle;
}

void
log_search()
{
	gSearch.fp = fopen(gSearch.filename, "r");
	gSearch.fileSize = get_filesize(gSearch.filename);

	ull flag_begin = sub_search(0, gSearch.fileSize, gSearch.t_start);
	ull flag_end = sub_search(flag_begin, gSearch.fileSize, gSearch.t_end);

	char *buf = (char *) malloc(sizeof (buf) * LINE_MAX);
	unsigned long long prev_position = 0;
	int iter = 0;
	do {
		prev_position = log_prev(flag_begin, gSearch.fp, &buf);
		if (parse_time(buf) < gSearch.t_start) {
			break;
		}
		flag_begin -= prev_position;
		fseek(gSearch.fp, flag_begin, SEEK_SET);
		fgets(buf, LINE_MAX, gSearch.fp);
		iter++;
	}
	while (prev_position != 0 && flag_begin >= 0);
	if (flag_begin < 0) {
		flag_begin = 0;
	}

	prev_position = log_prev(flag_end, gSearch.fp, &buf);
	flag_end -= prev_position;
	do {
		fgets(buf, LINE_MAX, gSearch.fp);
		if (parse_time(buf) > gSearch.t_end) {
			break;
		}
		flag_end = ftell(gSearch.fp);
	}
	while (flag_end < gSearch.fileSize);

	free(buf);

	log_echo(flag_begin, flag_end, gSearch.fp);
	fclose(gSearch.fp);
}

ull
get_filesize(char *filename)
{
	struct stat statbuff;
	if (stat(filename, &statbuff) < 0) {
		perror("get file size error.");
		exit(1);
	}

	return statbuff.st_size;
}

void
help()
{
	fprintf(stderr, "\
    Usage: logSearch [-p <begin>,<end>] [-u <timeformat>] [-f <filename>] [-b <timebegin>] [-e <timeend>]\n\
            search log file by BinarySearch.\n\
            \n\
            -b  time begin  format \"20180606 13:00:00\"\n\
            -e  time end    format \"20180606 13:00:00\"\n\
            -p  time Position, split by ','\n\
            -u  time format, default '%%d/%%b/%%Y:%%H:%%M:%%S'\n\
            -f  log file\n\
            -h  print help\n");
}

int
parse_params(int argc, char *argv[])
{
	int opt;
	char *temp;
	struct tm timeinfo;

	if (argc <= 1) {
		help();
		exit(1);
	}

	gSearch.t_start = 0;
	gSearch.t_end = 0;

	while ((opt = getopt(argc, argv, "p:u:f:b:e:h")) != -1) {
		switch (opt) {
		case 'p':
			temp = strtok(optarg, ",");
			gSearch.p_start = atoi(temp);
			temp = strtok(NULL, ",");
			gSearch.p_end = atoi(temp);
			break;
		case 'u':
			gSearch.time_format =
			    (char *) malloc(sizeof (char) *
					    (strlen(optarg) + 1));
			strncpy(gSearch.time_format, optarg, strlen(optarg));
			break;
		case 'f':
			gSearch.filename =
			    (char *) malloc(sizeof (char) *
					    (strlen(optarg) + 1));
			strncpy(gSearch.filename, optarg, strlen(optarg));
			break;
		case 'b':
			temp =
			    (char *) malloc(sizeof (char) *
					    (strlen(optarg) + 1));
			strptime(optarg, TIME_INPUT_FORMAT, &timeinfo);
			gSearch.t_start = mktime(&timeinfo);
			free(temp);
			break;
		case 'e':
			temp =
			    (char *) malloc(sizeof (char) *
					    (strlen(optarg) + 1));
			strptime(optarg, TIME_INPUT_FORMAT, &timeinfo);
			gSearch.t_end = mktime(&timeinfo);
			free(temp);
			break;
		default:
		case 'h':
			help();
			exit(1);
		}
	}

	if (gSearch.time_format == NULL) {
		gSearch.time_format = "%d/%b/%Y:%H:%M:%S";
	}

	return 0;
}
