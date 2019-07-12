#include <stdlib.h>
#include <stdio.h>

#include "graph.h"

/************************************************
 * Loading of a PGM file
 ************************************************/

static inline int ppm_space(int c)  {
	 return (c==' ' || c=='\n' || c=='\r' || c=='\t');
}

static void ppm_get(char *str,FILE *f) {
	 char *s;
	 int c;
	 
	 do  {
			c=fgetc(f);
			if (c=='#')  {
				 do  {
						c=fgetc(f);
				 } while (c!='\n');
				 c=fgetc(f);
			}
	 } while (ppm_space(c));
	 
	 s=str;
	 do  {
			*s++=c;
			c=fgetc(f);
	 } while (!ppm_space(c));
	 *s=0;
}
	 	

unsigned char *PGM_Load(int *xsize1,int *ysize1,char *filename) 
{
	 FILE *f;
	 int xsize,ysize,size;
	 unsigned char *img;
	 char s[80];
	 
	 f=fopen(filename,"r");
	 if (f==NULL) {
			printf("%s not found",filename);
			exit(1);
	 }
	 
	 ppm_get(s,f);
	 if (strcmp(s,"P5")!=0) {
			printf("Bad PGM type\n");
			exit(1);
	 }
	 
	 ppm_get(s,f);
	 xsize=atoi(s);
	 ppm_get(s,f);
	 ysize=atoi(s);
	 ppm_get(s,f);
	 size=xsize*ysize;
	 
	 img=malloc(size);
	 fread(img,1,size,f);
	 fclose(f);

	 *xsize1=xsize;
	 *ysize1=ysize;
	 
	 return img;
}

/*
 * Saving 
 */

void PGM_Save(unsigned char *buf,int xsize,int ysize,char *filename) 
{
	 FILE *f;
	 f=fopen(filename,"w");
	 fprintf(f,"P5\n%d %d\n%d\n",xsize,ysize,255);
	 fwrite(buf,1,xsize*ysize,f);
	 fclose(f);
}

