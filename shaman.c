/*****************************************\
* Check weather information from the NOAA *
* License: Sam Stuewe (C) 2013 GPLv2      *
\*****************************************/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <ctype.h>
#include <math.h>
#include <curl/curl.h>

FILE *url;
char provider[]="http://forecast.weather.gov/zipcity.php";
char command[256],locurl[256],line[2000],coordln[64],reportln[96],elevln[80],currentln[2000],hzdln[64],
	 defaultLocation[6];
char *c=NULL,ptr[60],*match=NULL,*degunts="F",*visunts="mi",*presunts="in",*wunts="mph",defaultUnits[]="E";
char reporter[80],condition[40],hazard[59],wgspd[5],wsspd[5],wtype[24],*wndir="",passloc[120],passspc[]="%20",
	 passcma[]="%2C",token[60],delims[]=" ";
float lat,lon,temperature,humidity,visibility,pressure,hidx,hiadj;
int elev,dewpoint,wdir,wspd;
int len,i,ind,a,d,chall,chcond,chtemp,chcoor,chrepo,chhum,chdp,hindex,chvis,chpres,chwnd,nohaz,
	degscl=0,count=0,count2=0,ctynm;

/* Rudimentary Error Handling */
void usage(char *progname) {
	fprintf(stderr,"Usage: %s [options] \"location\"\n\n",progname);
	fprintf(stderr,"Options:\n");
	fprintf(stderr,"  -a   print all available information.\n  -c   print current weather conditions.\n");
	fprintf(stderr,"  -d   print humidity.\n  -D   print dew-point.\n  -i   print with Imperial units. (default)\n");
	fprintf(stderr,"  -l   print heat index.\n  -h   print this help message.\n  -m   print with metric units.\n");
	fprintf(stderr,"  -p   print pressure.\n  -r   print reporter information.\n  -t   print temperature.\n");
	fprintf(stderr,"  -v   print visibility.\n  -w   print wind information.\n");
	fprintf(stderr,"  -z   suppress hazardous/severe weather warnings.\n");
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
		   "%*[^<]<value>%f%*[^-]-%*[^v]value>%d%*[^-]-%*[^v]value>%f%*[^=]=%*[^=]=\"%[^\"]\"%*[^\"]\"%*[^>]>\
		   %f%*[^.]%*[^=]%*[^v]value>%d%*[^\"]%*[^v]value>%[^<]%*[^=]%*[^v]value>%[^<]%*[^-]%*[^=]%*[^v]value>%f",
		   &temperature,&dewpoint,&humidity,condition,&visibility,&wdir,wgspd,wsspd,&pressure);
	if ( hindex || chall) {
		hidx=(-42.379)+2.04901523*temperature+10.14333127*humidity-0.22475541*temperature*humidity
			-6.83783E-3*temperature*temperature-5.481717E-2*humidity*humidity
			+1.22874E-3*temperature*temperature*humidity+8.5282E-4*temperature*humidity*humidity
			-1.99E-6*temperature*temperature*humidity*humidity;
		if ( temperature>=80 && temperature<=112 && humidity<=13 ) {
			hiadj=(((13-humidity)/4)*sqrtf((17-abs(temperature-95.))/17));
		}
		else if ( temperature>=80 && temperature<=87 && humidity>=85 ) {
			hiadj=(((humidity-85)/10)*((87-temperature)/5));
		}
		if ( hiadj ) hidx-=hiadj;
	}
	if ( degscl ) { 
		temperature=(temperature-32)/1.8;
		hidx=(hidx-32)/1.8;
		dewpoint=(dewpoint-32)/1.8;
		visibility*=1.6; visunts="km";
		pressure*=33.863753; presunts="mb";
	}
	if ( chrepo || chall ) printf("\n");
	if ( chcond || chall ) printf("Condition: %s\n",condition);
	if ( chtemp || chall ) printf("Temperature: %.3g°%s\n",temperature,degunts);
	if ( hindex || chall ) printf("Heat Index: %.3g°%s\n",hidx,degunts);
	if ( chwnd || chall ) {
		if ( !isdigit(*wgspd) && !isdigit(*wsspd) ) { 
			printf("Wind: Negligible\n"); 
			wspd=0; 
		}
		else sscanf( (!isdigit(*wgspd)) ? wsspd : wgspd,"%d",&wspd);
		if ( degscl ) { 
			wspd*=1.852; 
			wunts="kmph"; 
		}
		else wspd*=1.151;
		switch ( ((wdir>22 ? wdir-22 : wdir)/45) ) {
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
		if ( wspd ) printf("Wind: %s %s %d %s\n",(!isdigit(*wgspd)?"Sustained":"Gusts"),wndir,wspd,wunts);
	}
	if ( chhum || chall ) printf("Humidity: %.2g%%\n",humidity);
	if ( chdp || chall ) printf("Dew Point: %d°%s\n",dewpoint,degunts);
	if ( chvis || chall ) printf("Visibility: %.4g %s\n",visibility,visunts);
	if ( chpres || chall ) printf("Pressure: %.4g %s\n",pressure,presunts);
}

