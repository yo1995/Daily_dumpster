#
# Modify this Makefile according to your need. You may have to modify 
# the OpenGL library path.
#
PROG = correl
OBJS = correl.o view.o pnm.o getopt.o getopt1.o graph2d.o zmath.o
CFLAGS = -g -Wall -O2 -std=c99
LIBS = -lm -L/usr/X11/lib -lX11 -lGL -lXext
LFLAGS =

all: $(PROG) rectify

clean:
	rm -f *.o *~

rectify: rectify.o getopt.o getopt1.o zmath.o
	gcc -g rectify.o getopt.o getopt1.o zmath.o -o rectify -lm

$(PROG): $(OBJS)
	gcc -g $(OBJS) $(LFLAGS) -o $(PROG) $(LIBS)

distrib:
	( cd .. ; tar zcvf stereovision-1.00.tgz \
          stereovision/README stereovision/COPYING \
          stereovision/*.c stereovision/*.h stereovision/Makefile \
          stereovision/doc/stereo_slides.ps \
          stereovision/herve/test_herve stereovision/herve/tdir* \
	  stereovision/herve/herve.*.pgm \
          stereovision/buste/hop_10.rect* stereovision/buste/hop_10.c* \
	  stereovision/buste/test_buste stereovision/buste/proj* )

.c.o:
	gcc	$(CFLAGS) -c $*.c

