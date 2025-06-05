#include <stdio.h>
#include <stdlib.h>
#include <GL/freeglut.h>
#include <math.h>
/*
#define PI 3.1415926535
#define PI2 PI/2
#define PI3 3*PI/2 */

float px, py, pdx, pdy, pa; //posição do jogador

//funções para usar angulo
float degToRad(int a) 
{ 
    return (a*M_PI/180.0);
}
int FixAng(int a)
{ 
    if(a>359)
        a-=360; 
    if(a<0)
        a+=360;
    return a;
}

void drawPlayer()
{
    glColor3f(1,1,0);
    glPointSize(8);
    glBegin(GL_POINTS);
    glVertex2i(px, py);
    glEnd();

    glLineWidth(3);
    glBegin(GL_LINES);
    glVertex2i(px, py);
    glVertex2i(px+pdx*5, py+pdy*5);
    glEnd();
};

int mapX=8, mapY=8, mapS=64;
int map[] =
{
    1,1,1,1,1,1,1,1,
    1,0,0,0,0,0,0,1,
    1,0,0,0,0,0,0,1,
    1,0,0,1,1,0,0,1,
    1,0,0,1,1,0,0,1,
    1,0,0,0,0,0,0,1,
    1,0,0,0,0,0,0,1,
    1,1,1,1,1,1,1,1,
};

void drawMap2D()
{
    int x, y , xo, yo;
    for(y=0; y<mapY; y++)
    {
        for(x=0; x<mapX; x++)
        {
            if(map[y*mapX+x]==1)
            {
                glColor3f(1,1,1);
            }
            else
            {
                glColor3f(0,0,0);
            }
            xo = x*mapS;
            yo = y*mapS;
            glBegin(GL_QUADS);
            glVertex2i(xo + 1, yo + 1);
            glVertex2i(xo + 1, yo+mapS-1);
            glVertex2i(xo+mapS-1, yo+mapS-1);
            glVertex2i(xo+mapS-1, yo+1);
            glEnd();
        }
    }
}

//calculo de distancia 
/*
float distance(ax,ay,bx,by,ang)
{ 
    return (cos(degToRad(ang))*(bx-ax)-sin(degToRad(ang))*(by-ay));
} */

void drawRays2D()
{ 
    //desenhar o ceu
    glColor3f(0,1,1); 
    glBegin(GL_QUADS); 
    glVertex2i(526,  0); 
    glVertex2i(1006,  0); 
    glVertex2i(1006,160); 
    glVertex2i(526,160); 
    glEnd();	
    //desenhar o chao
    glColor3f(0,0,1); 
    glBegin(GL_QUADS); 
    glVertex2i(526,160); 
    glVertex2i(1006,160); 
    glVertex2i(1006,320); 
    glVertex2i(526,320); 
    glEnd();	 	
	
    int r,mx,my,mp,dof,side; 
    float vx,vy,rx,ry,ra,xo,yo,disV,disH; 
 
    ra=FixAng(pa+40);
 
    for(r=0;r<60;r++)
    {
    //Checa por linhas verticais
        dof=0; side=0; disV=100000;
        float Tan=tan(degToRad(ra));
        if(cos(degToRad(ra))> 0.001)
        { 
            rx=(((int)px>>6)<<6)+64;
            ry=(px-rx)*Tan+py; 
            xo= 64; yo=-xo*Tan;
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

            if(mp>0 && mp<mapX*mapY && map[mp]==1)
            { 
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

            if(mp>0 && mp<mapX*mapY && map[mp]==1)
            { 
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
        
        glColor3f(0,0.8,0);
        if(disV<disH)
        { 
            rx=vx; 
            ry=vy; 
            disH=disV; 
            glColor3f(0,0.6,0);
        } //Horizontal mais próxima

        glLineWidth(2); 
        glBegin(GL_LINES); 
        glVertex2i(px,py); 
        glVertex2i(rx,ry); 
        glEnd();//Desenha ray 2d
            
        int ca=FixAng(pa-ra); 
        disH=disH*cos(degToRad(ca));  //Consertar fisheye
        int lineH = (mapS*320)/(disH); 
        if(lineH>320)
            lineH=320;                   //Altura e limite
        int lineOff = 160 - (lineH>>1);  //offset para ficar na altura certa da tela
        
        //Desenha a parede vertical
        glLineWidth(8);
        glBegin(GL_LINES);
        glVertex2i(r*8+530,lineOff);
        glVertex2i(r*8+530,lineOff+lineH);
        glEnd();

        ra=FixAng(ra-1); //vai para o proximo ray
    }
}


void init()
{
    glClearColor(0.3,0.3,0.3,0);
    gluOrtho2D(0,1024,510,0);
    px=150; 
    py=400; 
    pa=90;
    pdx=cos(degToRad(pa)); 
    pdy=-sin(degToRad(pa)); 
}

void display()
{
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    drawMap2D();
    drawPlayer();
    drawRays2D();
    glutSwapBuffers();
};

void redimensiona (int w, int h)
{
    glutReshapeWindow(1024,512);
}

void buttons(unsigned char key, int x, int y)
{
    switch(key)
    {
        case 'a':
        { pa+=5; pa=FixAng(pa);
         pdx=cos(degToRad(pa));
          pdy=-sin(degToRad(pa));
        }
        break;

        case 'd':
        { 
            pa-=5; pa=FixAng(pa);
            pdx=cos(degToRad(pa));
            pdy=-sin(degToRad(pa));
        }
        break;

        case 'w':
        { 
            px+=pdx*5; 
            py+=pdy*5;
        }
        break;

        case 's':
        { 
            px-=pdx*5; 
            py-=pdy*5;
        }
        break;
    }
    glutPostRedisplay();
};

int main(int argc, char* argv[])
{
    glutInit(&argc, argv);
    glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGBA);
    glutInitWindowSize(1024, 512);
    glutInitWindowPosition(200,200);
    glutCreateWindow("");
    init();
    glutDisplayFunc(display);
    glutReshapeFunc(redimensiona);
    glutKeyboardFunc(buttons);
    glutMainLoop();
}
//g++ -o main.exe main.cpp -I"./include" -L"./lib/x64" -lfreeglut -lopengl32 -lglu32
//g++ -o main.exe main.cpp -I"./include" -L"./lib/x64" -lopengl32 -lglu32 -lglut32