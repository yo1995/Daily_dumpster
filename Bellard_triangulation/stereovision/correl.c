#include <stdlib.h>
#include <stdio.h>
#include <math.h>
#include <getopt.h>

#include "zmath.h"
#include "graph.h"
#include "correl.h"

typedef unsigned char uchar;


/* 13x13 window for correlation */

#define WX 6
#define WY 6


/*
 * Calcul de l'image moyennée
 */

void ImgMean(float *imgm, unsigned char *img, int size_x, int size_y)
{
    int x, y, j;
    float a, *q, *n, k;

/*
   for(j=0;j<size_x*size_y;j++) {
   imgm[j]=img[j];
   }
   return;
 */

    k = 1.0 / ((2 * WX + 1) * (2 * WY + 1));

    q = malloc(size_x * sizeof(float));
    n = malloc(size_x * sizeof(float));

    for (y = WY; y < size_y - WY; y++) {
	if (y == WY) {
	    for (x = 0; x < size_x; x++) {
		a = 0;
		for (j = 0; j < 2 * WY + 1; j++) {
		    a += (float) img[j * size_x + x];
		}
		q[x] = a;
	    }
	} else {
	    for (x = 0; x < size_x; x++) {
		q[x] += (float) img[(y + WY) * size_x + x] - (float) img[(y - WY - 1) * size_x + x];
	    }
	}

	a = 0;
	for (j = 0; j < 2 * WX + 1; j++)
	    a += q[j];
	a = a * k;
	for (x = 0; x <= WX; x++)
	    n[x] = a;

	for (x = WX + 1; x < size_x - WX; x++) {
	    a = a + (q[x + WX] - q[x - WX - 1]) * k;
	    n[x] = a;
	}

	for (x = size_x - WX; x < size_x; x++)
	    n[x] = n[size_x - WX - 1];

	if (y == WY) {
	    for (j = 0; j <= WY; j++) {
		for (x = 0; x < size_x; x++)
		    imgm[j * size_x + x] = (float) img[j * size_x + x] - n[x];
	    }
	} else {
	    for (x = 0; x < size_x; x++)
		imgm[y * size_x + x] = (float) img[y * size_x + x] - n[x];
	}
    }

    for (y = size_y - WY; y < size_y; y++) {
	for (x = 0; x < size_x; x++)
	    imgm[y * size_x + x] = (float) img[y * size_x + x] - n[x];
    }

    free(q);
    free(n);
}

