/*
 * 2d graphics
 */

int G2D_Init(unsigned char *b,int sx,int sy);
void G2D_Plot(int x,int y,int color);
void G2D_Line(int x1,int y1,int x2,int y2,int color);
void G2D_LineClip(float x1,float y1,float x2,float y2,int color);

void G2D_Resize(unsigned char *img1,int size1_x,int size1_y,
								unsigned char *img2,int size2_x,int size2_y);

/*
 * PNM format
 */


unsigned char *PGM_Load(int *xsize1,int *ysize1,char *filename);
void PGM_Save(unsigned char *buf,int xsize,int ysize,char *filename);
