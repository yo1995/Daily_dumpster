#include <stdlib.h>
#include <stdio.h>
#include <getopt.h>
#include <math.h>

#include "zmath.h"


void Matrix_Load(float *m,int y,int x,char *filename)
{
	 FILE *f;
	 int i,j;
	 
	 f=fopen(filename,"r");
	 if (f==NULL) {
			perror("matrix");
			exit(1);
	 }
	 for(i=0;i<y;i++)
		 for(j=0;j<x;j++) {
				fscanf(f,"%f",&m[i*x+j]);
		 }
	 fclose(f);
}

void Matrix_Save(float *m,int y,int x,char *filename)
{
	 FILE *f;
	 int i,j;
	 
	 f=fopen(filename,"w");
	 if (f==NULL) {
			perror("matrix");
			exit(1);
	 }
	 for(i=0;i<y;i++) {
			for(j=0;j<x;j++) {
				 fprintf(f,"%f ",m[i*x+j]);
			}
			fprintf(f,"\n");
	 }
	 
	 fclose(f);
}
	 

void M3_Print(M3 m) 
{
	 int i,j;
	 for(i=0;i<3;i++) {
			for(j=0;j<3;j++) printf("%10f",m.m[i][j]);
			printf("\n");
	 }
}

/* calcul des matrices de rectification a partir des matrices des cameras */


/* calcul de la surface  */

#define THETA_NB 128 