void Correlate(float *imgd, float *img1, float *img2,
	       int xsize, int ysize,
	       int dmin, int dmax,
	       float threshold1,
	       float threshold2)
{
    float *q_tab, *n_tab;
    float *q, *n, *r1, *s1, *r2, *s2;
    float a, b, c, p1, p2;
    int j, x, y, d, xmin, xmax;

    /* variables pour la recherche du max */
    int *dcmax1, *dcmax2, dc;
    int umin, umax, u, ucmax;
    float cmax;
    float y0, y1, y2, pmax;


    q_tab = malloc(sizeof(float) * xsize * (dmax - dmin + 1));
    n_tab = malloc(sizeof(float) * xsize * (dmax - dmin + 1));

    r1 = malloc(sizeof(float) * xsize);
    s1 = malloc(sizeof(float) * xsize);

    r2 = malloc(sizeof(float) * xsize);
    s2 = malloc(sizeof(float) * xsize);

    dcmax1 = malloc(sizeof(int) * xsize);
    dcmax2 = malloc(sizeof(int) * xsize);

    /* positionnement des bords */
    for (y = 0; y < WY; y++) {
	for (x = 0; x < xsize; x++) {
	    imgd[y * xsize + x] = NOTFOUND;
	    imgd[(ysize - y - 1) * xsize + x] = NOTFOUND;
	}
    }
    for (x = 0; x < WX; x++) {
	for (y = WY; y < ysize - WY; y++) {
	    imgd[y * xsize + x] = NOTFOUND;
	    imgd[y * xsize + (xsize - x - 1)] = NOTFOUND;
	}
    }


    for (y = WY; y < (ysize - WY); y++) {
	if ((y % 64) == 0)
	    fprintf(stderr, "y=%d\n", y);

	/* calculation of s1[x] */
	if (y == WY) {
	    for (x = 0; x < xsize; x++) {
		a = 0;
		for (j = 0; j < 2 * WY + 1; j++) {
		    b = img1[j * xsize + x];
		    a += b * b;
		}
		r1[x] = a;
	    }
	} else {
	    for (x = 0; x < xsize; x++) {
		b = img1[(y + WY) * xsize + x];
		c = img1[(y - WY - 1) * xsize + x];
		r1[x] += b * b - c * c;
	    }
	}

	a = 0;
	for (j = 0; j < 2 * WX + 1; j++)
	    a += r1[j];
	if (a <= 0)
	    s1[WX] = 0;
	else
	    s1[WX] = 1 / sqrt(a);

	for (x = WX + 1; x < xsize - WX; x++) {
	    a = a + r1[x + WX] - r1[x - WX - 1];
	    if (a <= 0)
		s1[x] = 0;
	    else
		s1[x] = 1 / sqrt(a);
	}


	/* calculation of s2[x] */
	if (y == WY) {
	    for (x = 0; x < xsize; x++) {
		a = 0;
		for (j = 0; j < 2 * WY + 1; j++) {
		    b = img2[j * xsize + x];
		    a += b * b;
		}
		r2[x] = a;
	    }
	} else {
	    for (x = 0; x < xsize; x++) {
		b = img2[(y + WY) * xsize + x];
		c = img2[(y - WY - 1) * xsize + x];
		r2[x] += b * b - c * c;
	    }
	}

	a = 0;
	for (j = 0; j < 2 * WX + 1; j++)
	    a += r2[j];
	if (a <= 0)
	    s2[WX] = 0;
	else
	    s2[WX] = 1 / sqrt(a);

	for (x = WX + 1; x < xsize - WX; x++) {
	    a = a + r2[x + WX] - r2[x - WX - 1];
	    if (a <= 0)
		s2[x] = 0;
	    else
		s2[x] = 1 / sqrt(a);
	}

	/* calculation of n[x] */
	for (d = dmin; d <= dmax; d++) {
	    q = &q_tab[(d - dmin) * xsize];
	    n = &n_tab[(d - dmin) * xsize];

	    if (d <= 0) {
		xmin = -d;
		xmax = xsize - 1;
	    } else {
		xmin = 0;
		xmax = xsize - 1 - d;
	    }

	    if (y == WY) {
		for (x = xmin; x <= xmax; x++) {
		    a = 0;
		    for (j = 0; j < 2 * WY + 1; j++) {
			a += img1[j * xsize + x] * img2[j * xsize + x + d];
		    }
		    q[x] = a;
		}
	    } else {
		for (x = xmin; x <= xmax; x++) {
		    p1 = img1[(y + WY) * xsize + x] * img2[(y + WY) * xsize + x + d];
		    p2 = img1[(y - WY - 1) * xsize + x] * img2[(y - WY - 1) * xsize + x + d];
		    q[x] += p1 - p2;
		}
	    }

	    xmin += WX;
	    xmax -= WX;

	    a = 0;
	    for (j = -WX; j <= WX; j++)
		a += q[xmin + j];
	    n[xmin] = a;

	    for (x = xmin + 1; x <= xmax; x++) {
		a = a + q[x + WX] - q[x - WX - 1];
		n[x] = a;
	    }



	}

	/* recherche du maximum */

	/* on déplace l'image 2 sur l'image 1 */
	for (x = WX; x < xsize - WX; x++) {

	    umin = x + dmin;
	    umax = x + dmax;
	    cmax = -1E10;
	    ucmax = -1;
	    imgd[y * xsize + x] = NOTFOUND;

	    if (umax >= WX && umin < xsize - WX) {

		if (umin < WX)
		    umin = WX;
		if (umax > xsize - WX - 1)
		    umax = xsize - WX - 1;

		for (u = umin + 1; u < umax; u++) {
		    a = n_tab[(u - x - dmin) * xsize + x] * s2[u];
		    if (a > cmax) {
			cmax = a;
			ucmax = u;
		    }
		}

		if (ucmax != -1) {

		    y0 = n_tab[(ucmax - x - 1 - dmin) * xsize + x] * s2[ucmax - 1];
		    y1 = n_tab[(ucmax - x - dmin) * xsize + x] * s2[ucmax];
		    y2 = n_tab[(ucmax - x + 1 - dmin) * xsize + x] * s2[ucmax + 1];

		    if ((y1 * s1[x]) >= threshold1) {
			a = (y0 + y2 - 2 * y1);
			if ((a * s1[x]) < -threshold2) {
			    pmax = 0.5 * (y0 - y2) / a;
			    if (pmax >= -1 && pmax <= 1) {
				imgd[y * xsize + x] = (ucmax - x + pmax);
			    }
			}
		    }
		}
	    }
	    dcmax1[x] = ucmax;
	}

	/* on déplace l'image 1 sur l'image 2 */
	for (x = WX; x < xsize - WX; x++) {

	    umin = x - dmax;
	    umax = x - dmin;
	    cmax = -1E10;
	    ucmax = -1;

	    if (umax >= WX && umin < xsize - WX) {

		if (umin < WX)
		    umin = WX;
		if (umax > xsize - WX - 1)
		    umax = xsize - WX - 1;

		for (u = umin; u <= umax; u++) {
		    a = n_tab[(x - u - dmin) * xsize + u] * s1[u];
		    if (a > cmax) {
			cmax = a;
			ucmax = u;
		    }
		}
	    }
	    dcmax2[x] = ucmax;
	}

	/* si les 2 maximums ne sont pas en correspondance, on élimine 
	 * le point 
	 */

	for (x = WX; x < xsize - WX; x++) {
	    dc = dcmax1[x];
	    if (dc == -1 || dcmax2[dc] == -1 || abs(dcmax2[dc] - x) > 1) {
		imgd[y * xsize + x] = NOTFOUND;
	    }
	}

    }

}

