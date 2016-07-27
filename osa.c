
#include <math.h>
#include <GL/gl.h>
#include <stdlib.h>

// playerin laskuri
extern int counter;

// virkistystaajuus max / Hz
#define RATE 75.0

// parametrien muutosnopeus 0-1
#define PARAMFACTOR 0.03
#define PARAMMAX 5.0

enum {
    TENSION=1,
    CONTINUITY,
    BIAS,
    COLORF,
    TRANSLATEZ,
    ROTX,
    RZSPD,
    TAUSTA

};
// muuttaa flotarin kiinte�n pisteen luvuksi
#define F(f) ((f/PARAMMAX)*128)
// muunnos takaisin
#define FL(i) ((float)i*PARAMMAX/128.0)
#define NOF_PARAMS 9

// paramaterit:
// tahti tension continuity bias v�rikerroin zpos zrot rzspd tausta
signed char param[]={
    0, F(4.99),  F(3.99),F(0.0),  F(0.0),F(-0.2),F(0.6),F(0.01),F(0.9),
    8, F(1.99),  F(3.99),F(2.0),  F(0.0),F(-1.3),F(1.3),F(0.01),F(0.9),
    12, F(1.99),  F(3.99),F(2.0),  F(0.2),F(-0.3),F(-0.6),F(0.01),F(0.9),
    16,F(0.0),  F(4.99),  F(2.0),  F(1.0),F(-0.7),F(2.5),F(0.5),F(1.0),
    24,F(0.0),  F(-4.4),  F(0.0),  F(0.6),F(-0.4),F(1.5),F(-0.7),F(1.0),
    32,F(1.00),  F(2.00),  F(1.4),  F(1.0),F(-0.5),F(0.5),F(0.0),F(0.9),
    34,F(0.0),  F(4.99),  F(2.0),  F(1.0),F(-0.7),F(0.5),F(1.0),F(1.5),
    36,F(0.0),  F(4.99),  F(2.0),  F(1.0),F(-0.7),F(-1.0),F(1.0),F(1.5),
    40,F(1.00),  F(2.00),  F(1.4),  F(1.0),F(-0.4),F(0.0),F(1.0),F(0.0),
    44,F(-1.00),  F(-2.00),  F(0.0),  F(1.0),F(-0.4),F(0.0),F(1.5),F(0.0),
    46,F(-1.00),  F(-2.00),  F(0.0),  F(1.0),F(-0.3),F(0.0),F(4.5),F(0.0),
    47,F(-1.00),  F(-2.00),  F(0.0),  F(1.0),F(-0.2),F(0.5),F(2.5),F(0.0),
    48,F(0.000),  F(0.000),  F(0.0),  F(1.0),F(-0.7),F(2.5),F(0.2),F(1.0),
    56,F(0.000),  F(0.000),  F(0.0),  F(0.0),F(-2.9),F(-3.5),F(2.2),F(1.0),
    57,F(0.000),  F(0.000),  F(0.0),  F(0.0),F(0.9),F(-3.5),F(2.2),F(1.0),
    58,F(0.000),  F(0.000),  F(0.0),  F(0.0),F(0.9),F(-3.5),F(2.2),F(0.0),
    59,F(0.000),  F(0.000),  F(0.0),  F(0.0),F(4.9),F(-3.5),F(2.2),F(0.0),
};

float paramf[NOF_PARAMS+1]={0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0};

int beat;
#define tension (paramf[TENSION])
#define continuity (paramf[CONTINUITY])
#define bias (paramf[BIAS])
#define color_f (paramf[COLORF])

typedef float Vect[3];
Vect kukka[7]={
    {0.0,0.5,0.0},
    {0.0,0.0,0.0},
    {0.1,-0.2,0.0},
    {0.3,-0.3,0.0},
    {0.5,-0.3,0.0},
    {0.6,-0.2,0.0},
    {0.6,-0.5,0.0}};

Vect kukka_morph[7]={
    {0.0,0.5,0.0},
    {0.0,0.0,0.0},
    {0.2,-0.2,0.0},
    {0.3,-0.15,0.0},
    {0.2,0.0,0.0},
    {0.1,0.0,0.0},
    {0.0,0.1,0.0}};

#ifndef BLKSIZE
#define BLKSIZE 1024
#endif
float buf[BLKSIZE];
#define NBANDS 32

#define FACTOR 0.35
float max_sample;
void analyysi(signed short *dest) {
    int i;
    float w,max=0;

    for (i=0; i<BLKSIZE; i++) {
	w=dest[i*2]/32768.0;
	buf[i]=w/5.0;
	if (fabs(w)>=max)
	    max=fabs(w);
    }

    max_sample=max*FACTOR+(1.0-FACTOR)*max_sample;
    if (beat>=48)
        max_sample=0.0;
}

void splineTCP(float u, Vect * control, Vect * result)
{
    // u on sijainti kontrollipisteiden 1 ja 2 v�lill� (0..1)
    // control pointteri kontrollipisteisiin
    // *x, *y, *z pointterit lopputulokselle
    int n;

    for(n=0;n<2;n++)
        (*result)[n] = (2*u*u*u - 3*u*u + 1)*control[1][n]
	 + (-2*u*u*u + 3*u*u)*control[2][n]
	 + (u*u*u - 2*u*u + u)*(0.5*(1-tension)*((1+bias)*(1-continuity)*(control[1][n]-control[0][n])
						+ (1-bias)*(1+continuity)*(control[2][n]-control[1][n])))
	 + (u*u*(u-1))*(0.5*(1-tension)*((1+bias)*(1+continuity)*(control[2][n]-control[1][n])
					  + (1-bias)*(1-continuity)*(control[3][n]-control[2][n])));
}
float timef;

