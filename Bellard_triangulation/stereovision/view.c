/*
 * 3D viewing. Converted from proprietary display interface to OpenGL
 */

#include <stdlib.h>
#include <stdio.h>
#include <math.h>

#include <X11/Xlib.h>
#include <X11/Xutil.h>
#include <X11/keysym.h>

#include <GL/glx.h> 
#include <GL/gl.h> 


#include "graph.h"
#include "zmath.h"
#include "correl.h"

/* compute the normals */
int try(V3 *n,V3 *tab,int sx,int sy,int x1,int y1,int x2,int y2,int x3,int y3)
{
  V3 *p0,*p1,*p2,v1,v2;

  if (x1 < 0 || x1 >= sx || y1 < 0 || y1 >= sy ||
      x2 < 0 || x2 >= sx || y2 < 0 || y2 >= sy ||
      x3 < 0 || x3 >= sx || y3 < 0 || y3 >= sy) return 0;

  p0=&tab[x1*sx+y1];
  p1=&tab[x2*sx+y2];
  p2=&tab[x3*sx+y3];
  
  if (p0->X == NOTFOUND || p1->X == NOTFOUND || p2->X == NOTFOUND) return 0;

  v1=V3_Sub(*p1,*p0);
  v2=V3_Sub(*p2,*p0);
  *n=V3_Cross(v1,v2);
  V3_Norm(n);
  return 1;
}

void compute_normals(V3 *norm,V3 *tab,int size_x,int size_y)
{
  int x,y;
  V3 *n;

  for(x=0;x<size_x;x++) {
    for(y=0;y<size_y;y++) {
      n=&norm[x*size_x+y];
      if (try(n,tab,size_x,size_y,x,y,x+1,y,x,y+1) ||
          try(n,tab,size_x,size_y,x,y,x+1,y,x,y-1) ||
          try(n,tab,size_x,size_y,x,y,x-1,y,x,y+1) ||
          try(n,tab,size_x,size_y,x,y,x-1,y,x,y-1)) {
      } else {
        n->X=1;
        n->Y=0;
        n->Z=0;
      }
    }
  }
}



/* the polygon code is no longer functional because we do not compute the normals */
void drawSurfacePoly(V3 *tab,V3 *norm,int size_x,int size_y)
{
  int x,y;
  V3 *pt[4];

  /* triangles */

  glBegin(GL_QUADS);

  for(x=0;x<size_x-1;x++) {
    for(y=0;y<size_y-1;y++) {
      pt[0]=&tab[y*size_x+x];
      pt[1]=&tab[y*size_x+(x+1)];
      pt[2]=&tab[(y+1)*size_x+(x+1)];
      pt[3]=&tab[(y+1)*size_x+x];
      
      if (pt[0]->X != NOTFOUND && 
          pt[1]->X != NOTFOUND && 
          pt[2]->X != NOTFOUND && 
          pt[3]->X != NOTFOUND) {

        glTexCoord2f((float) x /(size_x-1),(float) y /(size_y-1));
        glNormal3fv(norm[y*size_x+x].v);
        glVertex3f(pt[0]->X,pt[0]->Y,pt[0]->Z);
        
        glTexCoord2f((float) (x+1) /(size_x-1),(float) y /(size_y-1));
        glNormal3fv(norm[y*size_x+(x+1)].v);
        glVertex3f(pt[1]->X,pt[1]->Y,pt[1]->Z);
        
        glTexCoord2f((float) (x+1) /(size_x-1),(float) (y+1) /(size_y-1));
        glNormal3fv(norm[(y+1)*size_x+(x+1)].v);
        glVertex3f(pt[2]->X,pt[2]->Y,pt[2]->Z);
        
        glTexCoord2f((float) x /(size_x-1),(float) (y+1) /(size_y-1));
        glNormal3fv(norm[(y+1)*size_x+x].v);
        glVertex3f(pt[3]->X,pt[3]->Y,pt[3]->Z);
      }
    }
  }
  glEnd();
}

void drawSurfaceLine(V3 *tab,int size_x,int size_y)
{
  int x,y;
  V3 *pt[2];

  /* triangles */

  glBegin(GL_LINES);
  for(x=0;x<size_x-1;x++) {
    for(y=0;y<size_y;y++) {
      pt[0]=&tab[y*size_x+x];
      pt[1]=&tab[y*size_x+x+1];
      
      if (pt[0]->X != NOTFOUND && pt[1]->X != NOTFOUND) {
        glVertex3f(pt[0]->X,pt[0]->Y,pt[0]->Z);
        glVertex3f(pt[1]->X,pt[1]->Y,pt[1]->Z);
      }
    }
  }



  for(x=0;x<size_x;x++) {
    for(y=0;y<size_y-1;y++) {
      pt[0]=&tab[y*size_x+x];
      pt[1]=&tab[(y+1)*size_x+x];

      if (pt[0]->X != NOTFOUND && pt[1]->X != NOTFOUND) {
        glVertex3f(pt[0]->X,pt[0]->Y,pt[0]->Z);
        glVertex3f(pt[1]->X,pt[1]->Y,pt[1]->Z);
      }
    }
  }
  glEnd();
}

