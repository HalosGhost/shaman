/*****************************************\
* Check weather information from the NOAA *
* License: Sam Stuewe (C) 2013 GPLv2      *
\*****************************************/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

FILE *url;
char command[256],locurl[256],line[2000],reporter[48],condition[20];
char provider[54]="http://forecast.weather.gov/zipcity.php";
char *c=NULL,*wind=NULL,*ptr=NULL,*match=NULL;
char coordln[64],reportln[96],elevln[80],currentln[2000],*degunts="F";
char *lines[4];
float lat,lon;
int elev,temperature,humidity,pressure,dewpoint,visibility,hindex;
int len,i,chcond,chtemp,chcoor,chrepo,extend,degscl=0,count=0;

/* Rudimentary Error Handling */
void usage(char *progname) {
	fprintf(stderr,"Usage: %s [options] location\n\n",progname);
	fprintf(stderr,"If no options are passed, nothing will be printed.\n");
	fprintf(stderr,"  -a   print all available information.\n");
	fprintf(stderr,"  -c   print current weather conditions.\n");
	fprintf(stderr,"  -e   print extended temperature information.\n");
	fprintf(stderr,"  -h   print this help message.\n");
	//fprintf(stderr,"  -m   use metric units.\n");
	fprintf(stderr,"  -r   print reporter information.\n");
	fprintf(stderr,"  -t   print temperature.\n");
	exit(44);
}

void invalidOption(char *option) {
	printf("Unrecognized option (%s)\n",option);
	exit(45);
}

/* Get Functions */
void getReporter(char *reporterln,char *coordinateln,char *elevationln) {
	sscanf(reporterln,"%*[^>]>%[^<]",reporter);
	sscanf(coordinateln,"%*[^\"]\"%f\"%*[^\"]\"%f\"",&lat,&lon);
	sscanf(elevationln,"%*[^>]>%d",&elev);
	printf("%s\nLat: %.5g, Lon: %.5g, Elev: %d\n",reporter,lat,lon,elev);
}

void getConditions(char *conditionsln) {
	sscanf(conditionsln,"%*[^<]<value>%d%*[^-]-%*[^v]value>%d%*[^-]-%*[^v]value>%d%*[^=]=%*[^=]=\"%[^\"]",&temperature,&dewpoint,&humidity,condition);
	if (chrepo) printf("\n");
	if (chcond) printf("%s",condition);
	if (chcond&&chtemp) printf(" (%d°%s)",temperature,degunts);
	if (chtemp&&!chcond) printf("%d°%s",temperature,degunts);
	if (extend) printf("\nHumidity: %d%%\nDew Point: %d°%s",humidity,dewpoint,degunts);
	printf("\n");
}

void checkStones(char *location) {
	len=snprintf(command,sizeof(command),"curl -fv \"%s?inputstring=%s\"|&grep Location",provider,location);
	if ( len<=sizeof(command) ) { 
		if ( !(url=popen(command,"r")) ) { fprintf(stderr,"Could not get location\n");exit(1); }
		fscanf(url,"%*[^:]: %s",locurl);pclose(url);
		len=snprintf(command,sizeof(command),"curl -fsL \"%s&FcstType=dwml&unit=%d\"",locurl,degscl);
		if ( len<=sizeof(command) ) {
			if ( !(url=popen(command,"r")) ) { fprintf(stderr,"Could not check weather stones\n");exit(1); }
			while ( fgets(line,sizeof(line),url) ) {
				if ( (match=strstr(line,"point l")) ) { count++; if (count==2) strcpy(coordln,match); }
				else if ( (match=strstr(line,"area-description")) ) strcpy(reportln,match);
				else if ( (match=strstr(line,"height-units")) ) strcpy(elevln,match);
				else if ( (match=strstr(line,"apparent")) ) strcpy(currentln,match);
			}
		}
		if (chrepo) getReporter(reportln,coordln,elevln);
		if (chcond||chtemp||extend) getConditions(currentln);
	}
}

int main(int argc,char** argv) {
	if ( !argv[1]||argv[1][0]!='-' ) usage(argv[0]);
	for ( i=1; i<argc; i++ ) {
		if ( argv[i][0]=='-') {
			switch ( argv[i][1]=='-' ? argv[i][2] : argv[i][1] ) {
				case 'h': usage(argv[0]); break;
			    case 'a': chrepo=1; chcond=1; chtemp=1; extend=1; break;
			    case 'c': chcond=1; break;
			    case 'e': chtemp=1; extend=1; break;
			    case 'r': chrepo=1; break;
			    case 'm': degscl=1; degunts="C"; break;
			    case 't': chtemp=1; break;
			    default: invalidOption(argv[i]); break;
			}
		}
		else checkStones(argv[i]);
	}
	return 0;
}
