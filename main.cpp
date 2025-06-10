//#include <stdlib.h>
#include <GL/freeglut.h>
#include <math.h>

#include "Texturas/AllTextures.ppm"
#include "Texturas/ceu.ppm"
#include "Texturas/inicio.ppm"
#include "Texturas/sprites.ppm"

typedef struct 
{
    int w, a, s, d, enter;
}Teclado;
Teclado Botoes;

float px, py, pdx, pdy, pa; //posição do jogador

int gameState=0;

//funções para usar angulo
float degToRad(float a) 
{ 
    return (a*M_PI/180.0);
}
float FixAng(float a)
{ 
    if(a>359)
        a-=360; 
    if(a<0)
        a+=360;
    return a;
}

int mapX=8, mapY=8, mapS=64;
int map[] = //mapa das paredes
{
    1,1,2,1,1,1,1,1,
    1,0,1,0,0,0,0,1,
    1,0,1,0,0,0,0,1,
    1,0,1,0,0,0,0,1,
    1,0,0,0,1,0,0,1,
    1,0,0,0,0,0,0,1,
    1,0,0,0,0,0,0,1,
    1,1,1,1,1,1,1,1
};

int mapC[] = //mapa do chao
{
    0,0,0,0,0,0,0,0,
    0,0,0,0,0,0,0,0,
    0,0,2,2,2,2,2,0,
    0,0,0,0,0,0,2,0,
    0,0,0,0,0,0,2,0,
    0,0,0,0,0,0,2,0,
    0,0,0,0,0,0,0,0,
    0,0,0,0,0,0,0,0
};

int mapT[] = //mapa do teto
{
    0,0,0,0,0,0,0,0,
    0,0,0,0,0,0,0,0,
    0,0,0,0,0,0,0,0,
    0,0,0,0,0,0,0,0,
    0,0,0,0,0,0,0,0,
    0,0,0,0,0,0,0,0,
    0,0,0,0,0,0,0,0,
    0,0,0,0,0,0,0,0
};

typedef struct
{
    int type;
    int state;
    int map;
    int x, y, z;
}sprite; sprite sp[4];
int depth[120];

void drawSprite()
{
    for(int s=0;s<8; s++)
    {
    float sx = sp[0].x - px;
    float sy = sp[0].y - py;
    float sz = sp[0].z;

    float CS = cos(degToRad(pa)), SN=sin(degToRad(pa));
    float a = sy*CS+sx*SN;
    float b = sx*CS-sy*SN;
    sx = a;
    sy = b;

    sx=(sx*108.0/sy)+(120/2);
    sy=(sz*108.0/sy)+(80/2);

    int x, y;
    int scale=32*80/b;
    if(scale<0)
    {
        scale=0;
    }
    if(scale>120)
    {
        scale=120;
    }

    //textura
    float tx=0, ty = 0, txstep = 32.0/(float)scale, tystep = 32.0/(float)scale;

    for(x=sx-scale/2; x<sx+scale/2; x++)
    {
        for(y=0; y<scale; y++)
        {
            ty = 0;
            if(sx>0 && sx<120 && b<depth[(int)sx])
            {
                int pixel=((int)ty*32 + (int)tx)*3;
                int red   =sprites[pixel+0];
                int green =sprites[pixel+1];
                int blue  =sprites[pixel+2];
                glPointSize(8);
                glColor3ub(red, green, blue);
                glBegin(GL_POINTS);
                glVertex2i(x*8, sy*8-y*8);
                glEnd();
                ty+= tystep;
            }
        }
        tx +=txstep;
    }
    }
}


void drawMap2D()
{
    int x, y , xo, yo;
    int tmapS=mapS/4;

    for(y=0; y<mapY; y++)
    {
        for(x=0; x<mapX; x++)
        {
            //desenho do fundo cinza do minimapa
            xo = x*tmapS;
            yo = y*tmapS;
            glColor3f(0.4,0.4,0.4);
            glBegin(GL_QUADS);
            glVertex2i(xo, yo);
            glVertex2i(xo, yo+tmapS);
            glVertex2i(xo+tmapS, yo+tmapS);
            glVertex2i(xo+tmapS, yo);
            glEnd();

            //desenho das paredes do minimapa
            if(map[y*mapX+x]>0)
            {
                glColor3f(1,1,1);
            }
            else
            {
                glColor3f(0,0,0);
            }
            glBegin(GL_QUADS);
            glVertex2i(xo + 1, yo + 1);
            glVertex2i(xo + 1, yo+tmapS-1);
            glVertex2i(xo+tmapS-1, yo+tmapS-1);
            glVertex2i(xo+tmapS-1, yo+1);
            glEnd();

            
        }
    }
}


