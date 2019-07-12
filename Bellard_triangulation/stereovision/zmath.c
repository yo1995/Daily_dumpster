#include <stdlib.h>
#include <string.h>
#include <math.h>
#include "zmath.h"


/* ******* Gestion des matrices 4x4 ****** */

void IdM4(M4 *a)
{
	int i,j;
	for(i=0;i<4;i++)
	for(j=0;j<4;j++) 
	if (i==j) a->m[i][j]=1.0; else a->m[i][j]=0.0;
}

M4 MulM4(M4 a,M4 b)
{
	 int i,j,k;
	 float s;
	 M4 c;
	 for(i=0;i<4;i++)
		 for(j=0;j<4;j++) {
				s=0.0;
				for(k=0;k<4;k++) s+=a.m[i][k]*b.m[k][j];
				c.m[i][j]=s;
		 }
	 return c;
}

M4 ScaleM4(float sx,float sy,float sz)
{
	 M4 a;
	 IdM4(&a);
	 a.m[0][0]=sx;	
	 a.m[1][1]=sy; 
	 a.m[2][2]=sz;
	 return a;
}

M4 RotateM4(float t,int u)
{
	 float s,c;
	 int v,w;
	 M4 a;
	 if ((v=u+1)>2) v=0;
	 if ((w=v+1)>2) w=0;
	 s=sin(t);
	 c=cos(t);
	 IdM4(&a);
	 a.m[v][v]=c;	a.m[v][w]=-s;
	 a.m[w][v]=s;	a.m[w][w]=c;
	 return a;
}
	
M4 TranslateM4(float x,float y,float z)
{
	 M4 a;
	 IdM4(&a);
	 a.m[0][3]=x;
	 a.m[1][3]=y;
	 a.m[2][3]=z;
	 return a;
}

void MoveM4(M4 *a,M4 *b)
{
	memcpy(a,b,sizeof(M4));
}

void MoveV3(V3 *a,V3 *b)
{
	memcpy(a,b,sizeof(V3));
}


void MulM4V3(V3 *a,M4 *b,V3 *c)
{
	 a->X=b->m[0][0]*c->X+b->m[0][1]*c->Y+b->m[0][2]*c->Z+b->m[0][3];
	 a->Y=b->m[1][0]*c->X+b->m[1][1]*c->Y+b->m[1][2]*c->Z+b->m[1][3];
	 a->Z=b->m[2][0]*c->X+b->m[2][1]*c->Y+b->m[2][2]*c->Z+b->m[2][3];
}

void MulM3V3(V3 *a,M4 *b,V3 *c)
{
	 a->X=b->m[0][0]*c->X+b->m[0][1]*c->Y+b->m[0][2]*c->Z;
	 a->Y=b->m[1][0]*c->X+b->m[1][1]*c->Y+b->m[1][2]*c->Z;
	 a->Z=b->m[2][0]*c->X+b->m[2][1]*c->Y+b->m[2][2]*c->Z;
}

void MulM4V4(V4 *a,M4 *b,V4 *c)
{
	 a->X=b->m[0][0]*c->X+b->m[0][1]*c->Y+b->m[0][2]*c->Z+b->m[0][3]*c->W;
	 a->Y=b->m[1][0]*c->X+b->m[1][1]*c->Y+b->m[1][2]*c->Z+b->m[1][3]*c->W;
	 a->Z=b->m[2][0]*c->X+b->m[2][1]*c->Y+b->m[2][2]*c->Z+b->m[2][3]*c->W;
	 a->W=b->m[3][0]*c->X+b->m[3][1]*c->Y+b->m[3][2]*c->Z+b->m[3][3]*c->W;
}
	
/* inversion d'une matrice orthogonale homogène du type Y=M.X+P */ 
void InvOM4(M4 *a,M4 b)
{
	int i,j;
	float s;
	for(i=0;i<3;i++)
	for(j=0;j<3;j++) a->m[i][j]=b.m[j][i];
	a->m[3][0]=0.0; a->m[3][1]=0.0; a->m[3][2]=0.0; a->m[3][3]=1.0;
	for(i=0;i<3;i++) {
		s=0;
		for(j=0;j<3;j++) s-=b.m[j][i]*b.m[j][3];
		a->m[i][3]=s;
	}
}