/* recherche du pic de seconde importance : non mis dans le code

   float cmax1;
   cmax1=-1E10;

   for(u=umin+1;u<umax;u++) {
   if (abs(u-ucmax)>WX) {
   y0=n_tab[(u-1-x-dmin)*xsize+x] * s2[u-1];
   y1=n_tab[(u-x-dmin)*xsize+x] * s2[u];
   y2=n_tab[(u+1-x-dmin)*xsize+x] * s2[u+1];
   if (y1>=y0 && y1>=y2 && y1>cmax1) {
   cmax1=y1;
   }
   }
   }
   if (cmax1 != -1E10) {
   cmax1=(cmax-cmax1) * s1[x];
   } else {
   cmax1=(cmax-1)*s1[x];
   }

   if (cmax1>=0.1) {
 */

/* tests */
/*
   for(x=xmin;x<=xmax;x++) {
   a=0;
   for(i=-WX;i<=WX;i++) {
   b=0;
   for(j=-WY;j<=WY;j++) { 
   b+=img1[(y+j)*xsize+(x+i)] * img2[(y+j)*xsize+(x+i+d)];
   }
   a+=b;
   }
   if (a!=n[x]) {
   printf("n[%d,%d,%d]=%f calc=%f\n",x,y,d,a/65536.0,n[x]/65536.0);
   }
   }

   for(x=WX;x<xsize-WX;x++) {
   a=0;
   for(i=-WX;i<=WX;i++)
   for(j=-WY;j<=WY;j++) { 
   b=img2[(y+j)*xsize+(x+i)];
   a+=b*b;
   }
   a=1/sqrt(a);

   if (a!=s[x]) {
   printf("m[%d,%d]=%f calc=%f\n",x,y,a,s[x]);
   }
   }
 */

int *cnx_seen;			/* >=-1: already seen, <-1: value */
int cnx_val, cnx_first, cnx_cnt, cnx_sx, cnx_sy;

void cnx_find(int x, int y)
{
    int p;

    if (x < 0 || x >= cnx_sx || y < 0 || y >= cnx_sy)
	return;
    p = y * cnx_sx + x;
    if (cnx_seen[p] >= -1 || cnx_seen[p] != cnx_val)
	return;

    cnx_seen[p] = cnx_first;
    cnx_first = p;
    cnx_cnt++;

    cnx_find(x - 1, y);
    cnx_find(x + 1, y);
    cnx_find(x, y - 1);
    cnx_find(x, y + 1);
}

void cnx_interpolate(float *cnx_fbuf, int x, int y)
{
    int x1, x2, p1, p2;
    float a;

    x1 = x;
    while (x1 >= 0 && cnx_fbuf[y * cnx_sx + x1] == NOTFOUND)
	x1--;
    x2 = x;
    while (x2 < cnx_sx && cnx_fbuf[y * cnx_sx + x2] == NOTFOUND)
	x2++;

    if (x1 < 0 || x2 >= cnx_sx)
	return;

    p1 = y * cnx_sx + x1;
    p2 = y * cnx_sx + x2;
    a = cnx_fbuf[y * cnx_sx + x] = cnx_fbuf[p1] +
	((cnx_fbuf[p2] - cnx_fbuf[p1]) / (x2 - x1)) * (x - x1);
/*       printf("x=%d y=%d x1=%d x2=%d %f %f\n",x,y,x1,x2,cnx_fbuf[p1],cnx_fbuf[p2]); */
}