/*
 * Bounding box
 */

void GetCenter(V3 *center,V3 *pmin,V3 *pmax,V3 *pts,int nbpts)
{
  int i,first;
  V3 sum;
	 
  first=1;
  for(i=0;i<nbpts;i++) 
    if (pts[i].X != NOTFOUND) {
      if (first) {
        *pmin=pts[i];
        *pmax=pts[i];
        sum=pts[i];
        first=0;
      } else {
        if (pts[i].X<pmin->X) pmin->X=pts[i].X;
        if (pts[i].X>pmax->X) pmax->X=pts[i].X;
        
        if (pts[i].Y<pmin->Y) pmin->Y=pts[i].Y;
        if (pts[i].Y>pmax->Y) pmax->Y=pts[i].Y;
        
        if (pts[i].Z<pmin->Z) pmin->Z=pts[i].Z;
        if (pts[i].Z>pmax->Z) pmax->Z=pts[i].Z;
        sum=V3_Add(sum,pts[i]);
      }
    }
  center->X=sum.X/nbpts;
  center->Y=sum.Y/nbpts;
  center->Z=sum.Z/nbpts;
}		 

/* 
 * draw a cube 
 */

void drawCube(V3 *pmin,V3 *pmax)
{
  V3 *p;
  int i,j,k;
  V3 pts[8];

  for(i=0;i<2;i++)
    for(j=0;j<2;j++) 
      for(k=0;k<2;k++) {
        p=&pts[4*i+2*j+k];
        if (i==0) p->X=pmin->X; else p->X=pmax->X;
        if (j==0) p->Y=pmin->Y; else p->Y=pmax->Y;
        if (k==0) p->Z=pmin->Z; else p->Z=pmax->Z;
      }
     
  for(i=0;i<2;i++)  
    for(j=0;j<2;j++) {
      glBegin(GL_LINES);

      p=&pts[0*4+i*2+j];
      glVertex3f(p->X,p->Y,p->Z);
      p=&pts[1*4+i*2+j];
      glVertex3f(p->X,p->Y,p->Z);

      p=&pts[i*4+0*2+j];
      glVertex3f(p->X,p->Y,p->Z);
      p=&pts[i*4+1*2+j];
      glVertex3f(p->X,p->Y,p->Z);

      p=&pts[i*4+j*2+0];
      glVertex3f(p->X,p->Y,p->Z);
      p=&pts[i*4+j*2+1];
      glVertex3f(p->X,p->Y,p->Z);

      glEnd();
    }
}

void M4_to_GL(float *gl_matrix,M4 *matrix)
{
  float *m=gl_matrix;
  register int i,j;

  for(i=0;i<4;i++)
    for(j=0;j<4;j++) {
      *m++ = matrix->m[j][i];
    }
} 

void gluPerspective( GLdouble fovy, GLdouble aspect,
		     GLdouble zNear, GLdouble zFar )
{
   GLdouble xmin, xmax, ymin, ymax;

   ymax = zNear * tan( fovy * M_PI / 360.0 );
   ymin = -ymax;

   xmin = ymin * aspect;
   xmax = ymax * aspect;

   glFrustum( xmin, xmax, ymin, ymax, zNear, zFar );
}

static int attributeList[] = { GLX_RGBA, GLX_DOUBLEBUFFER, None };

float light_position0[4]={ -10, -10, 10, 1 };

float mat_specular[] =
{0.628281, 0.628281, 0.628281, 1.0};
float mat_ambient[] =
{0.24725, 0.24725, 0.24725, 1.0};
float mat_diffuse[] =
{0.75164, 0.75164, 0.75164, 1.0};
float mat_shininess[] =
{128.0 * 0.4};