void RectifyCalc(M3 *rect1,M3 *rect2,M34 p1,M34 p2,
								 int img_size_x,int img_size_y)
{
	 M3 p_m[2],p_inv[2],rot,rot1,rect_in,rect_test[2];
	 V3 c[2],p_t[2],v1,v2;
	 V3 vrot[3];
	 int i,j,k,l;
	 float theta,theta_min,dist_min,dist;
	 float xmin[2],xmax[2],ymin,ymax,mx,my;
		 
	 /* on cherche d'abord les centres optiques */
	 for(i=0;i<3;i++)
		 for(j=0;j<3;j++) {
				p_m[0].m[i][j]=p1.m[i][j];
				p_m[1].m[i][j]=p2.m[i][j];
		 }
	 for(i=0;i<3;i++) {
			p_t[0].v[i]=p1.m[i][3];
			p_t[1].v[i]=p2.m[i][3];
	 }
	 
	 for(i=0;i<2;i++) {
			p_inv[i]=M3_Inv(p_m[i]);
			c[i]=V3_Neg( V3_MulM3V3(p_inv[i],p_t[i]) );
	 }

	 printf("c1.x=%f c1.y=%f c1.z=%f\n",c[0].X,c[0].Y,c[0].Z);
	 printf("c2.x=%f c2.y=%f c2.z=%f\n",c[1].X,c[1].Y,c[1].Z);

	 /* On construit la matrice de passage de rotation */

	 vrot[0]=V3_Sub(c[1],c[0]);
	 if ( V3_Norm(&vrot[0]) ) {
			fprintf(stderr,"Les 2 centres optiques ne doivent pas être confondus\n");
			exit(1);
	 }
	 
	 vrot[1]=V3_Cross(vrot[0],V3_New(1,0,0));
	 if ( V3_Norm(&vrot[1]) ) {
			vrot[1]=V3_Cross(vrot[0],V3_New(0,1,0));
			V3_Norm(&vrot[1]);
	 }
	 vrot[2]=V3_Cross(vrot[0],vrot[1]);
	 
	 for(i=0;i<3;i++)
		 for(j=0;j<3;j++) 
		 rot.m[i][j]=vrot[i].v[j];

	 printf("rot=");
	 M3_Print(rot);
	 
	 /* On teste toutes les rotations possibles en recherchant la 
		* meilleure transformation
		*/
	 dist_min=1E10;
	 theta_min=0;

	 for(i=0;i<THETA_NB;i++) {
			theta=i*(2*M_PI/THETA_NB);
			
			rot1=M3_Rotate(theta,0);
			ymin=xmin[0]=xmin[1]=1E10;
			ymax=xmax[0]=xmax[1]=-1E10;

			for(j=0;j<2;j++) {
				 rect_test[j]=M3_Mul(M3_Mul(rot1,rot),p_inv[j]);

				 for(k=0;k<4;k++) {
						if (k&1) v1.X=img_size_x-1; else v1.X=0;
						if (k&2) v1.Y=img_size_y-1; else v1.Y=0;
						v1.Z=1;
						v1=V3_MulM3V3(rect_test[j],v1);
						if (v1.Z==0) {
							 fprintf(stderr,"Erreur pour la projection d'un angle d'une image\n");
							 exit(1);
						}
						v1.X/=v1.Z;
						v1.Y/=v1.Z;
						if (v1.X<xmin[j]) xmin[j]=v1.X;
						if (v1.X>xmax[j]) xmax[j]=v1.X; 

/* PATCH !!! */
/*						if (v1.X<xmin[0]) { xmin[1]=v1.X; xmin[0]=v1.X; }
						if (v1.X>xmax[0]) { xmax[1]=v1.X; xmax[0]=v1.X; } */

						if (v1.Y<ymin) ymin=v1.Y;
						if (v1.Y>ymax) ymax=v1.Y;
				 }
			}
			
			dist=0;
			for(j=0;j<2;j++) {
			
				 rect_in=M3_Identity();
				 mx= img_size_x / (xmax[j]-xmin[j]); 
				 my= img_size_y / (ymax-ymin);
				 rect_in.m[0][0]= mx; 
				 rect_in.m[1][1]= my;
				 rect_in.m[0][2]=-xmin[j] * mx;
				 rect_in.m[1][2]=-ymin * my;
				 rect_test[j]=M3_Mul(rect_in,rect_test[j]);
				 
				 for(k=0;k<4;k++) {
						if (k&1) v2.X=img_size_x-1; else v2.X=0;
						if (k&2) v2.Y=img_size_y-1; else v2.Y=0;
						v2.Z=0;
						
						v1.X=v2.X;
						v1.Y=v2.Y;
						v1.Z=1;
						v1=V3_MulM3V3(rect_test[j],v1);
				 
						v1.X/=v1.Z;
						v1.Y/=v1.Z;

						v2=V3_Sub(v1,v2);
						dist+=v2.X*v2.X+v2.Y*v2.Y;
				 }
			}
/*			printf("theta=%f dist=%f\n",theta,dist); */

			if (dist<dist_min) {
				 dist_min=dist;
				 theta_min=theta;

				 for(j=0;j<2;j++) {
						mx=rect_test[j].m[0][0];
						for(k=0;k<3;k++)
							for(l=0;l<3;l++)
							rect_test[j].m[k][l]/=mx;
				 }
				 
				 *rect1=rect_test[0]; 
				 *rect2=rect_test[1];
			}
			
	 }
	 printf("theta_min=%f dist_min=%f\n"
					"xmin1=%f xmax1=%f xmin2=%f xmax2=%f ymin=%f ymax=%f\n",
					theta_min,dist_min,
					xmin[0],xmax[0],xmin[1],xmax[1],ymin,ymax);
	 printf("r1=\n");
	 M3_Print(*rect1);
	 printf("r2=\n");
	 M3_Print(*rect2);
}



/*
 * Un petit essai de calibration
 * 
 * On utilise une méthode linéaire et on suppose que le rang de la matrice
 * A du cours vaut 11
 */

/*
 * Inversion d'une matrice n x n
 */

