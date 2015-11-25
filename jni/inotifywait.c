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
#include <regex.h>
#include <signal.h>
#include <stdarg.h>
#include <stdbool.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <syslog.h>
#include <unistd.h>
#include <android/log.h>
#include "com_forensicsupport_NativeLib.h"

#include <inotifytools/inotifytools.h>
#include <inotifytools/inotify.h>

extern char *optarg;
extern int optind, opterr, optopt;

#define MAX_STRLEN 4096
#define EXCLUDE_CHUNK 1024
#define APPNAME "MyApp"

#define nasprintf(...) niceassert( -1 != asprintf(__VA_ARGS__), "out of memory")

JNIEXPORT jstring JNICALL Java_com_forensicsupport_NativeLib_hello
  (JNIEnv * env, jobject obj) {
	 __android_log_print(ANDROID_LOG_VERBOSE, APPNAME, "Androido: Call @ JNI");
//	/system("su");
	main(1, "/storage/sdcard0/tmp.txt");

    __android_log_print(ANDROID_LOG_VERBOSE, APPNAME, "Androido: Call @ JNI");
		return (*env)->NewStringUTF(env, " Some Text");
}

JNIEXPORT jint JNICALL Java_com_forensicsupport_NativeLib_add
  (JNIEnv * env, jobject obj, jint value1, jint value2) {
		return (value1 + value2);
}
// METHODS
bool parse_opts(
  int * argc,
  char *** argv,
  int * events,
  bool * monitor,
  int * quiet,
  unsigned long int * timeout,
  int * recursive,
  bool * csv,
  bool * daemon,
  bool * syslog,
  char ** format,
  char ** timefmt,
  char ** fromfile,
  char ** outfile,
  char ** regex,
  char ** iregex
);

void print_help();


char * csv_escape( char * string ) {
	static char csv[MAX_STRLEN+1];
	static unsigned int i, ind;

	if ( strlen(string) > MAX_STRLEN ) {
		return NULL;
	}

	if ( strlen(string) == 0 ) {
		return NULL;
	}

	// May not need escaping
	if ( !strchr(string, '"') && !strchr(string, ',') && !strchr(string, '\n')
	     && string[0] != ' ' && string[strlen(string)-1] != ' ' ) {
		strcpy( csv, string );
		return csv;
	}

	// OK, so now we _do_ need escaping.
	csv[0] = '"';
	ind = 1;
	for ( i = 0; i < strlen(string); ++i ) {
		if ( string[i] == '"' ) {
			csv[ind++] = '"';
		}
		csv[ind++] = string[i];
	}
	csv[ind++] = '"';
	csv[ind] = '\0';

	return csv;
}


void validate_format( char * fmt ) {
	// Make a fake event
	struct inotify_event * event =
	   (struct inotify_event *)malloc(sizeof(struct inotify_event) + 4);
	if ( !event ) {
		__android_log_print(ANDROID_LOG_VERBOSE, APPNAME, "Seem to be out of memory... yikes!\n" );
		exit(EXIT_FAILURE);
	}
	event->wd = 0;
	event->mask = IN_ALL_EVENTS;
	event->len = 3;
	strcpy( event->name, "foo" );
	FILE * devnull = fopen( "/dev/null", "a" );
	if ( !devnull ) {
		__android_log_print(ANDROID_LOG_VERBOSE, APPNAME, "Androido: Couldn't open /dev/null: %s\n", strerror(errno) );
		free( event );
		return;
	}
	if ( -1 == inotifytools_fprintf( devnull, event, fmt ) ) {
		__android_log_print(ANDROID_LOG_VERBOSE, APPNAME, "Androido: Something is wrong with your format string.\n" );
		exit(EXIT_FAILURE);
	}
	free( event );
	fclose(devnull);
}


void output_event_csv( struct inotify_event * event ) {
    char *filename = csv_escape(inotifytools_filename_from_wd(event->wd));
    if (filename != NULL)
        printf("%s,", csv_escape(filename));

	__android_log_print(ANDROID_LOG_VERBOSE, APPNAME, "Androido: %s,", csv_escape( inotifytools_event_to_str( event->mask ) ) );
	if ( event->len > 0 )
		__android_log_print(ANDROID_LOG_VERBOSE, APPNAME, "Androido: %s", csv_escape( event->name ) );
	__android_log_print(ANDROID_LOG_VERBOSE, APPNAME, "Androido: \n");
}


