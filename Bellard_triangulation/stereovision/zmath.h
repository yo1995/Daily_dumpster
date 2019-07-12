#ifndef __ZMATH__
#define __ZMATH__

/* Matrix & Vertex */

typedef struct {
	float m[4][4];
} M4;

typedef struct {
	float m[3][3];
} M3;

typedef struct {
	 float m[3][4];
} M34;


#define X v[0]
#define Y v[1]
#define Z v[2]
#define W v[3]

typedef struct {
	 float v[3];
} V3;

typedef struct {
	 float v[4];
} V4;
	

#define UX	0
#define UY	1
#define UZ	2

void IdM4(M4 *a);
void MoveM4(M4 *a,M4 *b);
void MoveV3(V3 *a,V3 *b);
void MulM4V3(V3 *a,M4 *b,V3 *c);
void MulM4V4(V4 * a,M4 *b,V4 * c);
void MulM3V3(V3 *a,M4 *b,V3 *c);
void AddV3(V3 *a,V3 *b,V3 *c);
void SubV3(V3 *a,V3 *b,V3 *c);
void CrossV3(V3 *c,V3 *a,V3 *b);
int NormV3(V3 *a);
float DotV3(V3 *a,V3 *b);

void InvOM4(M4 *a,M4 b);

M4 MulM4(M4 a,M4 b);
M4 ScaleM4(float sx,float sy,float sz);
M4 RotateM4(float t,int u);
M4 TranslateM4(float x,float y,float z);


/* 3D settings */

/* evaluate the VRC matrix */
/*M4 EvalVOM4(V3 *VRP,V3 *VPN,V3 *VUP);
*/
/* evaluate NPC matrix */
M4 EvalVMM4(float umin,float umax,float vmin,float vmax,
			V3 *PRP,float F,float B,float *zmin);
		
V3 V3_New(float x,float y,float z);
V4 V4_New(float x,float y,float z,float w);
V3 V3_Add(V3 a,V3 b);
V3 V3_Sub(V3 a,V3 b);
V3 V3_Cross(V3 a,V3 b);
V3 V3_Neg(V3 a);
int V3_Norm(V3 *a);

M4 M4_Inv(M4 b);
M3 M3_Inv(M3 b);
V3 V3_MulM3V3(M3 b,V3 c);
M3 M3_Mul(M3 a,M3 b);
M3 M3_Rotate(float t,int u);
M3 M3_Identity(void);

#endif  __ZMATH__