int Matrix_Inv(float *r,float *m,int n)
{
	 int i,j,k,l;
	 float max,tmp,t;

	 /* identitée dans r */
	 for(i=0;i<n*n;i++) r[i]=0;
	 for(i=0;i<n;i++) r[i*n+i]=1;
	 
	 for(j=0;j<n;j++) {
			
			/* recherche du nombre de plus grand module sur la colonne j */
			max=m[j*n+j];
			k=j;
			for(i=j+1;i<n;i++)
				if (fabs(m[i*n+j])>fabs(max)) {
					 k=i;
					 max=m[i*n+j];
				}
			printf("max = %f ligne=%d\n",max,k);
			
			if (max==0) {
				 fprintf(stderr,"Matrice non inversible\n");
				 return 1;
			}
			
			/* permutation des lignes j et k */
			if (k!=j) {
				 for(i=0;i<n;i++) {
						tmp=m[j*n+i];
						m[j*n+i]=m[k*n+i];
						m[k*n+i]=tmp;
						
						tmp=r[j*n+i];
						r[j*n+i]=r[k*n+i];
						r[k*n+i]=tmp;
				 }
			}
			
			/* multiplication de la ligne j par 1/max */
			max=1/max;
			for(i=0;i<n;i++) {
				 m[j*n+i]*=max;
				 r[j*n+i]*=max;
			}
			
			
			for(l=0;l<n;l++) if (l!=j) {
				 t=m[l*n+j];
				 for(i=0;i<n;i++) {
						m[l*n+i]-=m[j*n+i]*t;
						r[l*n+i]-=r[j*n+i]*t;
				 }
			}
	 }
/*	 
	 for(j=n-1;j>=1;j--) {
			for(i=0;i<j;i++) {
				 t=m[i*n+j];
				 for(k=0;k<n;k++) {
						m[i*n+k]-=m[j*n+k]*t;
						r[i*n+k]-=r[j*n+k]*t;
				 }
			}
	 }
 */
	 return 0;
}

/*
 * Multiplication de 2 matrices, résultat dans c
 * Tailles:
 * c= y x
 * a= y z
 * b= z x
 */
void Matrix_Mul(float *c,float *a,float *b,int y,int x,int z)
{
	 int i,j,k;
	 float s;
	 
	 for(i=0;i<y;i++) {
			for(j=0;j<x;j++) {
				 s=0;
				 for(k=0;k<z;k++) s+=a[i*z+k]*b[k*x+j];
				 c[i*x+j]=s;
			}
	 }
}

void Matrix_Print(float *m,int y,int x)
{
	 int i,j;
	 printf("[");
	 for(i=0;i<y;i++) {
			printf("[");
			for(j=0;j<x;j++) {
				 printf("%g",m[i*x+j]);
				 if (j!=(x-1)) printf(",");
			}
			if (i!=(y-1)) printf("],\n"); else printf("]]\n");
	 }
}

#define N 9

void Matrix_Test(void)
{
	 float a[N*N],b[N*N],c[N*N];
	 int i,j;
	 
	 for(i=0;i<N;i++)
		 for(j=0;j<N;j++) {
				a[i*N+j]=random() % 100;
				c[i*N+j]=a[i*N+j];
		 }
	 

	 printf("a=\n");
	 Matrix_Print(a,N,N);
	 
	 
	 Matrix_Inv(b,a,N);
	 Matrix_Mul(a,c,b,N,N,N); 
	 
	 printf("res=\n");
	 Matrix_Print(a,N,N);

	 
}


#define NBPTS 6