#define AREA_MIN    500
#define AREA_SMOOTH 400

void ZSmooth(float *bufs, int size_x, int size_y)
{
    int x, y, i, p;

    cnx_seen = malloc(sizeof(int) * size_x * size_y);
    cnx_sx = size_x;
    cnx_sy = size_y;

    for (i = 0; i < size_x * size_y; i++) {
	cnx_seen[i] = -2 - (bufs[i] == NOTFOUND);
    }

    printf("smoothing 1\n");

    for (y = 0; y < size_y; y++) {
	for (x = 0; x < size_x; x++) {

	    cnx_cnt = 0;
	    cnx_first = -1;
	    cnx_val = -2;	/* !=NOTFOUND */
	    cnx_find(x, y);
	    if (cnx_cnt > 0 && cnx_cnt < AREA_MIN) {
		p = cnx_first;
		while (p != -1) {
		    bufs[p] = NOTFOUND;
		    p = cnx_seen[p];
		}
	    }
	}
    }

    printf("smoothing 2\n");

    for (y = 0; y < size_y; y++) {
	for (x = 0; x < size_x; x++) {

	    cnx_cnt = 0;
	    cnx_first = -1;
	    cnx_val = -3;	/* ==NOTFOUND */
	    cnx_find(x, y);
	    if (cnx_cnt > 0 && cnx_cnt < AREA_SMOOTH) {
		p = cnx_first;
		while (p != -1) {
		    cnx_interpolate(bufs, p % cnx_sx, p / cnx_sx);
		    p = cnx_seen[p];
		}
	    }
	}
    }

    free(cnx_seen);
    printf("smoothing end\n");
}


void Calc2d(V3 * pts, float *imgd, int size_x, int size_y, int factor)
{
    int x, y, x1, y1, sx, sy;
    V3 v;

    sx = size_x / factor;
    sy = size_y / factor;

    for (y = 0, y1 = 0; y < sy; y++, y1 += factor) {
	for (x = 0, x1 = 0; x < sx; x++, x1 += factor) {
	    if (imgd[y1 * size_x + x1] != NOTFOUND) {
		v.X = x1;
		v.Y = y1;
		v.Z = imgd[y1 * size_x + x1] * factor;
		pts[y * sx + x].X = v.X;
		pts[y * sx + x].Y = v.Y;
		pts[y * sx + x].Z = v.Z;
	    } else {
		pts[y * sx + x].X = NOTFOUND;
	    }
	}
    }
}