void output_error( bool syslog, char* fmt, ... ) {
	va_list va;
	va_start(va, fmt);
	if ( syslog ) {
		vsyslog(LOG_INFO, fmt, va);
	} else {
		vfprintf(stderr, fmt, va);
	}
	va_end(va);
}

char * strcopyfun(char * source)
{

	int sourceLen = strlen(source);
	int LenCtr = 0;
	char * dest;
	__android_log_print(ANDROID_LOG_VERBOSE, APPNAME, "Androido: in  %s op %s ", source, dest);
	__android_log_print(ANDROID_LOG_VERBOSE, APPNAME, "Androido: in  %d ", sourceLen);
	dest[0] = source[0];
	__android_log_print(ANDROID_LOG_VERBOSE, APPNAME, "Androido: in  %d ", LenCtr);
	while(LenCtr < sourceLen)
	{
		__android_log_print(ANDROID_LOG_VERBOSE, APPNAME, "Androido: in  %d ", LenCtr);
		//dest[LenCtr] = source[LenCtr];

		//__android_log_prnt(ANDROID_LOG_VERBOSE, APPNAME, "Androido: in  %d ", LenCtr);
		LenCtr++;
	}
	dest[LenCtr] = '\0';
	__android_log_print(ANDROID_LOG_VERBOSE, APPNAME, "Androido: in  %s op %s ", source, dest);
	return dest;
}