void spline3DMorph(float factor)
{
#define NBTW 16
    int i,j,k,c;
    static Vect r,r_morph,n;
    static float rf[17*NBTW*3];

    c=0;
    for (i=0; i<sizeof(kukka)/sizeof(Vect)-3; i++) {
	for (j=0; j<NBTW; j++) {
	    splineTCP((float)j/NBTW, &kukka[i], &r);
	    splineTCP((float)j/NBTW, &kukka_morph[i], &r_morph);
	    for (k=0; k<3; k++)
		rf[c*3+k]=(1.0-factor)*r[k]+factor*r_morph[k];

	    rf[c*3+2]=sin(c*M_PI/64)*0.07;
	    c++;
	}
    }
    for (i=0; i<(c-1); i++) {
        n[0]=(rf[(i+1)*3+2]-rf[i*3+2]);
	n[1]=-(rf[(i+1)*3]-rf[i*3]);
	n[2]=rf[(i+1)*3+1]-rf[i*3+1];

	rf[i*3+1]+=sin(i*0.5+timef*0.006)*max_sample*(0.006*(1.0+i*0.6));
	rf[i*3+2]+=sin(i*0.05+timef*0.002)*max_sample*(0.001*(1.0+i*1.0));

	glEnable(GL_LIGHTING);
        glEnable(GL_NORMALIZE);
        glEnable(GL_DEPTH_TEST);
	glColor3f(color_f*0.82,color_f*0.87,color_f*0.95);
        glNormal3fv((GLfloat *)n);

	glBegin(GL_POLYGON);
	glVertex3f(1.0*rf[i*3],-rf[i*3+2],rf[i*3+1]*1.1);
	glVertex3f(1.0*rf[i*3+3],-rf[i*3+3+2],rf[i*3+3+1]*1.1);
	glVertex3f(1.0*rf[i*3+3],+rf[i*3+3+2],rf[i*3+3+1]*1.1);
	glVertex3f(1.0*rf[i*3],+rf[i*3+2],rf[i*3+1]*1.1);
	glEnd();

	glColor3f(0.0,0.0,0.0);
        glDisable(GL_LIGHTING);

	glBegin(GL_LINES);
	glVertex3f(1.0*rf[i*3],-rf[i*3+2],rf[i*3+1]*1.1);
	glVertex3f(1.0*rf[i*3+3],-rf[i*3+3+2],rf[i*3+3+1]*1.1);
	glVertex3f(1.0*rf[i*3+3],+rf[i*3+3+2],rf[i*3+3+1]*1.1);
	glVertex3f(1.0*rf[i*3],+rf[i*3+2],rf[i*3+1]*1.1);
	glEnd();
    }
}

extern char quit;

int osa(int time)
{
    int t;
    float f;
    //static int kelkka=0;
    int new_frame;
    static int old_frame=1000;
    static GLfloat light_ambient[]={0.2,0.2,0.2,0.0f};
    static float rz=0.0;
    int i,j,n;

    timef=time;
    beat=((float)(counter/44100.0)*(125.0/60.0)-0.3)/4.0;

    if (beat>=59) {
        quit=1;
    }
    /* Tsekkaa, ettei tulosteta yli frameraten.
     Parametrien aikablurraus vaatii t�m�n.*/
    new_frame=(int)((time/1000.0)*RATE);
    if (new_frame==old_frame)
	return(-1);
    old_frame=new_frame;

    for (i=0; i< sizeof(param)/NOF_PARAMS-1 ; i++) {
	if (beat<param[(i+1)*NOF_PARAMS]) {
	    for (j=1; j<=NOF_PARAMS; j++)
		paramf[j]=
		    (1.0-PARAMFACTOR)*paramf[j]+
                    PARAMFACTOR*param[i*NOF_PARAMS+j]*PARAMMAX/128.0;
	    break;
	}
    }
    f=paramf[TAUSTA];
    glClearColor(f,f,f,1.0);
    glClear(GL_COLOR_BUFFER_BIT|GL_DEPTH_BUFFER_BIT);

    /* InitViewPort */
    glMatrixMode (GL_PROJECTION);
    glLoadIdentity ();
    glFrustum(-0.01,0.01, -0.0075,0.0075, 0.01,1135.0);

    glMatrixMode (GL_MODELVIEW);
    glLoadIdentity ();
    glTranslatef(0.0,0.0,(paramf[TRANSLATEZ]));

    glRotatef(-(paramf[ROTX])*90.0/5.0,1.0,0.0,0.0);
    rz+=paramf[RZSPD]*1.50;
    glRotatef(rz,0.0,0.0,1.0);

    /* aaltoMuoto */
    if (beat<48) {
	glDisable(GL_LIGHTING);
	glLineWidth(3.0);

        for(n=0;n<2;n++)
        {
            glBegin(GL_LINE_STRIP);
            for (i=0; i<BLKSIZE; i++) {
	        f=1.0-sin(i*M_PI/BLKSIZE)*1.0;
	        glColor3f(f,f,f);
                glVertex3f(2.0*((float)i/BLKSIZE-0.5),buf[i],-0.3);
            }
	    glEnd();
            glRotatef(90,0,0,1);
        }
    }

    glLightfv(GL_LIGHT0, GL_AMBIENT, light_ambient);
    glEnable(GL_LIGHT0);

    glEnable(GL_COLOR_MATERIAL);
    glRotatef(360.0/24.0,0.0,0.0,1.0);
    for (t=0; t<12; t++) {
	glRotatef(360.0/12.0,0.0,0.0,1.0);
	spline3DMorph(sin(time*0.001)*0.5+0.5);
    }
    return(0);
}
