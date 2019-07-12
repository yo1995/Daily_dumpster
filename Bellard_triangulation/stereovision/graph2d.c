/*
 * Fast functions to draw 2d graphics in 8 bits framebuffers
 * Without any clipping !!!!
 * (c) 1996 Fabrice Bellard
 */

#include "graph.h"

static unsigned char *pbuf;
static int size_x,size_y;


int G2D_Init(unsigned char *b,int sx,int sy)
{
	 pbuf=b;
	 size_x=sx;
	 size_y=sy;
	 return 0;
}


/* 
 * Changement de taille d'une image, sans antialiasing
 * stockage dans img1
 */

void G2D_Resize(unsigned char *img1,int size1_x,int size1_y,
								unsigned char *img2,int size2_x,int size2_y)
{
	 int x,y,x2,y2;
	 int inc_x,inc_y;
	 
	 inc_x=(size2_x * (1<<16)) / size1_x;
	 inc_y=(size2_y * (1<<16)) / size1_y;

	 y2=0;
	 for(y=0;y<size1_y;y++) {
			x2=0;
			for(x=0;x<size1_x;x++) {
				 img1[y*size1_x+x]=img2[(y2>>16)*size2_x+(x2>>16)];
				 x2+=inc_x;
			}
			y2+=inc_y;
	 }
}


void G2D_Plot(int x,int y,int color)
{
	 pbuf[x+y*size_x]=color;
}


void G2D_Line(int x1,int y1,int x2,int y2,int color)
{
	 register int a,dx,dy,n;
	 register unsigned char *pp;
	 
	 if (y1>y2 || (y1==y2 && x1>x2)) {
			a=x1; x1=x2; x2=a;
			a=y1; y1=y2; y2=a;
	 }
	 a=y1*size_x+x1;
	 pp=pbuf+a;
	 dx=x2-x1;
	 dy=y2-y1;
	 
/* macro pour le dessin effectif de la ligne */	 

#define DRAWLINE(dx,dy,inc_1,inc_2) \
	 n=dx;\
	 a=2*dy-dx;\
	 dy=2*dy;\
	 dx=2*dx-dy;\
	 do {\
			*pp=color; \
			if (a>0) { pp+=(inc_1); a-=dx; }\
			else { pp+=(inc_2); a+=dy; }\
	 } while (--n >= 0);

/* fin macro */	 
	 
	 if (dx==0 && dy==0) {
			*pp=color;
	 } else if (dx>0) {
			if (dx>=dy) {
				 DRAWLINE(dx,dy,size_x+1,1);
			} else {
				 DRAWLINE(dy,dx,size_x+1,size_x);
			}
	 } else {
			dx=-dx;
			if (dx>=dy) {
				 DRAWLINE(dx,dy,size_x-1,-1);
			} else {
				 DRAWLINE(dy,dx,size_x-1,size_x);
			}
	 }


#undef DRAWLINE

}

void G2D_LineFloat(float x1,float y1,float x2,float y2,int color)
{
	 G2D_Line((int) (x1 * (size_x-1)),
						(int) (y1 * (size_y-1)),
						(int) (x2 * (size_x-1)),
						(int) (y2 * (size_y-1)),
						color);
}



inline int Clip(float denom,float num,float *tmin,float *tmax)
{
	 float t;
	 
	 if (denom>0) {
			t=num/denom;
			if (t>*tmax) return 0;
			if (t>*tmin) *tmin=t;
	 } else if (denom<0) {
			t=num/denom;
			if (t<*tmin) return 0;
			if (t<*tmax) *tmax=t;
	 } else if (num>0) return 0;
	 return 1;
}

void G2D_LineClip(float x1,float y1,float x2,float y2,int color)
{
	 float dx,dy,tmin,tmax;
	 
	 int c1= (x1<0.0) |
		       ((x1>1.0) << 1) |
		       ((y1<0.0) << 2) |
		       ((y1>1.0) << 3);
	 
	 int c2= (x2<0.0) |
		       ((x2>1.0) << 1) |
		       ((y2<0.0) << 2) |
		       ((y2>1.0) << 3);
	 
	 if ( (c1&c2)!=0 ) { 
			return;
	 } else if ((c1|c2)==0) {
			G2D_LineFloat(x1,y1,x2,y2,color);
	 } else {
			dx=x2-x1;
			dy=y2-y1;
			tmin=0;
			tmax=1;
			if (Clip(dx,-x1,&tmin,&tmax) &&
					Clip(-dx,x1-1,&tmin,&tmax) &&
					Clip(dy,-y1,&tmin,&tmax) &&
					Clip(-dy,y1-1,&tmin,&tmax) ) {
				 G2D_LineFloat(x1+tmin*dx,y1+tmin*dy,x1+tmax*dx,y1+tmax*dy,color); 
			}
	 }
}

	 
										
										
										
										
	 
	 