void Calc3d(V3 * pts,
	    M34 proj1, M34 proj2, M3 rect1, M3 rect2,
	    float *imgd, int size_x, int size_y, int factor)
{
    int x1, y1, x, y, i, j, k, sx, sy;
    V3 v[2], p_t[2], c[2], dir[2];
    M3 p_m[2], p_inv[2], *rect;
    float det, l1;

    for (i = 0; i < 3; i++)
	for (j = 0; j < 3; j++) {
	    p_m[0].m[i][j] = proj1.m[i][j];
	    p_m[1].m[i][j] = proj2.m[i][j];
	}
    for (i = 0; i < 3; i++) {
	p_t[0].v[i] = proj1.m[i][3];
	p_t[1].v[i] = proj2.m[i][3];
    }

    for (k = 0; k < 2; k++) {
	if (k == 0)
	    rect = &rect1;
	else
	    rect = &rect2;
	p_t[k] = V3_MulM3V3(*rect, p_t[k]);
	p_m[k] = M3_Mul(*rect, p_m[k]);

	p_inv[k] = M3_Inv(p_m[k]);
	c[k] = V3_Neg(V3_MulM3V3(p_inv[k], p_t[k]));
    }

    sx = size_x / factor;
    sy = size_y / factor;

    /* non optimisé ici */

    for (y = 0, y1 = 0; y < sy; y++, y1 += factor) {
	for (x = 0, x1 = 0; x < sx; x++, x1 += factor) {
	    if (imgd[y1 * size_x + x1] != NOTFOUND) {
		v[0].X = x1;
		v[0].Y = y1;
		v[0].Z = 1;

		v[1].X = x1 + imgd[y1 * size_x + x1];
		v[1].Y = y1;
		v[1].Z = 1;

		dir[0] = V3_MulM3V3(p_inv[0], v[0]);
		dir[1] = V3_MulM3V3(p_inv[1], v[1]);

		/* intersection des 2 droites */
		det = dir[1].X * dir[0].Z - dir[1].Z * dir[0].X;
		if (fabs(det) <= 1E-6) {
		    pts[y * sx + x].X = NOTFOUND;
		} else {
		    l1 = (-dir[1].Z * (c[1].X - c[0].X) + dir[1].X * (c[1].Z - c[0].Z)) / det;
		    /* calcul de X,Y,Z à l'intersection */
		    pts[y * sx + x].X = c[0].X + l1 * dir[0].X;
		    pts[y * sx + x].Y = c[0].Y + l1 * dir[0].Y;
		    pts[y * sx + x].Z = c[0].Z + l1 * dir[0].Z;

/*                                                      
   printf("x=%d y=%d l1=%f l2=%f\n",x1,y1,l1,
   (-dir[1].Z*(c[1].Y-c[0].Y)+dir[1].Y*(c[1].Z-c[0].Z)) / 
   (dir[1].Y*dir[0].Z-dir[1].Z*dir[0].Y) 
   );
 */
		}

	    } else {
		pts[y * sx + x].X = NOTFOUND;
	    }
	}
    }

}



void DistSave(float *bufd, int size_x, int size_y, char *dist_filename)
{
    float a, min, max, mean, sum, sum2, sigma;
    int x, y, count;
    unsigned char *buf;

    min = 1E10;
    max = -1E10;
    sum = 0;
    sum2 = 0;
    count = 0;
    for (y = 0; y < size_y; y++)
	for (x = 0; x < size_x; x++) {
	    a = bufd[y * size_x + x];
	    if (a != NOTFOUND) {
		count++;
		if (a < min)
		    min = a;
		if (a > max)
		    max = a;
		sum += a;
		sum2 += a * a;
	    }
	}
    if (count == 0) {
	mean = 0;
	sigma = 0;
	min = 0;
	max = 0;
    } else {
	mean = sum / count;
	sigma = sqrt(sum2 / count - mean * mean);
    }
    printf("Statistics:\n");
    printf("dist_found=%d\n", count);
    printf("dist_min=%f dist_max=%f\n", min, max);
    printf("dist_mean=%f dist_sigma=%f\n", mean, sigma);

    buf = malloc(size_x * size_y);

    for (y = 0; y < size_y; y++)
	for (x = 0; x < size_x; x++) {
	    a = bufd[y * size_x + x];
	    if (a != NOTFOUND) {
		buf[y * size_x + x] = (a - min) / (max - min) * 255.0;
	    } else {
		buf[y * size_x + x] = 0;
	    }
	}

    PGM_Save(buf, size_x, size_y, dist_filename);
    free(buf);
}



/*


 * RECTIFICATION
 * 
 * 
 */

/* 
 * Mapping de l'image2 dans l'image1 avec la matrice m
 * On effectue une interpolation linéaire pour lisser l'image résultante.
 */