int main(int argc, char ** argv)
{
	int events = 0;
	int orig_events;
	bool monitor = true;
	int quiet = 0;
	unsigned long int timeout = 0;
	int recursive = 1;
	bool csv = false;
	bool daemon = false;
	bool syslog = false;
	char * format = NULL;
	char * timefmt = NULL;
	//__android_log_print(ANDROID_LOG_VERBOSE, APPNAME, "Mian-arg: %s\n", *argv);
	char * fromfile = argv;//NULL;
	char * outfile = NULL;
	char * regex = NULL;
	char * iregex = NULL;
	pid_t pid;
    int fd;
    __android_log_print(ANDROID_LOG_VERBOSE, APPNAME, "Androido: Call@ main-1 fromfile  %s", fromfile);
//	fromfile = strcopyfun(andr);
    //strcpy(fromfile, "/storage/sdcard0/tmp.txt");

	__android_log_print(ANDROID_LOG_VERBOSE, APPNAME, "Androido: Call@ main-1.0 %s", fromfile);
	__android_log_print(ANDROID_LOG_VERBOSE, APPNAME, "Androido: Call@ main-1.1");
	// Parse commandline options, aborting if something goes wrong
	if ( !parse_opts(&argc, &argv, &events, &monitor, &quiet, &timeout,
	                 &recursive, &csv, &daemon, &syslog, &format, &timefmt, 
                         &fromfile, &outfile, &regex, &iregex) ) {

		__android_log_print(ANDROID_LOG_VERBOSE, APPNAME, "Androido: Call@ main-1.01");
		return EXIT_FAILURE;
	}

	__android_log_print(ANDROID_LOG_VERBOSE, APPNAME, "Androido: Call@ main-1.2");
	if ( !inotifytools_initialize() )
	{
		__android_log_print(ANDROID_LOG_VERBOSE, APPNAME, "Androido: Couldn't initialize inotify.  Are you running Linux "
		                "2.6.13 or later, and was the\n"
		                "CONFIG_INOTIFY option enabled when your kernel was "
		                "compiled?  If so, \n"
		                "something mysterious has gone wrong.  Please e-mail "
		                PACKAGE_BUGREPORT "\n"
		                " and mention that you saw this message.\n");
		return EXIT_FAILURE;
	}
	else
	{
		__android_log_print(ANDROID_LOG_VERBOSE, APPNAME, "Androido: initialize inotify success.");
	}
	if ( timefmt ) inotifytools_set_printf_timefmt( timefmt );
	if (
		(regex && !inotifytools_ignore_events_by_regex(regex, REG_EXTENDED) ) ||
		(iregex && !inotifytools_ignore_events_by_regex(iregex, REG_EXTENDED|
		                                                        REG_ICASE))
	) {
		__android_log_print(ANDROID_LOG_VERBOSE, APPNAME, "Androido: Error in `exclude' regular expression.\n");
		return EXIT_FAILURE;
	}


	if ( format ) validate_format(format);
	__android_log_print(ANDROID_LOG_VERBOSE, APPNAME, "Androido2: Chk event. %d %B %d", events, monitor, recursive);
	// Attempt to watch file
	// If events is still 0, make it all events.
	if (events == 0)
		events = IN_ALL_EVENTS;
        orig_events = events;
        if ( monitor && recursive ) {
        	__android_log_print(ANDROID_LOG_VERBOSE, APPNAME, "Androido2: add def event.");
                events = events | IN_CREATE | IN_MOVED_TO | IN_MOVED_FROM;
        }

	FileList list = construct_path_list( argc, argv, fromfile );
	__android_log_print(ANDROID_LOG_VERBOSE, APPNAME, "Androido2: argc %d, argv %s, fromfile %s list.watch_files[0] %d", argc, argv, fromfile, list.watch_files[0]);
	if (0 == list.watch_files[0]) {
		__android_log_print(ANDROID_LOG_VERBOSE, APPNAME, "Androido2: No files specified to watch!\n");
		return EXIT_FAILURE;
	}


	__android_log_print(ANDROID_LOG_VERBOSE, APPNAME, "Androido2: Call@ main-1.5, %d", daemon);
    // Daemonize - BSD double-fork approach
	if ( daemon ) {

		pid = fork();
	        if (pid < 0) {
			__android_log_print(ANDROID_LOG_VERBOSE, APPNAME, "Androido: Failed to fork1 whilst daemonizing!\n");
	                return EXIT_FAILURE;
	        } 
	        if (pid > 0) {
			_exit(0);
	        }
		if (setsid() < 0) {
			__android_log_print(ANDROID_LOG_VERBOSE, APPNAME, "Androido: Failed to setsid whilst daemonizing!\n");
	                return EXIT_FAILURE;
	        }
		signal(SIGHUP,SIG_IGN);
	        pid = fork();
	        if (pid < 0) {
	                __android_log_print(ANDROID_LOG_VERBOSE, APPNAME, "Androido: Failed to fork2 whilst daemonizing!\n");
	                return EXIT_FAILURE;
	        }
	        if (pid > 0) {
	                _exit(0);
	        }
		if (chdir("/") < 0) {
			__android_log_print(ANDROID_LOG_VERBOSE, APPNAME, "Androido: Failed to chdir whilst daemonizing!\n");
	                return EXIT_FAILURE;
	        }

		// Redirect stdin from /dev/null
	        fd = open("/dev/null", O_RDONLY);
		if (fd != fileno(stdin)) {
			dup2(fd, fileno(stdin));
			close(fd);
		}

		// Redirect stdout to a file
	        fd = open(outfile, O_WRONLY | O_CREAT | O_APPEND, 0600);
		if (fd < 0) {
                        __android_log_print(ANDROID_LOG_VERBOSE, APPNAME, "Androido: Failed to open output file %s\n", outfile );
                        return EXIT_FAILURE;
                }
		if (fd != fileno(stdout)) {
			dup2(fd, fileno(stdout));
			close(fd);
		}

        // Redirect stderr to /dev/null
		fd = open("/dev/null", O_WRONLY);
	        if (fd != fileno(stderr)) {
	                dup2(fd, fileno(stderr));
	                close(fd);
	        }
	
        } else if (outfile != NULL) { // Redirect stdout to a file if specified
		fd = open(outfile, O_WRONLY | O_CREAT | O_APPEND, 0600);
		if (fd < 0) {
			__android_log_print(ANDROID_LOG_VERBOSE, APPNAME, "Androido: Failed to open output file %s\n", outfile );
			return EXIT_FAILURE;
		}
		if (fd != fileno(stdout)) {
			dup2(fd, fileno(stdout));
			close(fd);
		}
        }

        if ( syslog ) {
		openlog ("inotifywait", LOG_CONS | LOG_PID | LOG_NDELAY, LOG_DAEMON);
        }

	if ( !quiet ) {
		if ( recursive ) {
			output_error( syslog, "Setting up watches.  Beware: since -r "
				"was given, this may take a while!\n" );
		} else {
			output_error( syslog, "Setting up watches.\n" );
		}
	}

	int i = 0;
	char *tempFile =list.watch_files[i];
	__android_log_print(ANDROID_LOG_VERBOSE, APPNAME, "Androido3: Call@ main-2 %d %s", list.watch_files[i], tempFile);;
	// now watch files
	for ( i = 0; list.watch_files[i]; ++i ) {
		char const *this_file = list.watch_files[i];
		if ( (recursive && !inotifytools_watch_recursively_with_exclude(this_file, events, list.exclude_files )) || (!recursive && !inotifytools_watch_file( this_file, events )) ){
			if ( inotifytools_error() == ENOSPC ) {
				__android_log_print(ANDROID_LOG_VERBOSE, APPNAME, "Androido3: Failed to watch %s ", this_file );
				output_error( syslog, "Failed to watch %s; upper limit on inotify "
				                "watches reached!\n", this_file );
				output_error( syslog, "Please increase the amount of inotify watches "
				        "allowed per user via `/proc/sys/fs/inotify/"
				        "max_user_watches'.\n");
			}
			else {
				__android_log_print(ANDROID_LOG_VERBOSE, APPNAME, "Androido3: Failed to watch %s and err %s\n", this_file, strerror( inotifytools_error()));
				output_error( syslog, "Couldn't watch %s: %s\n", this_file,
				        strerror( inotifytools_error() ) );
			}
			return EXIT_FAILURE;
		}
		else
		{
			__android_log_print(ANDROID_LOG_VERBOSE, APPNAME, "Androido3: watch no err");
		}
	}

	if ( !quiet ) {
		__android_log_print(ANDROID_LOG_VERBOSE, APPNAME, "Androido3: Watches established.\n" );
		output_error( syslog, "Watches established.\n" );
	}
	__android_log_print(ANDROID_LOG_VERBOSE, APPNAME, "Androido4: Watches established.\n" );
	// Now wait till we get event
	struct inotify_event * event;
	char * moved_from = 0;

	do {
		__android_log_print(ANDROID_LOG_VERBOSE, APPNAME, "Androido4: events.\n" );
		event = inotifytools_next_event( timeout );
		if ( !event )
		{
			if ( !inotifytools_error() )
			{
				return EXIT_TIMEOUT;
			}
			else
			{
				output_error( syslog, "%s\n", strerror( inotifytools_error() ) );
				return EXIT_FAILURE;
			}
		}

		if ( quiet < 2 && (event->mask & orig_events) )
		{
			//__android_log_print(ANDROID_LOG_VERBOSE, APPNAME, "AFSF: events: quiet. %d event name %s event ws %d  filename -not found", event->mask, event->name, event->wd);
			char *file_path, *temp_file, *temp_str;
			if(event->mask)
			{
				file_path = inotifytools_filename_from_wd(event->wd);
				temp_file = &file_path[strlen(file_path) - strlen(event->name)];
				//__android_log_print(ANDROID_LOG_VERBOSE, APPNAME, "AFSF: events: %s " , temp_str	);
				//__android_log_print(ANDROID_LOG_VERBOSE, APPNAME, "AFSF: events: %d file loc %s filename %s" , event->mask, file_path, event->name);
				//if(event->mask  4096 )
					//strncat(file_path,event->name,strlen(file_path));
				//__android_log_print(ANDROID_LOG_VERBOSE, APPNAME, "AFSF: events: %d @ %s filename %s" ,event->wd, file_path, event->name);
				//__android_log_print(ANDROID_LOG_VERBOSE, APPNAME, "AFSF-time: """);
				if(event->mask  < 4096 )
				{
					//if (strcmp(temp_file,event->name) != 0)
						//strncat(file_path,event->name,strlen(file_path));
					__android_log_print(ANDROID_LOG_VERBOSE, APPNAME, "AFSF: file event %d @ %s of file %s ", event->mask, file_path, event->name);
				}
				else
				{
					if ((strcmp(temp_file,event->name) == 0) && strlen(event->name) > 0 )
						__android_log_print(ANDROID_LOG_VERBOSE, APPNAME, "AFSF: file event @ %s of file %s ", file_path, event->name);
					else
						__android_log_print(ANDROID_LOG_VERBOSE, APPNAME, "AFSF: folder event @ %s ",  file_path);
				}


			}


			//char * file_loc = inotifytools_filename_from_wd(event->wd);//event->name
			//if(isdir(event->name))
			//__android_log_print(ANDROID_LOG_VERBOSE, APPNAME, "AFSF: events: quiet. fmt %s ", event->fmt);
			if ( csv ) {
				output_event_csv( event );
			}
			else if ( format ) {
				inotifytools_printf( event, format );
			}
			else
			{
				inotifytools_printf( event, "%w %,e %f\n" );
			}
		}

		// if we last had MOVED_FROM and don't currently have MOVED_TO,
		// moved_from file must have been moved outside of tree - so unwatch it.
		if ( moved_from && !(event->mask & IN_MOVED_TO) ) {
			if ( !inotifytools_remove_watch_by_filename( moved_from ) ) {
				output_error( syslog, "Error removing watch on %s: %s\n",
				         moved_from, strerror(inotifytools_error()) );
			}
			free( moved_from );
			moved_from = 0;
		}

		if ( monitor && recursive ) {
			if ((event->mask & IN_CREATE) ||
			    (!moved_from && (event->mask & IN_MOVED_TO))) {
				// New file - if it is a directory, watch it
				static char * new_file;
				__android_log_print(ANDROID_LOG_VERBOSE, APPNAME, "Androido6: file Create");
				nasprintf( &new_file, "%s%s",
				           inotifytools_filename_from_wd( event->wd ),
				           event->name );

				if ( isdir(new_file) &&
				    !inotifytools_watch_recursively( new_file, events ) ) {
					output_error( syslog, "Couldn't watch new directory %s: %s\n",
					         new_file, strerror( inotifytools_error() ) );
				}
				free( new_file );
			} // IN_CREATE
			else if (event->mask & IN_MOVED_FROM) {
				nasprintf( &moved_from, "%s%s/",
				           inotifytools_filename_from_wd( event->wd ),
				           event->name );
				// if not watched...
				if ( inotifytools_wd_from_filename(moved_from) == -1 ) {
					free( moved_from );
					moved_from = 0;
				}
			} // IN_MOVED_FROM
			else if (event->mask & IN_MOVED_TO) {
				if ( moved_from ) {
					static char * new_name;
					nasprintf( &new_name, "%s%s/",
					           inotifytools_filename_from_wd( event->wd ),
					           event->name );
					inotifytools_replace_filename( moved_from, new_name );
					free( moved_from );
					moved_from = 0;
				} // moved_from
			}
		}

		fflush( NULL );

	} while ( monitor );

	// If we weren't trying to listen for this event...
	if ( (events & event->mask) == 0 ) {
		// ...then most likely something bad happened, like IGNORE etc.
		return EXIT_FAILURE;
	}

	return EXIT_SUCCESS;
}