/* inversion d'une matrice homogène du type Y=M.X+P */

M4 M4_Inv(M4 b)
{
	 int i,j;
	 float s;
	 M3 c;
	 M4 a;
	 
	 for(i=0;i<3;i++)
		 for(j=0;j<3;j++) c.m[i][j]=b.m[i][j];
	 
	 c=M3_Inv(c);
	 for(i=0;i<3;i++)
		 for(j=0;j<3;j++) a.m[i][j]=c.m[i][j];
	 
	 a.m[3][0]=0.0; a.m[3][1]=0.0; a.m[3][2]=0.0; a.m[3][3]=1.0;
	 
	 for(i=0;i<3;i++) {
		s=0;
		for(j=0;j<3;j++) s-=c.m[i][j]*b.m[j][3];
		a.m[i][3]=s;
	}
	 return a;
}

/* inverse of a 3x3 matrix */
M3 M3_Inv(M3 m)
{
	 float det;
	 M3 a;
	 
	 det = m.m[0][0]*m.m[1][1]*m.m[2][2]-m.m[0][0]*m.m[1][2]*m.m[2][1]-
		 m.m[1][0]*m.m[0][1]*m.m[2][2]+m.m[1][0]*m.m[0][2]*m.m[2][1]+
		 m.m[2][0]*m.m[0][1]*m.m[1][2]-m.m[2][0]*m.m[0][2]*m.m[1][1];

	 a.m[0][0] = (m.m[1][1]*m.m[2][2]-m.m[1][2]*m.m[2][1])/det;
	 a.m[0][1] = -(m.m[0][1]*m.m[2][2]-m.m[0][2]*m.m[2][1])/det;
	 a.m[0][2] = -(-m.m[0][1]*m.m[1][2]+m.m[0][2]*m.m[1][1])/det;
	 
	 a.m[1][0] = -(m.m[1][0]*m.m[2][2]-m.m[1][2]*m.m[2][0])/det;
	 a.m[1][1] = (m.m[0][0]*m.m[2][2]-m.m[0][2]*m.m[2][0])/det;
	 a.m[1][2] = -(m.m[0][0]*m.m[1][2]-m.m[0][2]*m.m[1][0])/det;

	 a.m[2][0] = (m.m[1][0]*m.m[2][1]-m.m[1][1]*m.m[2][0])/det;
	 a.m[2][1] = -(m.m[0][0]*m.m[2][1]-m.m[0][1]*m.m[2][0])/det;
	 a.m[2][2] = (m.m[0][0]*m.m[1][1]-m.m[0][1]*m.m[1][0])/det;
	 return a;
}

																										

/* normalisation d'un vecteur */
int V3_Norm(V3 *a)
{
	float n;
	n=sqrt(a->X*a->X+a->Y*a->Y+a->Z*a->Z);
	if (n==0) return 1;
	a->X/=n;
	a->Y/=n;
	a->Z/=n;
	return 0;
}

float DotV3(V3 *a,V3 *b)
{
	return (a->X*b->X+a->Y*b->Y+a->Z*b->Z);
}

void NegV3(V3 *a)
{
	a->X=-a->X;
	a->Y=-a->Y;
	a->Z=-a->Z;
}

void SubV3(V3 *a,V3 *b,V3 *c)
{
	a->X=b->X-c->X;
	a->Y=b->Y-c->Y;
	a->Z=b->Z-c->Z;
}

void AddV3(V3 *a,V3 *b,V3 *c)
{
	a->X=b->X+c->X;
	a->Y=b->Y+c->Y;
	a->Z=b->Z+c->Z;
}

M4 EvalVOM4(V3 *VRP,V3 *VPN,V3 *VUP)
{
	M4 a,b,vo;
	V3 r[3];
	int i;

/* T(-VRP) */
	a=TranslateM4(-VRP->X,-VRP->Y,-VRP->Z);
			
/* R */
	MoveV3(&r[2],VPN);
	V3_Norm(&r[2]);
	r[0]=V3_Cross(*VUP,r[2]);
	V3_Norm(&r[0]);
	r[1]=V3_Cross(r[2],r[0]);
	
	IdM4(&b);
	for(i=0;i<3;i++) {
		 b.m[0][i]=r[i].X;
		 b.m[1][i]=r[i].Y;
		 b.m[2][i]=r[i].Z;
	}
/* vo=R*T(-VRP) */
	 vo=MulM4(b,a);
	 return vo;
}