void ZMap(unsigned char *buf1, int size_x1, int size_y1,
	  unsigned char *buf2, int size_x2, int size_y2,
	  M3 m)
{
    int x, y;
    float x2_x, y2_x, w2_x, x2_y, y2_y, w2_y;
    float x2, y2;
    int x2_i, y2_i;
    float pix00, pix10, pix01, t10, t01;

    x2_y = m.m[0][2];
    y2_y = m.m[1][2];
    w2_y = m.m[2][2];

    for (y = 0; y < size_y1; y++) {
	x2_x = x2_y;
	y2_x = y2_y;
	w2_x = w2_y;
	for (x = 0; x < size_x1; x++) {
	    x2 = x2_x / w2_x;
	    y2 = y2_x / w2_x;
	    if (x2 >= 0 && x2 < (size_x2 - 1) && y2 >= 0 && y2 < (size_y2 - 1)) {
		x2_i = (int) (floor(x2));
		y2_i = (int) (floor(y2));
		t10 = x2 - floor(x2);
		t01 = y2 - floor(y2);
		if ((t01 + t10) <= 1) {
		    pix00 = buf2[y2_i * size_x2 + x2_i];
		    pix10 = buf2[y2_i * size_x2 + (x2_i + 1)] - pix00;
		    pix01 = buf2[(y2_i + 1) * size_x2 + x2_i] - pix00;
		    buf1[y * size_x1 + x] = (int) (pix00 + t10 * pix10 + t01 * pix01);
		} else {
		    pix00 = buf2[(y2_i + 1) * size_x2 + (x2_i + 1)];
		    pix01 = buf2[y2_i * size_x2 + (x2_i + 1)] - pix00;
		    pix10 = buf2[(y2_i + 1) * size_x2 + x2_i] - pix00;
		    buf1[y * size_x1 + x] = (int) (pix00 + (1 - t10) * pix10 + (1 - t01) * pix01);
		}
	    } else {
		buf1[y * size_x1 + x] = 0;
	    }
	    x2_x += m.m[0][0];
	    y2_x += m.m[1][0];
	    w2_x += m.m[2][0];
	}
	x2_y += m.m[0][1];
	y2_y += m.m[1][1];
	w2_y += m.m[2][1];
    }
}


void Matrix_Load(float *m, int y, int x, char *filename)
{
    FILE *f;
    int i, j;

    f = fopen(filename, "r");
    if (f == NULL) {
	perror("matrix");
	exit(1);
    }
    for (i = 0; i < y; i++)
	for (j = 0; j < x; j++) {
	    fscanf(f, "%f", &m[i * x + j]);
	}
    fclose(f);
}


void print_help(void)
{
    printf("usage: correl [-x window_xsize] [-y window_ysize] [-m] [-f factor]\n"
	 "              [-r rect] [-p proj] [-S] [--dmin n] [--dmax n]\n"
	   "              image1.pgm image2.pgm\n"
	   "Version 1.00 By Fabrice Bellard\n"
	   "\n"
	   "-m        : turn on mapping\n"
	   "-f factor : the size of the displayed image will be divided by factor\n"
	   "-S        : save the rectified images\n"
	);
}

static struct option long_options[] =
{
    {"dmin", 1, 0, 0},
    {"dmax", 1, 0, 0},
    {"smooth", 0, 0, 0},
    {"dist", 1, 0, 0},
    {"threshold1", 1, 0, 0},
    {"threshold2", 1, 0, 0},
    {NULL, 0, 0, 0},
};