void drawPlayer()
{
    int tpx=px/4, tpy=py/4;
    glColor3f(1,1,0);
    glPointSize(8);
    glBegin(GL_POINTS);
    glVertex2i(tpx, tpy);
    glEnd();

    glLineWidth(3);
    glBegin(GL_LINES);
    glVertex2i(tpx, tpy);
    glVertex2i(tpx+pdx*10, tpy+pdy*10);
    glEnd();
};


void drawRays2D()
{ 
    int r,mx,my,mp,dof,side; 
    float vx,vy,rx,ry,ra,xo,yo,disV,disH; 
 
    ra=FixAng(pa+30);
 
    for(r=0;r<120;r++)
    {
        int vmt=0,hmt=0;   
    //Checa por linhas verticais
        dof=0; side=0; disV=100000;
        float Tan=tan(degToRad(ra));
        if(cos(degToRad(ra))> 0.001)
        { 
            rx=(((int)px>>6)<<6)+64;
            ry=(px-rx)*Tan+py; 
            xo= 64; 
            yo=-xo*Tan;
        } //Checa se está olhando para esquerda
        else 
            if(cos(degToRad(ra))<-0.001)
            { 
                rx=(((int)px>>6)<<6) -0.0001; 
                ry=(px-rx)*Tan+py; 
                xo=-64; 
                yo=-xo*Tan;
            }//Checa se está olhando para direita
             else 
             { 
                rx=px; 
                ry=py; 
                dof=8;
            }//Olhando para cima ou para baixo

        while(dof<8) 
        { 
            mx=(int)(rx)>>6; 
            my=(int)(ry)>>6; 
            mp=my*mapX+mx;

            if(mp>0 && mp<mapX*mapY && map[mp]>0)
            { 
                vmt=map[mp]-1;
                dof=8; 
                disV=cos(degToRad(ra))*(rx-px)-sin(degToRad(ra))*(ry-py);
            }        
            else
            { 
                rx+=xo; 
                ry+=yo; 
                dof+=1;
            } //Checa a proxima horizontal
        } 
        vx=rx; vy=ry;

        //Checa por linhas horizontais
        dof=0; 
        disH=100000;
        Tan=1.0/Tan; 
        if(sin(degToRad(ra))> 0.001)
        { 
            ry=(((int)py>>6)<<6) -0.0001; 
            rx=(py-ry)*Tan+px; 
            yo=-64; 
            xo=-yo*Tan;
        }//Olhando para cima
        else 
            if(sin(degToRad(ra))<-0.001)
            { 
                ry=(((int)py>>6)<<6)+64;      
                rx=(py-ry)*Tan+px; 
                yo= 64; 
                xo=-yo*Tan;
            }//Para baixo
            else
            { 
                rx=px; 
                ry=py; 
                dof=8;
            }//Esquerda ou Direita
        
        while(dof<8) 
        { 
            mx=(int)(rx)>>6; 
            my=(int)(ry)>>6; 
            mp=my*mapX+mx;                     

            if(mp>0 && mp<mapX*mapY && map[mp]>0)
            { 
                hmt=map[mp]-1;
                dof=8; 
                disH=cos(degToRad(ra))*(rx-px)-sin(degToRad(ra))*(ry-py);
            }
            else
            { 
                rx+=xo; 
                ry+=yo; 
                dof+=1;
            }   //Checa a próxima horizontal
        } 
        float shade=1;
        glColor3f(0,0.8,0);
        if(disV<disH)
        { 
            hmt=vmt; 
            shade=0.5; 
            rx=vx; 
            ry=vy; 
            disH=disV; 
            glColor3f(0,0.6,0);
        }

        /*
        float pxt=px/4, pyt=py/4, rxt=rx/4, ryt=ry/4;
        glLineWidth(2); 
        glBegin(GL_LINES); 
        glVertex2i(pxt,pyt); 
        glVertex2i(rxt,ryt); 
        glEnd();//Desenha ray 2d
        */
            
        int ca=FixAng(pa-ra); 
        disH=disH*cos(degToRad(ca));  //Consertar fisheye
        int lineH = (mapS*640)/(disH); 
        float ty_step=32.0/(float)lineH; 
        float ty_off=0;
        if(lineH>640)
        { 
            ty_off=(lineH-640)/2.0; 
            lineH=640;
        }                            //line height and limit
        int lineOff = 320 - (lineH>>1);    //offset para ficar na altura certa da tela
        
        depth[r] = disH;
        //Desenha a parede vertical
        glLineWidth(8);
        glBegin(GL_LINES);
        glVertex2i(r*8/*+530*/,lineOff);
        glVertex2i(r*8/*+530*/,lineOff+lineH);
        glEnd();

        //Desenhar paredes
        int y;
        float ty=ty_off*ty_step/*+hmt*32*/, tx;

        if(shade==1)
        { 
            tx=(int)(rx/2.0)%32; 
            if(ra>180) 
                tx=31-tx;
        }  
        else        
        { 
            tx=(int)(ry/2.0)%32; 
            if(ra>90 && ra<270) 
                tx=31-tx;
        }

        for(y=0;y<lineH;y++)
        {
            int pixel=((int)ty*32+(int)tx)*3+(hmt*32*32*3);
            int red   =AllTextures[pixel+0]*shade;
            int green =AllTextures[pixel+1]*shade;
            int blue  =AllTextures[pixel+2]*shade;
            glPointSize(8); 
            glColor3ub(red,green,blue); 
            glBegin(GL_POINTS); 
            glVertex2i(r*8/*+530*/,y+lineOff); 
            glEnd();
            ty+=ty_step;
        }

          //---draw floors---
        for(y=lineOff+lineH;y<640;y++)
        {
            float dy=y-(640/2.0), deg=degToRad(ra), raFix=cos(degToRad(FixAng(pa-ra)));
            tx=px/2 + cos(deg)*316*32/dy/raFix;
            ty=py/2 - sin(deg)*316*32/dy/raFix;
            int mp=mapC[(int)(ty/32.0)*mapX+(int)(tx/32.0)]*32*32;
            int pixel=(((int)(ty)&31)*32 + ((int)(tx)&31))*3+mp*3;
            int red   =AllTextures[pixel+0]*0.7;
            int green =AllTextures[pixel+1]*0.7;
            int blue  =AllTextures[pixel+2]*0.7;

            glColor3ub(red,green,blue);
            glPointSize(8);
            glBegin(GL_POINTS);
            glVertex2i(r*8/*+530*/,y);
            glEnd();

            //---draw ceiling---
            mp=mapT[(int)(ty/32.0)*mapX+(int)(tx/32.0)]*32*32;
            pixel=(((int)(ty)&31)*32 + ((int)(tx)&31))*3+mp*3;
            red   =AllTextures[pixel+0];
            green =AllTextures[pixel+1];
            blue  =AllTextures[pixel+2];
            if(mp>0)
            { 
                glPointSize(8); 
                glColor3ub(red,green,blue); 
                glBegin(GL_POINTS); 
                glVertex2i(r*8/*+530*/,640-y); 
                glEnd();
            }
        }

        ra=FixAng(ra-0.5); //vai para o proximo ray
    }
}