bool parse_opts(  int * argc,  char *** argv, int * events,  bool * monitor,   int * quiet,  unsigned long int * timeout,  int * recursive,  bool * csv,  bool * daemon,
  bool * syslog,  char ** format,   char ** timefmt,  char ** fromfile,  char ** outfile,  char ** regex,  char ** iregex)
{
	__android_log_print(ANDROID_LOG_VERBOSE, APPNAME, "Androido: 000");
	assert( argc ); assert( argv ); assert( events ); assert( monitor );
	assert( quiet ); assert( timeout ); assert( csv ); assert( daemon );
	assert( syslog ); assert( format ); assert( timefmt ); assert( fromfile ); 
	assert( outfile ); assert( regex ); assert( iregex );
	__android_log_print(ANDROID_LOG_VERBOSE, APPNAME, "Androido: 001");
	// Short options
	char * opt_string = "mrhcdsqt:fo:e:";

	// Construct array
	struct option long_opts[17];

	// --help
	long_opts[0].name = "help";
	long_opts[0].has_arg = 0;
	long_opts[0].flag = NULL;
	long_opts[0].val = (int)'h';
	// --event
	long_opts[1].name = "event";
	long_opts[1].has_arg = 1;
	long_opts[1].flag = NULL;
	long_opts[1].val = (int)'e';
	int new_event;
	__android_log_print(ANDROID_LOG_VERBOSE, APPNAME, "Androido: 010");
	// --monitor
	long_opts[2].name = "monitor";
	long_opts[2].has_arg = 0;
	long_opts[2].flag = NULL;
	long_opts[2].val = (int)'m';
	// --quiet
	long_opts[3].name = "quiet";
	long_opts[3].has_arg = 0;
	long_opts[3].flag = NULL;
	long_opts[3].val = (int)'q';
	// --timeout
	long_opts[4].name = "timeout";
	long_opts[4].has_arg = 1;
	long_opts[4].flag = NULL;
	long_opts[4].val = (int)'t';
	char * timeout_end = NULL;
	// --filename
	long_opts[5].name = "filename";
	long_opts[5].has_arg = 0;
	long_opts[5].flag = NULL;
	long_opts[5].val = (int)'f';
	// --recursive
	long_opts[6].name = "recursive";
	long_opts[6].has_arg = 0;
	long_opts[6].flag = NULL;
	long_opts[6].val = (int)'r';
	// --csv
	long_opts[7].name = "csv";
	long_opts[7].has_arg = 0;
	long_opts[7].flag = NULL;
	long_opts[7].val = (int)'c';
	// --daemon
	long_opts[8].name = "daemon";
	long_opts[8].has_arg = 0;
	long_opts[8].flag = NULL;
	long_opts[8].val = (int)'d';
	// --syslog
	long_opts[9].name = "syslog";
	long_opts[9].has_arg = 0;
	long_opts[9].flag = NULL;
	long_opts[9].val = (int)'s';
	// --format
	long_opts[10].name = "format";
	long_opts[10].has_arg = 1;
	long_opts[10].flag = NULL;
	long_opts[10].val = (int)'n';
	// format with trailing newline
	static char * newlineformat;
	// --timefmt
	long_opts[11].name = "timefmt";
	long_opts[11].has_arg = 1;
	long_opts[11].flag = NULL;
	long_opts[11].val = (int)'i';
	// --fromfile
	long_opts[12].name = "fromfile";
	long_opts[12].has_arg = 1;
	long_opts[12].flag = NULL;
	long_opts[12].val = (int)'z';
	// --outfile
	long_opts[13].name = "outfile";
	long_opts[13].has_arg = 1;
	long_opts[13].flag = NULL;
	long_opts[13].val = (int)'o';
	// --exclude
	long_opts[14].name = "exclude";
	long_opts[14].has_arg = 1;
	long_opts[14].flag = NULL;
	long_opts[14].val = (int)'a';
	// --excludei
	long_opts[15].name = "excludei";
	long_opts[15].has_arg = 1;
	long_opts[15].flag = NULL;
	long_opts[15].val = (int)'b';
	// Empty last element
	long_opts[16].name = 0;
	long_opts[16].has_arg = 0;
	long_opts[16].flag = 0;
	long_opts[16].val = 0;
	__android_log_print(ANDROID_LOG_VERBOSE, APPNAME, "Androido: 011");
	// Get first option
	char curr_opt = getopt_long(*argc, *argv, opt_string, long_opts, NULL);
	//__android_log_print(ANDROID_LOG_VERBOSE, APPNAME, "AFSF: curr_opt %c", curr_opt);
	// While more options exist...
	while ( (curr_opt != '?') && (curr_opt != (char)-1) )
	{
		switch ( curr_opt )
		{
			// --help or -h
			case 'h':
				print_help();
				// Shouldn't process any further...
				return false;
				break;

			// --monitor or -m
			case 'm':
				*monitor = true;
				break;

			// --quiet or -q
			case 'q':
				(*quiet)++;
				break;

			// --recursive or -r
			case 'r':
				(*recursive)++;
				break;

			// --csv or -c
			case 'c':
				(*csv) = true;
				break;

			// --daemon or -d
			case 'd':
				(*daemon) = true;
				(*monitor) = true;
				(*syslog) = true;
				break;

			// --syslog or -s
			case 's':
				(*syslog) = true;
				break;

			// --filename or -f
			case 'f':
				__android_log_print(ANDROID_LOG_VERBOSE, APPNAME, "Androido: The '--filename' option no longer exists.  "
				                "The option it enabled in earlier\nversions of "
				                "inotifywait is now turned on by default.\n");
				return false;
				break;

			// --format
			case 'n':
				newlineformat = (char *)malloc(strlen(optarg)+2);
				strcpy( newlineformat, optarg );
				strcat( newlineformat, "\n" );
				(*format) = newlineformat;
				break;

			// --timefmt
			case 'i':
				(*timefmt) = optarg;
				break;

			// --exclude
			case 'a':
				(*regex) = optarg;
				break;

			// --excludei
			case 'b':
				(*iregex) = optarg;
				break;

			// --fromfile
			case 'z':
				if (*fromfile) {
					__android_log_print(ANDROID_LOG_VERBOSE, APPNAME, "Androido: Multiple --fromfile options given.\n");
					return false;
				}
				(*fromfile) = optarg;
				break;

			// --outfile
			case 'o':
				if (*outfile) {
					__android_log_print(ANDROID_LOG_VERBOSE, APPNAME, "Androido: Multiple --outfile options given.\n");
					return false;
				}
				(*outfile) = optarg;
				break;

			// --timeout or -t
			case 't':
				*timeout = strtoul(optarg, &timeout_end, 10);
				if ( *timeout_end != '\0' )
				{
					__android_log_print(ANDROID_LOG_VERBOSE, APPNAME, "Androido: '%s' is not a valid timeout value.\n"
					        "Please specify an integer of value 0 or "
					        "greater.\n",
					        optarg);
					return false;
				}
				break;

			// --event or -e
			case 'e':
				// Get event mask from event string
				new_event = inotifytools_str_to_event(optarg);

				// If optarg was invalid, abort
				if ( new_event == -1 )
				{
					__android_log_print(ANDROID_LOG_VERBOSE, APPNAME, "Androido: '%s' is not a valid event!  Run with the "
					                "'--help' option to see a list of "
					                "events.\n", optarg);
					return false;
				}

				// Add the new event to the event mask
				(*events) = ( (*events) | new_event );

				break;


		}

		curr_opt = getopt_long(*argc, *argv, opt_string, long_opts, NULL);

	}
	//__android_log_print(ANDROID_LOG_VERBOSE, APPNAME, "AFSF: %s", *timefmt);
	if ( *monitor && *timeout != 0 ) {
		__android_log_print(ANDROID_LOG_VERBOSE, APPNAME, "Androido: -m and -t cannot both be specified.\n");
		return false;
	}

	if ( *regex && *iregex ) {
		__android_log_print(ANDROID_LOG_VERBOSE, APPNAME, "Androido: --exclude and --excludei cannot both be specified.\n");
		return false;
	}

	if ( *format && *csv ) {
		__android_log_print(ANDROID_LOG_VERBOSE, APPNAME, "Androido: -c and --format cannot both be specified.\n");
		return false;
	}

	if ( !*format && *timefmt ) {
		__android_log_print(ANDROID_LOG_VERBOSE, APPNAME, "Androido: --timefmt cannot be specified without --format.\n");
		return false;
	}

	if ( *format && strstr( *format, "%T" ) && !*timefmt ) {
		__android_log_print(ANDROID_LOG_VERBOSE, APPNAME, "Androido: %%T is in --format string, but --timefmt was not "
		                "specified.\n");
		return false;
	}

	if ( *daemon && *outfile == NULL ) {
		__android_log_print(ANDROID_LOG_VERBOSE, APPNAME, "Androido: -o must be specified with -d.\n");
		return false;
	}

	(*argc) -= optind;
	*argv = &(*argv)[optind];

	// If ? returned, invalid option
	return (curr_opt != '?');
}