void display(V3 *pts,int size_x,int size_y,
             int wx,int wy,unsigned char *img_map)
{
  Display *dpy;
  XVisualInfo *vi;
  Colormap cmap;
  XSetWindowAttributes swa;
  Window win;
  GLXContext cx;
  char buf[80];
  XEvent xev;
  KeySym keysym;
  XComposeStatus status;

  int cmd;
  M4 rotm;
  float dist;
  V3 pmin,pmax,center;
  float m[16];
  V3 *norms;

  /* get a connection */
  dpy = XOpenDisplay(NULL);
  
  /* get an appropriate visual */
  vi = glXChooseVisual(dpy, DefaultScreen(dpy), attributeList);

  /* create a GLX context */
  cx = glXCreateContext(dpy, vi, 0, GL_TRUE);

  /* create a color map */
  cmap = XCreateColormap(dpy, RootWindow(dpy, vi->screen),
			 vi->visual, AllocNone);

  /* create a window */
  swa.colormap = cmap;
  swa.border_pixel = 0;
  swa.event_mask = StructureNotifyMask;
  win = XCreateWindow(dpy, RootWindow(dpy, vi->screen), 0, 0, wx, wy,
		      0, vi->depth, InputOutput, vi->visual,
		      CWBorderPixel|CWColormap|CWEventMask, &swa);
  XMapWindow(dpy, win);
  /* Wait for map. */
  while(1) {
    XNextEvent(dpy, &xev);
    if(xev.type == MapNotify && xev.xmap.event == win) break;
  }

  XSelectInput(dpy, win, KeyPressMask | KeyReleaseMask);
  XAutoRepeatOff(dpy);

  /* connect the context to the window */
  glXMakeCurrent(dpy, win, cx);

  /* init */

  glEnable(GL_DEPTH_TEST);
  glEnable(GL_NORMALIZE);

  /* camera */
  glViewport(0, 0, wx, wy);
  glMatrixMode(GL_PROJECTION);
  glLoadIdentity();
  gluPerspective(65.0, (GLfloat) wx / (GLfloat) wy, 0.5, 20.0);



  norms=malloc( sizeof(V3) * size_x * size_y);
  
  compute_normals(norms,pts,size_x,size_y);

  if (img_map != NULL) {
    /* build the texture */

    int i,sx,sy,size;
    unsigned char *tex;
    sx=256;
    sy=256;
    size=sx * sy; 
    tex=malloc(3 * size);
    for(i=0;i<size;i++) {
      tex[3*i]=img_map[i];
      tex[3*i+1]=img_map[i];
      tex[3*i+2]=img_map[i];
    }
    /* we use RGB texture for TinyGL */
    glTexImage2D(GL_TEXTURE_2D, 0, 3, sx, sy, 0,
                 GL_RGB, GL_UNSIGNED_BYTE, tex);

    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
  /* Setup texturing */
    glTexEnvi(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_DECAL);
    glHint(GL_PERSPECTIVE_CORRECTION_HINT, GL_FASTEST);

  }

  /* camera matrix */

  /* compute the bounding box */
  GetCenter(&center,&pmin,&pmax,pts,size_x*size_y);

  IdM4(&rotm);
  cmd=0;
  dist=1.0;
  while (1) {
    glClearColor(0.0, 0.0, 0.0, 0.0);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();

    glTranslatef(0,0,-dist);

    M4_to_GL(m,&rotm);
    glMultMatrixf(m);

    /* lightening */
    glEnable(GL_LIGHT0);
    glEnable(GL_LIGHTING);
    glLightfv(GL_LIGHT0, GL_POSITION, light_position0);

    glScalef(1.0/(pmax.X-pmin.X),1.0/(pmax.Y-pmin.Y),1.0/(pmax.Z-pmin.Z));
    glTranslatef(-center.X,-center.Y,-center.Z);

    glMaterialfv(GL_FRONT, GL_SPECULAR, mat_specular);
    glMaterialfv(GL_FRONT, GL_SHININESS, mat_shininess);
    glMaterialfv(GL_FRONT, GL_AMBIENT, mat_ambient);
    glMaterialfv(GL_FRONT, GL_DIFFUSE, mat_diffuse);

    if (img_map != NULL) {
      glEnable(GL_TEXTURE_2D);
    }
    glColor3f(1,1,1);
    drawSurfacePoly(pts,norms,size_x,size_y);

    glDisable(GL_TEXTURE_2D);
    glDisable(GL_LIGHTING);
    glColor3f(0,0,1);
    drawCube(&pmin,&pmax);

    glXSwapBuffers(dpy,win);

    if (XPending(dpy) > 0) {
      XNextEvent(dpy,&xev);
             switch(xev.type) {
             case KeyPress:
               XLookupString((XKeyEvent *)&xev,buf,80,&keysym,&status);
               switch (keysym) {
               case XK_q:
                 goto the_end;
                 
                 
               case XK_Page_Up:
               case XK_Page_Down:
               case XK_Left:
               case XK_Right:
               case XK_Up:
               case XK_Down:
               case XK_Home:
               case XK_End:
                 cmd=keysym;
                 break;
               }
               break;
             case KeyRelease:
               cmd=0;
               break;
             }
    }
           
    switch (cmd) {
           case XK_Left:
             rotm=MulM4(RotateM4(2*M_PI / 64,1),rotm);
             break;
           case XK_Right:
             rotm=MulM4(RotateM4(-2*M_PI / 64,1),rotm);
             break;
           case XK_Up:
             rotm=MulM4(RotateM4(2*M_PI / 64,0),rotm);
             break;
           case XK_Down:
             rotm=MulM4(RotateM4(-2*M_PI / 64,0),rotm);
             break;
           case XK_Page_Up:
             rotm=MulM4(RotateM4(-2*M_PI / 64,2),rotm);
             break;
           case XK_Page_Down:
             rotm=MulM4(RotateM4(2*M_PI / 64,2),rotm);
             break;
           case XK_Home:
             dist/=1.1;
             break;
           case XK_End:
             dist*=1.1;
             break;
    }
           
  }
	 
the_end:	 
  XAutoRepeatOn(dpy);
  XFlush(dpy);
}

