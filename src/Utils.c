#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <ctype.h>
#include "Utils.h"

int macCompare(u_int8_t *a, u_int8_t *b){
	if((a[0] != b[0]) || (a[1] != b[1]) || (a[2] != b[2]) || (a[3] != b[3]) || (a[4] != b[4]) || (a[5] != b[5])) return 0;
	return 1;
}

int write_buffer_to_file(char *file_name, char *buffer, u_int32_t size){
	FILE *fp = NULL;
	int ret;

	fp = fopen((const char*)file_name, "w");
	if(!fp){
		fprintf(stderr, "ERROR: cannot open file %s\n", file_name);
		goto bail;
	}

	ret = fwrite(buffer, size, 1, fp);
	if(ret < 1){
		fprintf(stderr, "ERROR: fwrite failed ret: %d\n", ret);
		goto bail;
	}

	fclose(fp);
	return 0;
bail:
	return -1;
}

char *getMD5(char *filename) {
	char *md5 = malloc(32);
	char buffer[256];
	char *command = malloc(7 + strlen(filename));

	memcpy(command, "md5sum ", 7);
	memcpy(&command[7], filename, strlen(filename));

	FILE *fp = popen(command, "r");
	sleep(1);

	while (fgets(buffer, sizeof(buffer) - 1, fp) != NULL){};

	memcpy(md5, buffer, 32);

	pclose(fp);
	free(command);
	return md5;
}
