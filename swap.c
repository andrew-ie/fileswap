#include <stdio.h>
#include <fcntl.h>
#include <libgen.h>
#include <errno.h>
#include <error.h>
#include <unistd.h>
#include <syscall.h>
#include <string.h>
#include <linux/fs.h>
#include <stdlib.h>
/*
 * Structure that contains the directory and file information
 * separated to alow processing
 */
struct dirfile {
	char *dir;
	char *filename;
};
/*
 * Function to generate the directory structure from a path.
 */
struct dirfile generatePath(char *path) {
	int len = strlen(path);
	char *dir = malloc(len);
	char *file = malloc(len);
	struct dirfile retVal;
	strncpy(dir, path, len);
	strncpy(file, path, len);
	retVal.dir = dirname(dir);
	retVal.filename = basename(file);
	return retVal;
}
/*
 * Entry point
 */
int main(int argc, char **argv) {
	int olddirfd;
	int newdirfd;
	int retVal = -1;
	if (argc != 3) {
		fprintf(stderr, "Usage %s <file1> <file2>\n", argv[0]);
	}
	else {
		struct dirfile first = generatePath(argv[1]);
		struct dirfile second = generatePath(argv[2]);
		olddirfd = open(first.dir, O_DIRECTORY);
		newdirfd = open(second.dir, O_DIRECTORY);
		if (olddirfd >= 0 && newdirfd >=0) {
			retVal = syscall(SYS_renameat2, olddirfd, first.filename, newdirfd, second.filename, RENAME_EXCHANGE);
			if (retVal) {
				printf("%d\n", errno);
				perror("Error renaming");
			}
		}
		if (olddirfd >= 0) {
			close(olddirfd);
		}
		if (newdirfd >= 0) {
			close(newdirfd);
		}
	}
	return retVal;
}