void print_help()
{
	__android_log_print(ANDROID_LOG_VERBOSE, APPNAME, "Androido: inotifywait %s\n", PACKAGE_VERSION);
	__android_log_print(ANDROID_LOG_VERBOSE, APPNAME, "Androido: Wait for a particular event on a file or set of files.\n");
	__android_log_print(ANDROID_LOG_VERBOSE, APPNAME, "Androido: Usage: inotifywait [ options ] file1 [ file2 ] [ file3 ] "
	       "[ ... ]\n");
	__android_log_print(ANDROID_LOG_VERBOSE, APPNAME, "Androido: Options:\n");
	__android_log_print(ANDROID_LOG_VERBOSE, APPNAME, "Androido: \t-h|--help     \tShow this help text.\n");
	__android_log_print(ANDROID_LOG_VERBOSE, APPNAME, "Androido: \t@<file>       \tExclude the specified file from being "
	       "watched.\n");
	__android_log_print(ANDROID_LOG_VERBOSE, APPNAME, "Androido: \t--exclude <pattern>\n"
	       "\t              \tExclude all events on files matching the\n"
	       "\t              \textended regular expression <pattern>.\n");
	__android_log_print(ANDROID_LOG_VERBOSE, APPNAME, "Androido: \t--excludei <pattern>\n"
	       "\t              \tLike --exclude but case insensitive.\n");
	__android_log_print(ANDROID_LOG_VERBOSE, APPNAME, "Androido: \t-m|--monitor  \tKeep listening for events forever.  Without\n"
	       "\t              \tthis option, inotifywait will exit after one\n"
	       "\t              \tevent is received.\n");
	__android_log_print(ANDROID_LOG_VERBOSE, APPNAME, "Androido: \t-d|--daemon   \tSame as --monitor, except run in the background\n"
               "\t              \tlogging events to a file specified by --outfile.\n"
               "\t              \tImplies --syslog.\n");
	__android_log_print(ANDROID_LOG_VERBOSE, APPNAME, "Androido: \t-r|--recursive\tWatch directories recursively.\n");
	__android_log_print(ANDROID_LOG_VERBOSE, APPNAME, "Androido: \t--fromfile <file>\n"
	       "\t              \tRead files to watch from <file> or `-' for "
	       "stdin.\n");
	__android_log_print(ANDROID_LOG_VERBOSE, APPNAME, "Androido: \t-o|--outfile <file>\n"
	       "\t              \tPrint events to <file> rather than stdout.\n");
	__android_log_print(ANDROID_LOG_VERBOSE, APPNAME, "Androido: \t-s|--syslog   \tSend errors to syslog rather than stderr.\n");
	printf("\t-q|--quiet    \tPrint less (only print events).\n");
	__android_log_print(ANDROID_LOG_VERBOSE, APPNAME, "Androido: \t-qq           \tPrint nothing (not even events).\n");
	__android_log_print(ANDROID_LOG_VERBOSE, APPNAME, "Androido: \t--format <fmt>\tPrint using a specified printf-like format\n"
	       "\t              \tstring; read the man page for more details.\n");
	__android_log_print(ANDROID_LOG_VERBOSE, APPNAME, "Androido: \t--timefmt <fmt>\tstrftime-compatible format string for use with\n"
	       "\t              \t%%T in --format string.\n");
	__android_log_print(ANDROID_LOG_VERBOSE, APPNAME, "Androido: \t-c|--csv      \tPrint events in CSV format.\n");
	__android_log_print(ANDROID_LOG_VERBOSE, APPNAME, "Androido: \t-t|--timeout <seconds>\n"
	       "\t              \tWhen listening for a single event, time out "
	       "after\n"
	       "\t              \twaiting for an event for <seconds> seconds.\n"
	       "\t              \tIf <seconds> is 0, inotifywait will never time "
	       "out.\n");
	__android_log_print(ANDROID_LOG_VERBOSE, APPNAME, "Androido: \t-e|--event <event1> [ -e|--event <event2> ... ]\n"
	       "\t\tListen for specific event(s).  If omitted, all events are \n"
	       "\t\tlistened for.\n\n");
	__android_log_print(ANDROID_LOG_VERBOSE, APPNAME, "Androido: Exit status:\n");
	__android_log_print(ANDROID_LOG_VERBOSE, APPNAME, "Androido: \t%d  -  An event you asked to watch for was received.\n",
	       EXIT_SUCCESS );
	__android_log_print(ANDROID_LOG_VERBOSE, APPNAME, "Androido: \t%d  -  An event you did not ask to watch for was received\n",
	       EXIT_FAILURE);
	__android_log_print(ANDROID_LOG_VERBOSE, APPNAME, "Androido: \t      (usually delete_self or unmount), or some error "
	       "occurred.\n");
	__android_log_print(ANDROID_LOG_VERBOSE, APPNAME, "Androido: \t%d  -  The --timeout option was given and no events occurred\n",
	       EXIT_TIMEOUT);
	__android_log_print(ANDROID_LOG_VERBOSE, APPNAME, "Androido: \t      in the specified interval of time.\n\n");
	__android_log_print(ANDROID_LOG_VERBOSE, APPNAME, "Androido: Events:\n");
	print_event_descriptions();
}