void drawSky()     //draw sky and rotate based on player rotation
{
    int x,y;
    for(y=0;y<40;y++)
    {
        for(x=0;x<120;x++)
        {
            int xo=(int)pa*2-x; if(xo<0){ xo+=120;} xo=xo % 120; //return 0-120 based on player angle
            int pixel=(y*120+xo)*3;
            int red   =ceu[pixel+0];
            int green =ceu[pixel+1];
            int blue  =ceu[pixel+2];
            glPointSize(8); glColor3ub(red,green,blue); glBegin(GL_POINTS); glVertex2i(x*8/*+530*/,y*8); glEnd();
        }	
    }
}

void tela(int s)
{
    int x, y;
    int *T;
    s = 0;
    if(s==0)
    T = inicio;

    for(y=0;y<90;y++)
    {
        for(x=0;x<120;x++)
        {
            int pixel=(y*120+x)*3;
            int red = T[pixel];
            int green = T[pixel+1];
            int blue = T[pixel+2];
            
            glPointSize(8);
            glColor3ub(red, green, blue);
            glBegin(GL_POINTS);
            glVertex2i(x*8, y*8);
        }
    }
}

void init()
{
    glClearColor(0.3,0.3,0.3,0);
    px=150; 
    py=400; 
    pa=90;
    pdx=cos(degToRad(pa)); 
    pdy=-sin(degToRad(pa)); 
    sp[0].type=1;
    sp[0].state=1;
    sp[0].map=0;
    sp[0].x = 3*64;
    sp[0].y = 6*64;
    sp[0].z = 20;

}