int main(int argc, char *argv[])
{
    int c, i;
    int option_index;
    int size_x, size_y;
    int window_x, window_y, mapping, factor;
    char buf[256], dist_filename[256];
    int dmin, dmax, proj_given, rect_given, calc_2d, rect_save;
    int smooth_calc;
    M3 rect[2], rect_inv[2];
    M34 proj[2];
    V3 *pts;
    int xsize[2], ysize[2];
    unsigned char *img_org[2], *img_rect[2], *img_map;
    float *imgd;
    float *img_mean[2];
    float threshold1, threshold2;

    window_x = 320;
    window_y = 240;
    mapping = 0;
    factor = 4;
    rect_given = 0;
    proj_given = 0;
    calc_2d = 0;
    rect_save = 0;
    dmin = -60;
    dmax = 60;
    smooth_calc = 0;
    dist_filename[0] = '\0';
    threshold1 = 0.6;
    threshold2 = 0.01;

    while (1) {
	c = getopt_long(argc, argv, "h?x:y:mf:r:p:S", long_options, &option_index);
	if (c == -1)
	    break;
	switch (c) {

	case 0:
	    /* long opt */
	    switch (option_index) {
	    case 0:
		/* dmin */
		dmin = atoi(optarg);
		break;
	    case 1:
		/* dmax */
		dmax = atoi(optarg);
		break;
	    case 2:
		/* smooth */
		smooth_calc = 1;
		break;
	    case 3:
		/* dist */
		strcpy(dist_filename, optarg);
		break;
	    case 4:
		/* threshold */
		threshold1 = atof(optarg);
		break;
	    case 5:
		/* threshold */
		threshold2 = atof(optarg);
		break;
	    }
	    break;

	case 'h':
	case '?':
	    print_help();
	    exit(0);
	    break;
	case 'x':
	    window_x = atoi(optarg);
	    break;
	case 'y':
	    window_y = atoi(optarg);
	    break;
	case 'm':
	    mapping = 1;
	    break;
	case 'f':
	    factor = atoi(optarg);
	    break;
	case '2':
	    calc_2d = 1;
	    break;
	case 'r':
	    sprintf(buf, "%s1", optarg);
	    Matrix_Load((float *) &rect[0].m, 3, 3, buf);
	    sprintf(buf, "%s2", optarg);
	    Matrix_Load((float *) &rect[1].m, 3, 3, buf);
	    fprintf(stderr, "Rectification matrices loaded.\n");
	    rect_given = 1;
	    break;
	case 'p':
	    sprintf(buf, "%s1", optarg);
	    Matrix_Load((float *) &proj[0].m, 3, 4, buf);
	    sprintf(buf, "%s2", optarg);
	    Matrix_Load((float *) &proj[1].m, 3, 4, buf);
	    fprintf(stderr, "Projection matrices loaded.\n");
	    proj_given = 1;
	    break;

	case 'S':
	    rect_save = 1;
	    break;
	}
    }
    if (optind == argc) {
	print_help();
	exit(0);
    }
    /* chargement des images */
    for (i = 0; i < 2; i++) {
	printf("Loading %s\n", argv[optind + i]);
	img_org[i] = PGM_Load(&xsize[i], &ysize[i], argv[optind + i]);
	size_x = xsize[0];
	size_y = ysize[0];
    }
    if (xsize[0] != xsize[1] || ysize[0] != ysize[1]) {
	fprintf(stderr, "The images have not the same size\n");
	exit(1);
    }
    printf("%dx%d images\n", size_x, size_y);

    /* rectification des images */
    if (rect_given) {
	fprintf(stderr, "Rectification\n");
	for (i = 0; i < 2; i++) {
	    img_rect[i] = malloc(size_x * size_y);
	    rect_inv[i] = M3_Inv(rect[i]);
	    ZMap(img_rect[i], size_x, size_y, img_org[i], size_x, size_y, rect_inv[i]);
	    if (rect_save) {
	    char buf2[256];
	    printf("Save is on");
		sprintf(buf2, "rect%d.pgm", i + 1);
		PGM_Save(img_rect[i], size_x, size_y, buf2);
	    }
	}
	if (rect_save)
	    exit(0);
    } else {
	for (i = 0; i < 2; i++)
	    img_rect[i] = img_org[i];
	rect[0] = M3_Identity();
	rect[1] = M3_Identity();
    }

    printf("Correlation\n");

    /* moyennage */
    for (i = 0; i < 2; i++) {
	img_mean[i] = malloc(size_x * size_y * sizeof(float));
	ImgMean(img_mean[i], img_rect[i], size_x, size_y);
    }
/*       
   DistSave(img_mean[0],size_x,size_y,"m1.pgm");
   DistSave(img_mean[1],size_x,size_y,"m2.pgm");
 */

    /* correlation */

    imgd = malloc(size_x * size_y * sizeof(float));
#if 1
    Correlate(imgd, img_mean[0], img_mean[1], size_x, size_y,
	      dmin, dmax, threshold1, threshold2);
#else
    /* test image */
    {
      int x,y;
      float r;
      for(y=0;y<size_y;y++) 
        for(x=0;x<size_x;x++) {
          r=sqrt(x*x+y*y);
          imgd[y*size_x+x]=30*sin(r/50);
        }
    }
    proj_given=0;
    mapping=0;
#endif



    for (i = 0; i < 2; i++)
	free(img_mean[i]);

    if (smooth_calc) {
	ZSmooth(imgd, size_x, size_y);
    }
    if (dist_filename[0] != '\0') {
	DistSave(imgd, size_x, size_y, dist_filename);
	exit(0);
    }
    pts = malloc(size_x * size_y / (factor * factor) * sizeof(V3));
    if (proj_given) {
	fprintf(stderr, "Converting to 3d\n");
	Calc3d(pts, proj[0], proj[1], rect[0], rect[1], imgd, size_x, size_y, factor);
    } else {
	Calc2d(pts, imgd, size_x, size_y, factor);
    }

    if (mapping) {
	img_map = malloc(256 * 256);
	G2D_Resize(img_map, 256, 256, img_rect[0], size_x, size_y);
    } else {
	img_map = NULL;
    }

    // display(pts, size_x / factor, size_y / factor, window_x, window_y, img_map);
    return 0;
}