void getHazards(char *hazardline) {
	sscanf(hazardline,"%*[^\"]\"%[^\"]",hazard);
	if ( *hazard ) {
		if ( chrepo || chall ) printf("\n");
		printf("ALERT: %s\a\n",hazard);
	}
}

void checkStones(char *location) {
	len=snprintf(command,sizeof(command),"curl -fv \"%s?inputstring=%s\"|&grep Location",provider,location);
	if ( len<=sizeof(command) ) { 
		if ( !(url=popen(command,"r")) ) { 
			fprintf(stderr,"Could not get location\n");
			exit(1); 
		}
		fscanf(url,"%*[^:]: %s",locurl);pclose(url);
		len=snprintf(command,sizeof(command),"curl -fsL \"%s&FcstType=dwml&unit=%d\"",locurl,degscl);
		if ( len<=sizeof(command) ) {
			if ( !(url=popen(command,"r")) ) { 
				fprintf(stderr,"Could not check weather stones\n");
				exit(1); 
			}
			while ( fgets(line,sizeof(line),url) ) {
				if ( (match=strstr(line,"point l")) ) { 
					count++; 
					if ( count==2 ) strncpy(coordln,match,63); 
				}
				else if ( (match=strstr(line,"area-description")) ) strncpy(reportln,match,95);
				else if ( (match=strstr(line,"height-units")) ) strncpy(elevln,match,79);
				else if ( (match=strstr(line,"apparent")) ) strncpy(currentln,match,1999);
				else if ( (match=strstr(line,"headline")) ) { 
					count2++; 
					if ( count2==1 ) strncpy(hzdln,match,60); 
				}
			}
		}
		if ( chrepo || chall ) getReporter(reportln,coordln,elevln);
		if ( !nohaz ) getHazards(hzdln);
		if ( chcond || chhum || chpres || chtemp || hindex || chvis || chwnd || chall) {
			getConditions(currentln);
		}; pclose(url);
	}
}

void discoverConfig(void) {
	FILE *rc=NULL;
	const char *cwd = getenv("PWD");
	if ( !rc ) { 
		chdir(getenv("XDG_CONFIG_HOME")); 
		if ( chdir("shaman")==0 ) rc = fopen("config","r"); 
	}
	if ( !rc ) { 
		chdir(getenv("HOME")); 
		rc = fopen(".shamanrc","r"); 
	}
	chdir(cwd);
	if ( rc ) {
		while ( fgets(line,sizeof(line),rc) ) {
			sscanf(line,"Location%*[^\"]\"%[^\"]",defaultLocation);
			sscanf(line,"Units%*[^\"]%[^\"]",defaultUnits);
		}; fclose(rc);
	}
	if ( defaultUnits[0]=='M' ) { degscl=1; degunts="C"; }
}

int main(int argc,char** argv) {
	defaultLocation[0]=0;
	discoverConfig();
	if ( argc==1 ) {
		if ( !*defaultLocation ) usage(argv[0]);
		else { 
			chall=1; 
			strncpy(passloc,defaultLocation,40); 
		}
	}
	for ( i=1; i<argc; i++ ) {
		if ( argv[i][0]=='-' ) {
			for ( a=1; a<strlen(argv[i]); a++ ) {
				switch ( argv[i][a] ) {
					case 'h': usage(argv[0]); break;
					case 'a': chall=1; break;
					case 'c': chcond=1; break;
					case 'd': chhum=1; break;
				    case 'D': chdp=1; break;
				    case 'i': degscl=0; degunts="F"; break;
				    case 'l': hindex=1; break;
					case 'm': degscl=1; degunts="C"; break;
					case 'p': chpres=1; break;
					case 'r': chrepo=1; break;
					case 't': chtemp=1; break;
					case 'v': chvis=1; break;
					case 'w': chwnd=1; break;
				    case 'z': nohaz=1; break;
					default: invalidOption(argv[i]); break;
				}
			}
		}
	}
	if ( argc>1 ) {
		if ( argv[1][0]!='-' ) chall=1;
		if ( argv[argc-1][0]!='-' ) strncpy(passloc,argv[argc-1],40);
		else strncpy(passloc,defaultLocation,40);
	}
	for ( d=0; d<strlen(passloc); d++ ) {
		if ( !isdigit(passloc[d]) ) { ctynm=1; break; }
	}
	if ( ctynm ) {
		if ( strlen(argv[argc-1])<5 && *defaultLocation ) strncpy(token,defaultLocation,59);
		else strncpy(token,argv[argc-1],50);
		strncpy(ptr,strtok(token,delims),59);
		if ( strstr(ptr,",") ) {
			ptr[strcspn(ptr,",")]=0;
			strncat(ptr,passcma,3);
		}
		strncpy(passloc,ptr,60);
		char *y;
		while ( (y=strtok(NULL,delims)) ) {
			strncat(passloc,passspc,3);
			strncpy(ptr,y,59);
			if ( strstr(ptr,",") ) {
				ptr[strcspn(ptr,",")]=0;
				strncat(ptr,passcma,3);
			}
			strncat(passloc,ptr,60);
		}
	}
	checkStones(passloc);
	return 0;
}