float frame1=0, frame2, fps;

void display()
{
    if(gameState == 0)
    {
        tela(gameState);
    }

    if(gameState == 1)
    {
        frame2 = glutGet(GLUT_ELAPSED_TIME);
        fps = (frame2 - frame1);
        frame1 = glutGet(GLUT_ELAPSED_TIME);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        if(Botoes.a == 1)
        { 
            pa+=0.1*fps; pa=FixAng(pa);
            pdx=cos(degToRad(pa));
            pdy=-sin(degToRad(pa));
        }

        if(Botoes.d == 1)
        { 
            pa-=0.1*fps; pa=FixAng(pa);
            pdx=cos(degToRad(pa));
            pdy=-sin(degToRad(pa));
        }

        int xo = 0, yo = 0;
        
        if(pdx<0)
            xo = -20;
        else
            xo = 20; 

        if (pdy<0)
            yo = -20;
        else
            yo = 20;

        int  ipx=px/64, ipxoA=(px+xo)/64, ipxoS=(px-xo)/64, ipy=py/64, ipyoA=(py+yo)/64, ipyoS=(py-yo)/64;

        if(Botoes.w == 1)
        { 
            if(map[ipy*mapX+ipxoA]==0) //verificação de parede
                px+=pdx*0.1*fps; 

            if(map[ipyoA*mapX+ipx]==0) //verificação de parede
                py+=pdy*0.1*fps;
        }

        if(Botoes.s == 1)
        { 
            if(map[ipy*mapX+ipxoS]==0) //verificação de parede
                px-=pdx*0.1*fps;  

            if(map[ipyoS*mapX+ipx]==0) //verificação de parede
                py-=pdy*0.1*fps;
        }

        drawSky();
        drawRays2D();
        drawSprite();
        drawMap2D();
        drawPlayer();

    }
    glutPostRedisplay();
    glutSwapBuffers();
};

void redimensiona (int w, int h)
{
    glutReshapeWindow(960,640);
}

void Pressionado(unsigned char key, int x, int y)
{
    switch(key)
    {
        case 'a':
            Botoes.a = 1;
        break;

        case 'd':
            Botoes.d = 1;
        break;

        case 'w':
            Botoes.w = 1;
        break;

        case 's':
            Botoes.s = 1;
        break;
        
        case 13:
            if(gameState == 0)
                gameState = 1;
        break;
    }

    glutPostRedisplay();
}

void naoPressionado(unsigned char key, int x, int y)
{
    switch(key)
    {
        case 'a':
            Botoes.a = 0;
        break;

        case 'd':
            Botoes.d = 0;
        break;

        case 'w':
            Botoes.w = 0;
        break;

        case 's':
            Botoes.s = 0;
        break;
    }
    glutPostRedisplay();
}

int main(int argc, char* argv[])
{
    glutInit(&argc, argv);
    glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGBA);
    glutInitWindowSize(960, 640);
    glutInitWindowPosition(glutGet(GLUT_SCREEN_WIDTH)/2-480,glutGet(GLUT_SCREEN_HEIGHT)/2-320);
    glutCreateWindow("");
    gluOrtho2D(0,960,640,0);
    init();
    glutDisplayFunc(display);
    glutReshapeFunc(redimensiona);
    glutKeyboardFunc(Pressionado);
    glutKeyboardUpFunc(naoPressionado);
    glutMainLoop();
}
//g++ -o main.exe main.cpp -I"./include" -L"./lib/x64" -lfreeglut -lopengl32 -lglu32
//g++ -o main.exe main.cpp -I"./include" -L"./lib/x64" -lopengl32 -lglu32 -lglut32