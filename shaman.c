/*****************************************\
* Check weather information from the NOAA *
* License: Sam Stuewe (C) 2013 GPLv2      *
\*****************************************/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

FILE *url;
char provider[54]="http://forecast.weather.gov/zipcity.php";
char command[256],locurl[256],line[2000],coordln[64],reportln[96],elevln[80],currentln[2000];
char *c=NULL,*ptr=NULL,*match=NULL;
char reporter[48],condition[20],*degunts="F",visunts[20],wgspd[5],wsspd[5],wtype[24],wndir[2],*wunts="mph";
float lat,lon,visibility,pressure;
int elev,temperature,humidity,dewpoint,wdir,wspd;
int len,i,chall,chcond,chtemp,chcoor,chrepo,chhum,chvis,chpres,chwnd,degscl=0,count=0;

/* Rudimentary Error Handling */
void usage(char *progname) {
	fprintf(stderr,"Usage: %s [options] location\n\n",progname);
	fprintf(stderr,"If no options are passed, -a is assumed.\n");
	fprintf(stderr,"  -a   print all available information.\n");
	fprintf(stderr,"  -c   print current weather conditions.\n");
	fprintf(stderr,"  -d   print humidity and dew-point.\n");
	fprintf(stderr,"  -h   print this help message.\n");
	//fprintf(stderr,"  -m   use metric units.\n");
	fprintf(stderr,"  -p   print pressure.\n");
	fprintf(stderr,"  -r   print reporter information.\n");
	fprintf(stderr,"  -t   print temperature.\n");
	fprintf(stderr,"  -v   print visibility.\n");
	//fprintf(stderr,"  -w   print wind information.\n");
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
		   "%*[^<]<value>%d%*[^-]-%*[^v]value>%d%*[^-]-%*[^v]value>%d%*[^=]=%*[^=]=\"%[^\"]\"%*[^\"]\"%[^\"]\">\
		   %f%*[^.]%*[^=]%*[^v]value>%d%*[^\"]%*[^v]value>%[^<]%*[^=]%*[^v]value>%[^<]%*[^-]%*[^=]%*[^v]value>%f",
		   &temperature,&dewpoint,&humidity,condition,visunts,&visibility,&wdir,wgspd,wsspd,&pressure);
	//wspd=1.151*(wgspd[0]=='N' ? wsspd[0]=='N' ? 0 : wsspd : wgspd)
	//wndir calculation
	if (chrepo||chall) printf("\n");
	if (chcond||chall) printf("%s",condition);
	if ((chcond&&chtemp)||chall) printf(" (%d°%s)\n",temperature,degunts);
	if (chtemp&&!chcond) printf("%d°%s\n",temperature,degunts);
	if (chhum||chall) printf("Humidity: %d%%\nDew Point: %d°%s\n",humidity,dewpoint,degunts);
	if (chvis||chall) printf("Visbility: %.4g %s\n",visibility,visunts);
	if (chpres||chall) printf("Pressure: %.4g in\n",pressure);
	//if (chwnd||chall) { if (wspd) printf("Wind: %s %s %d %s",wtype,wndir,wspd,wunts); };
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
			    case 'r': chrepo=1; break;
			    case 'm': degscl=1; degunts="C"; break;
			    case 'p': chpres=1; break;
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