/* evaluate NPC matrix */
M4 EvalVMM4(float umin,float umax,float vmin,float vmax,
			V3 *PRP,float F,float B,float *zmin)
{
	M4 t,sh,s,vm;
	V3 DOP;	
	float vz,fz;

/* T(-PRP) */
	t=TranslateM4(-PRP->X,-PRP->Y,-PRP->Z);

/* SHpar	 */
	DOP.X=(umax+umin)/2-PRP->X;
	DOP.Y=(vmax+vmin)/2-PRP->Y;
	DOP.Z=-PRP->Z;
	IdM4(&sh);
	sh.m[0][2]=- DOP.X / DOP.Z;
	sh.m[1][2]=- DOP.Y / DOP.Z;

/* Scale Sper */
	vz=-PRP->Z;
	fz=-1/(vz+B);
	s=ScaleM4(-2*vz/(umax-umin)*fz,-2*vz/(vmax-vmin)*fz,fz);
	*zmin=(vz+F)*fz;

/* vm=Sper*SHpar*T(-PRP) */
	 vm=MulM4(s,MulM4(sh,t));
	 return vm;
}


V3 V3_New(float x,float y,float z)
{
	 V3 a;
	 a.X=x;
	 a.Y=y;
	 a.Z=z;
	 return a;
}

V4 V4_New(float x,float y,float z,float w)
{
  V4 a;
  a.X=x;
  a.Y=y;
  a.Z=z;
  a.W=w;
  return a;
}


V3 V3_Sub(V3 b,V3 c)
{
	 V3 a;
	 a.X=b.X-c.X;
	 a.Y=b.Y-c.Y;
	 a.Z=b.Z-c.Z;
	 return a;
}

V3 V3_Add(V3 b,V3 c)
{
	 V3 a;
	 a.X=b.X+c.X;
	 a.Y=b.Y+c.Y;
	 a.Z=b.Z+c.Z;
	 return a;
}

V3 V3_Cross(V3 a,V3 b)
{
	 V3 c;
	 c.X=a.Y*b.Z-a.Z*b.Y;
	 c.Y=a.Z*b.X-a.X*b.Z;
	 c.Z=a.X*b.Y-a.Y*b.X;
	 return c;
}

V3 V3_MulM3V3(M3 b,V3 c)
{
	 V3 a;
	 a.X=b.m[0][0]*c.X+b.m[0][1]*c.Y+b.m[0][2]*c.Z;
	 a.Y=b.m[1][0]*c.X+b.m[1][1]*c.Y+b.m[1][2]*c.Z;
	 a.Z=b.m[2][0]*c.X+b.m[2][1]*c.Y+b.m[2][2]*c.Z;
	 return a;
}

V3 V3_Neg(V3 a)
{
	 V3 b;
	 b.X=-a.X;
	 b.Y=-a.Y;
	 b.Z=-a.Z;
	 return b;
}

M3 M3_Mul(M3 a,M3 b)
{
	 int i,j,k;
	 float s;
	 M3 c;

	 for(i=0;i<3;i++)
		 for(j=0;j<3;j++) {
				s=0.0;
				for(k=0;k<3;k++) s+=a.m[i][k]*b.m[k][j];
				c.m[i][j]=s;
		 }
	 return c;
}



M3 M3_Identity(void)
{
	 int i,j;
	 M3 a;
	 for(i=0;i<3;i++)
		 for(j=0;j<3;j++) 
		 if (i==j) a.m[i][j]=1.0; else a.m[i][j]=0.0;
	 return a;
}

M3 M3_Rotate(float t,int u)
{
	 float s,c;
	 int v,w;
	 M3 a;
	 if ((v=u+1)>2) v=0;
	 if ((w=v+1)>2) w=0;
	 s=sin(t);
	 c=cos(t);
	 a=M3_Identity();
	 a.m[v][v]=c;	a.m[v][w]=-s;
	 a.m[w][v]=s;	a.m[w][w]=c;
	 return a;
}

