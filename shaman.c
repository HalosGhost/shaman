/*****************************************\
* Check weather information from the NOAA *
* License: Sam Stuewe (C) 2013 GPLv2      *
\*****************************************/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <ctype.h>

FILE *url;
char provider[54]="http://forecast.weather.gov/zipcity.php";
char command[256],locurl[256],line[2000],coordln[64],reportln[96],elevln[80],currentln[2000];
char *c=NULL,*ptr=NULL,*match=NULL;
char reporter[48],condition[20],*degunts="F",*visunts="mi",*presunts="in",wgspd[5],wsspd[5],wtype[24],*wndir="",*wunts="mph";
float lat,lon,temperature,visibility,pressure;
int elev,humidity,dewpoint,wdir,wspd;
int len,i,chall,chcond,chtemp,chcoor,chrepo,chhum,chvis,chpres,chwnd,degscl=0,count=0;

/* Rudimentary Error Handling */
void usage(char *progname) {
	fprintf(stderr,"Usage: %s [options] location\n\n",progname);
	fprintf(stderr,"Options:\n");
	fprintf(stderr,"  -a   print all available information.\n");
	fprintf(stderr,"  -c   print current weather conditions.\n");
	fprintf(stderr,"  -d   print humidity and dew-point.\n");
	fprintf(stderr,"  -h   print this help message.\n");
	fprintf(stderr,"  -m   print with metric units.\n");
	fprintf(stderr,"  -p   print pressure.\n");
	fprintf(stderr,"  -r   print reporter information.\n");
	fprintf(stderr,"  -t   print temperature.\n");
	fprintf(stderr,"  -v   print visibility.\n");
	fprintf(stderr,"  -w   print wind information.\n");
	fprintf(stderr,"\nIf no options are passed, -a is assumed.\n");
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
	sscanf(conditionsln,
		   "%*[^<]<value>%f%*[^-]-%*[^v]value>%d%*[^-]-%*[^v]value>%d%*[^=]=%*[^=]=\"%[^\"]\"%*[^\"]\"%*[^>]>\
		   %f%*[^.]%*[^=]%*[^v]value>%d%*[^\"]%*[^v]value>%[^<]%*[^=]%*[^v]value>%[^<]%*[^-]%*[^=]%*[^v]value>%f",
		   &temperature,&dewpoint,&humidity,condition,&visibility,&wdir,wgspd,wsspd,&pressure);
	if (degscl) { 
		temperature=(temperature-32)/1.8; 
		dewpoint=(dewpoint-32)/1.8;
		visibility*=1.6; visunts="km";
		pressure*=33.863753; presunts="mb";
	}
	if (chrepo||chall) printf("\n");
	if (!chtemp&&chcond) printf("%s\n",condition);
	if ((chcond&&chtemp)||chall) printf("%s (%.3g°%s)\n",condition,temperature,degunts);
	if (chtemp&&!chcond) printf("%.3g°%s\n",temperature,degunts);
	if (chwnd||chall) {
		if (!isdigit(*wgspd)&&!isdigit(*wsspd)) { printf("Wind: Negligible\n"); wspd=0; }
		else sscanf((!isdigit(*wgspd))?wsspd:wgspd,"%d",&wspd);
		if (degscl) { wspd*=1.852; wunts="kmph"; }
		else wspd*=1.151;
		switch ( (wdir/45) ) {
			case 0: wndir="N"; break;
			case 1: wndir="NE"; break;
			case 2: wndir="E"; break;
			case 3: wndir="SE"; break;
			case 4: wndir="S"; break;
			case 5: wndir="SW"; break;
			case 6: wndir="W"; break;
			case 7: wndir="NW"; break;
			case 8: wndir="N"; break;
			default: wndir="Vrbl"; break;
		}
		if (wspd) printf("Wind: %s %s %d %s\n",(!isdigit(*wgspd)?"Sustained":"Gusts"),wndir,wspd,wunts);
	}
	if (chhum||chall) printf("Humidity: %d%%\nDew Point: %d°%s\n",humidity,dewpoint,degunts);
	if (chvis||chall) printf("Visbility: %.4g %s\n",visibility,visunts);
	if (chpres||chall) printf("Pressure: %.4g %s\n",pressure,presunts);
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
		if (chrepo||chall) getReporter(reportln,coordln,elevln);
		if (chcond||chhum||chpres||chtemp||chvis||chwnd||chall) getConditions(currentln);
	}
}

int main(int argc,char** argv) {
	if ( !argv[1] ) usage(argv[0]);
	if ( argv[1]&&argv[1][0]!='-' ) chall=1;
	for ( i=1; i<argc; i++ ) {
		if ( argv[i][0]=='-') {
			switch ( argv[i][1]=='-' ? argv[i][2] : argv[i][1] ) {
				case 'h': usage(argv[0]); break;
			    case 'a': chall=1; break;
			    case 'c': chcond=1; break;
				case 'd': chhum=1; break;
			    case 'm': degscl=1; degunts="C"; break;
			    case 'p': chpres=1; break;
			    case 'r': chrepo=1; break;
			    case 't': chtemp=1; break;
			    case 'v': chvis=1; break;
			    case 'w': chwnd=1; break;
			    default: invalidOption(argv[i]); break;
			}
		}
		else checkStones(argv[i]);
	}
	return 0;
}