void Calibrate(float *proj,float (*pts)[5])
{
	 float A[12][12],A2[11],A3[11],A4[11][11];
	 float B[11][11],Q[12];
	 float u,v,a;
	 int i,j;
	 
	 /* calcul de la matrice A */
	 
	 for(i=0;i<NBPTS;i++) {

			u=pts[i][3];
			v=pts[i][4];

			/* u */

			A[2*i][0]=1;
			A[2*i][1]=pts[i][0];
			A[2*i][2]=pts[i][1];
			A[2*i][3]=pts[i][2];
			
			A[2*i][4]=0;
			A[2*i][5]=0;
			A[2*i][6]=0;
			A[2*i][7]=0;

			A[2*i][8]=-u;
			A[2*i][9]=-u*pts[i][0];
			A[2*i][10]=-u*pts[i][1];
			A[2*i][11]=-u*pts[i][2];

			/* v */

			A[2*i+1][0]=0;
			A[2*i+1][1]=0;
			A[2*i+1][2]=0;
			A[2*i+1][3]=0;

			A[2*i+1][4]=1;
			A[2*i+1][5]=pts[i][0];
			A[2*i+1][6]=pts[i][1];
			A[2*i+1][7]=pts[i][2];
			
			A[2*i+1][8]=-v;
			A[2*i+1][9]=-v*pts[i][0];
			A[2*i+1][10]=-v*pts[i][1];
			A[2*i+1][11]=-v*pts[i][2];
	 }
	 
	 /* recherche d'un vecteur non nul dans Ker A 
		* On utilise le fait que A[0][0] != 0
		*/

	 for(i=0;i<11;i++) {
			for(j=0;j<11;j++) A4[i][j]=A[1+i][1+j];
			A3[i]=A[1+i][0];
			A2[i]=A[0][1+i];
	 }

	 Matrix_Inv((float *)B,(float *)A4,11);

	 Matrix_Mul(&Q[1],(float *)B,A3,11,1,11);
	 Q[0]=-1;

	 /* on veut: norm(q3)=1 */
	 
	 a=sqrt(Q[9]*Q[9]+Q[10]*Q[10]+Q[11]*Q[11]);
	 for(i=0;i<12;i++) Q[i]/=a;
	 
	 /* on rempli la matrice de projection */
	 for(i=0;i<3;i++) {
			for(j=0;j<3;j++) proj[4*i+j]=Q[4*i+j+1];
			proj[4*i+3]=Q[4*i];
	 }
	 
	 printf("Q=\n");
	 Matrix_Print(Q,1,11);
	 printf("proj=\n");
	 Matrix_Print(proj,1,11);
	 
}











void print_help(void)
{
	 printf("usage: rectify [-x size_x] [-y size_y] rect proj [points]\n"
					"Version 1.00 By Fabrice Bellard\n"
					"\n"
					);
}

int main(int argc,char *argv[])
{
	 M3 rect[2];
	 M34 proj[2];
	 int c,i;
	 char buf[256];
	 int size_x,size_y;
	 float pts[NBPTS][5];
	 int pts_given;
	 
/*
	 Matrix_Test();
	 return 0;
*/	 
	
	 size_x=512;
	 size_y=512;
	 pts_given=0;
	 
	 while (1) {
			c=getopt(argc,argv,"h?x:y:");
			if (c==-1) break;
			switch (c) {
			 case 'h':
			 case '?':
				 print_help();
				 exit(0);
				 break;
			 case 'x':
				 size_x=atoi(optarg);
				 break;
			 case 'y':
				 size_y=atoi(optarg);
				 break;
			}
	 }

	 if (optind==argc) {
			print_help();
			exit(0);
	 }

	 /* est-ce qu'on demande la calibration de la camera ? */
	 if (optind+2<argc) {
			
			for(i=0;i<2;i++) {
				 /* on charge les 6 points (X Y Z U V) */
				 sprintf(buf,"%s%d",argv[optind+2],i+1);
				 printf("Loading points %s\n",buf);
				 Matrix_Load((float *)pts,6,5,buf);

				 Calibrate((float *)proj[i].m,pts);
			}
			pts_given=1;
	 }
	 

	 /* on sauve ou charge les matrices de projection */
	 for(i=0;i<2;i++) {
			sprintf(buf,"%s%d",argv[optind+1],i+1);
			if (pts_given) {
				 printf("Saving %s\n",buf);
				 Matrix_Save((float *)proj[i].m,3,4,buf);
			} else {
				 printf("Loading %s\n",buf);
				 Matrix_Load((float *)proj[i].m,3,4,buf);
			}
	 }
	 
	 /* rectification */

	 RectifyCalc(&rect[0],&rect[1],proj[0],proj[1],size_x-1,size_y-1);

	 for(i=0;i<2;i++) {
			sprintf(buf,"%s%d",argv[optind],i+1);
			printf("Saving %s\n",buf);
			Matrix_Save((float *)rect[i].m,3,3,buf);
	 }

	 return 0;
}
