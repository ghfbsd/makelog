/************************************************************************/
/*  mssplit - split an MiniSEED data stream into discrete files 	*/
/*									*/
/*	George Helffrich 						*/
/*	University of Bristol Earth Sciences				*/
/*	13 Jan. 2006							*/
/*									*/
/************************************************************************/

#include <stdio.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <sys/errno.h>
#define VERSION "0.01"
#define	FATAL(str) { fprintf (stderr, str); exit(1); }

#define DEFAULT_BUF 512

char *syntax[] = {
"%s version " VERSION,
"%s  [ -d DIR ] [ -b size ] [ -v ]",
"    where:",
"	-h	Help - prints syntax message.",
"	-v      Verbose output",
"	-d DIR  Directory where output streams will be placed; default .",
"	-b size Size of mseed blocks; default 512",
NULL };

char *cmdname;
FILE *info;

void print_syntax(char *c, char *text[]) {
    int i = 0;
    while (text[i]) {
	fprintf(stderr,text[i],c); fputs("\n", stderr);
	i++;
    }
}

/************************************************************************/
/*  main routine							*/
/************************************************************************/
main (int argc, char **argv)
{
    FILE *input, *output = NULL;
    
    char *infile, *ptr, *dir = ".", *cur = NULL;
    char *buf = (char *)malloc(DEFAULT_BUF);
    int status = 0, overb = 0;
    struct {
       char station[5];
       char channel[3];
       char network[2];
       char location[2];
    } stn;
    char sid[5+1+3+1+2+1+2+1];
    char cid[5+1+3+1+2+1+2+1];

    /* Variables needed for getopt. */
    extern char	*optarg;
    extern int	optind, opterr;
    int		c;
    long	i, bufl=DEFAULT_BUF;

    info = stdout;
    cmdname = argv[0];
    /*	Parse command line options.					*/
    while ( (c = getopt(argc,argv,"hvd:b:")) != -1)
	switch (c) {
	case '?':
	case 'h':   print_syntax (cmdname, syntax); exit(0); break;
	case 'v':   overb = 1; break;
	case 'd':   dir = optarg; break;
	case 'b':
	    i = strtol(optarg, &ptr, 10); if (ptr) FATAL("bad -b value\n");
	    buf = (char *)realloc(buf, i); break;
	default:    FATAL("error parsing options\n");
	}
    /*	Skip over all options and their arguments.			*/
    argv = &(argv[optind]);
    argc -= optind;

    /* The remaining arguments are [ input ] files.			*/

    infile = "<stdin>";
    input = stdin;
    switch (argc) {
	case 0:	
	    info = stderr;
	    break;
	case 1:
	    info = stderr;
	    infile = argv[0];
	    if ((input = fopen (infile, "r")) == NULL)
		FATAL ("unable to open input file\n");
	    break;
	default:
	    print_syntax (cmdname, syntax); exit(1); break;
    }

    /* Read input stream.						*/

    cid[0] = '\0'; while (fread(buf, bufl, 1, input) > 0) {
	/* New buffer; decode MSEED header */
	char *p, *cur = NULL;

	switch (buf[6]) {
	    case 'D': case 'R': case 'Q': case 'M': break;
	    default:
		fprintf (stderr, "%s: File %s is not MSEED\n", cmdname, infile);
		exit(1);
	}
	memcpy (stn.station, buf+8, 5);
	memcpy (stn.channel, buf+15, 3);
	memcpy (stn.network, buf+18, 2);
	memcpy (stn.location, buf+13, 2);
	
#define NBLEN(v) (p=strstr(v," "), p?(p-v>sizeof(v)?sizeof(v):p-v):sizeof(v))
	sprintf(sid,"%-0.*s.%-0.*s.%-0.*s.%-0.*s",
	    NBLEN(stn.station),stn.station,
	    NBLEN(stn.network),stn.network,
	    NBLEN(stn.location),stn.location,
	    NBLEN(stn.channel),stn.channel);
	if (strcmp(sid,cid)) { /* New stream */
	    if (overb) printf("%s\n",sid);
	    if (cur) free(cur);
	    if (cur = (char *)malloc(2+strlen(dir)+strlen(cid)), !cur)
		FATAL("No memory");
	    sprintf(cur, "%s/%s",dir,sid);
	    if (output) fclose(output);
	    output = fopen(cur, "a+");
	    if (output == NULL) {
		if (errno == ENOTDIR)
		    fprintf(stderr,"%s: Bad -d directory: %s", cmdname, dir);
		else
		    fprintf(stderr,"%s: Can't open file %s",cmdname, p);
		exit(1);
	    }
	    strcpy(cid,sid);
	}
	if (1 != fwrite(buf, bufl, 1, output)) {
	    fprintf(stderr,"%s: Error writing %s", cmdname, cur);
	    exit(1);
	}
    }
	
    return (status);
}
