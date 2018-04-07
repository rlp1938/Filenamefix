/*
 * filenamefix.c
 *
 * Copyright 2018 Robert L (Bob) Parker <rlp1938@gmail.com>
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston,
 * MA 02110-1301, USA.
 *
 *
 */


#include <stdio.h>
#include <stdlib.h>
#include <dirent.h>
#include <sys/types.h>
#include <string.h>
#include <unistd.h>

static int fixit(char *fname);
static char *removedupstr(char *str, char dup);
static char *strmove(char *dest, const char *src);

int main(int argc, char **argv)
{ /* get rid of spaces from file names in a dir, and then rename that
	file to the new name.
  */
	// Nothing clever here, if anything is out of kilter just crash.
	if (argc != 2) goto stupid;

	char *dirname = argv[1];
	if (strcmp(dirname, "-h") == 0) goto help;
	if (strcmp(dirname, "--help") == 0) goto help;
	DIR *dp = opendir(dirname);
	if(!dp) goto stupid;
	struct dirent *dd;
	while((dd = readdir(dp))) {
		if (dd->d_type == DT_REG) { /* Ignore all other fs objects */
			char fnew[NAME_MAX];
			strcpy(fnew, dd->d_name);
			if (fixit(fnew)) {
				fprintf(stdout, "%s\n", fnew);
				/* This where this can get a bit ditzy. I am going
				 * to alter the contents of the dir that I am
				 * examining on the fly. Arguably I should simply be
				 * opening the dir as a file and altering it in place.
				 * */
				char fold[NAME_MAX];
				strcpy(fold, dd->d_name);
				if (closedir(dp) == -1) {
					perror(dirname);
					exit(1);
				}
				if (rename(fold, fnew) == -1) {
					perror(fnew);
					perror(fold);
					exit(1);
				}
				sync();	// I have no idea if this useful or not.
				dp = opendir(dirname);
				/* Worst comes to worst, run the thing more than once */
			}
		}
	}
	closedir(dp);
	return 0;

stupid:
	fputs("Needs a dir name stupid.\n", stderr);
	exit(1);

help:
	fputs("\tReplaces spaces in filenames with '_'\n"
	"\tfixfilename .\n\tRun this in the dir that you want to"
	" fix.\n",
	stderr);
	exit(0);
}

int fixit(char *fname)
{	/* Replaces any ' ' in fname with '_' and returns a count of
	* the number of replacements made.
	* If groups of '_' result, the group is reduced to just 1 '_'.
	* If changes are made, the result is copied back to fname.
	*/
	int count = 0;
	char *cp;
	char buf[NAME_MAX];
	int i;
	for (i = 0, cp = fname; *cp ; i++, cp++) {
		if (*cp == ' ') {
			count++;
			buf[i] = '_';
		} else buf[i] = *cp;
	}
	buf[i] = 0;	// ensure it's zero terminated.
	if (count) { // we have an insane file name.
		char *res = removedupstr(buf, '_');
		strcpy(fname, res);
	}
	return count;
} // fixit()

char *removedupstr(char *str, char dup)
{ /* Replace multiple occurrences of dup in str with just one. */
	char dupstr[3];
	static char result[NAME_MAX];
	dupstr[0] = dupstr[1] = dup;
	dupstr[2] = 0;
	strcpy(result, str);
	char *cp = strstr(result, dupstr);
	while ((cp)) {
		strmove(cp, cp+1);
		cp = strstr(result, dupstr);
	}
	return result;
} // removedupstr()

char *strmove(char *dest, const char *src)
{ /* Moves src to dest in a manner that src and dest may overlap.
	* If the move results in an expansion of the string length the
	* caller must ensure that there is enough room in dest.
	*/
	size_t len = strlen(src);	// number of bytes to move.
	memmove(dest, src, len);
	dest[len] = 0;
	return dest;
} // strmove()

