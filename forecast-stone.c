/********************************************\
* Check weather information from weather.gov *
* License: CCPL Sam Stuewe 2013 (BY-SA)      *
\********************************************/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

FILE *url;
char command[256];
char provider[41]="http://forecast.weather.gov/zipcity.php";
char line[3500];
char *c=NULL,*condition=NULL,*wind=NULL,*reporter=NULL;
int temp,humidity,pressure,dewpoint,visibility,hindex,lat,lon,elev;
int len,i;

void usage(char *progname) {
	fprintf(stderr,"Usage: %s [options] location\n",progname);
	exit(44);
}

void checkStone(char *location) {
	len=snprintf(command,sizeof(command),"curl -fsL \"%s?inputstring=%s\"",provider,location);
	if ( len<=sizeof(command) ) {
		if ( !(url=popen(command,"r")) ) { fprintf(stderr,"Could not check stone\n");exit(1); }
		while ( fgets(line,sizeof(line),url) ) {
			if ( (condition=strstr(line,"myforecast-current")) ) {
				if ( strlen(condition)<20 ) exit(46);
				condition+=20;
				if ( (c=strchr(condition,'<')) ) *c='\0';
				else exit(45);
			}
			if ( condition ) fprintf(stdout,"%s\n",condition);
		}
	}
	else { fprintf(stderr,"Command buffer too short\n");exit(45); }
}

int main(int argc,char** argv) {
	if ( !argv[1] ) usage(argv[0]);
	for ( i=1; i<argc; i++ ) {
		if ( argv[i][0]=='-' ) {
			switch ( argv[i][1] ) {
				case 'h': usage(argv[0]); break;
			    default: fprintf(stderr,"Unrecognized option (%s)\n",argv[i]);exit(1);break;
			}
		}
		else checkStone(argv[i]);
	}
	return 0;
}
