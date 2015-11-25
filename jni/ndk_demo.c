#include "../config.h"
#include "common.h"

#include <sys/types.h>
#include <sys/select.h>
#include <sys/stat.h>
#include <sys/time.h>

#include <assert.h>
#include <errno.h>
#include <fcntl.h>
#include <getopt.h>
#include <regex/regex.h>
#include <signal.h>
#include <stdarg.h>
#include <stdbool.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <syslog.h>
#include <unistd.h>

#include <libinotifytools/inotifytools/inotifytools.h>
#include <libinotifytools/inotifytools/inotify.h>
#include "com_forensicsupport_NativeLib.h"

JNIEXPORT jstring JNICALL Java_com_forensicsupport_NativeLib_hello
  (JNIEnv * env, jobject obj) {
		return (*env)->NewStringUTF(env, " You ROXX");
}

JNIEXPORT jint JNICALL Java_com_forensicsupport_NativeLib_add
  (JNIEnv * env, jobject obj, jint value1, jint value2) {
		return (value1 + value2);
}

int main(int argc, char ** argv)
{
	int events = 0;
	int orig_events;
	int quiet = 0;
	unsigned long int timeout = 0;
	int recursive = 0;
	char * format = NULL;
	char * timefmt = NULL;
	char * fromfile = NULL;
	char * outfile = NULL;
	char * regex = NULL;
	char * iregex = NULL;
    int fd;


}
