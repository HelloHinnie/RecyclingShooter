#include <GL/freeglut.h>
#include <iostream>
#include <cmath>




GLfloat angle = 45.0f, fAspect = 1.33f;
GLfloat rotX, rotY, rotX_ini, rotY_ini;
GLfloat obsX, obsY, obsZ, obsX_ini, obsY_ini, obsZ_ini;
GLfloat rotBraco=0.0, rotAnte=0.0, rotMao=0.0;
int x_ini,y_ini,bot;
GLfloat colorR = 227.0f, colorG = 161.0f, colorB = 115.0f;

float skyboxVertices[] =
{
    //Coordenadas
    -1.0f, -1.0f,  1.0f,//        7--------6
	 1.0f, -1.0f,  1.0f,//       /|       /|
	 1.0f, -1.0f, -1.0f,//      4--------5 |
	-1.0f, -1.0f, -1.0f,//      | |      | |
	-1.0f,  1.0f,  1.0f,//      | 3------|-2
	 1.0f,  1.0f,  1.0f,//      |/       |/
	 1.0f,  1.0f, -1.0f,//      0--------1
	-1.0f,  1.0f, -1.0f

}

unsigned int skyboxIndices[] =
{
	// Right
	1, 2, 6,
	6, 5, 1,
	// Left
	0, 4, 7,
	7, 3, 0,
	// Top
	4, 5, 6,
	6, 7, 4,
	// Bottom
	0, 3, 2,
	2, 1, 0,
	// Back
	0, 1, 5,
	5, 4, 0,
	// Front
	3, 7, 6,
	6, 2, 3
};


void desenhaParalelepipedo(float largura, float altura, float profundidade) {
    float w = largura / 2.0f;
    float h = altura / 2.0f;
    float d = profundidade / 2.0f;

    glBegin(GL_QUADS);

    // frente
    glNormal3f(0.0f, 0.0f, 1.0f); // Normal pointing towards positive Z
    glVertex3f(-w, -h,  d);
    glVertex3f( w, -h,  d);
    glVertex3f( w,  h,  d);
    glVertex3f(-w,  h,  d);

    // trás
    glNormal3f(0.0f, 0.0f, -1.0f); // Normal pointing towards negative Z
    glVertex3f(-w, -h, -d);
    glVertex3f(-w,  h, -d);
    glVertex3f( w,  h, -d);
    glVertex3f( w, -h, -d);

    // lado esquerdo
    glNormal3f(-1.0f, 0.0f, 0.0f); // Normal pointing towards negative X
    glVertex3f(-w, -h, -d);
    glVertex3f(-w, -h,  d);
    glVertex3f(-w,  h,  d);
    glVertex3f(-w,  h, -d);

    // lado direito
    glNormal3f(1.0f, 0.0f, 0.0f);  // Normal pointing towards positive X
    glVertex3f( w, -h, -d);
    glVertex3f( w,  h, -d);
    glVertex3f( w,  h,  d);
    glVertex3f( w, -h,  d);

    // topo
    glNormal3f(0.0f, 1.0f, 0.0f);  // Normal pointing towards positive Y
    glVertex3f(-w,  h, -d);
    glVertex3f(-w,  h,  d);
    glVertex3f( w,  h,  d);
    glVertex3f( w,  h, -d);

    // base
    glNormal3f(0.0f, -1.0f, 0.0f); // Normal pointing towards negative Y
    glVertex3f(-w, -h, -d);
    glVertex3f( w, -h, -d);
    glVertex3f( w, -h,  d);
    glVertex3f(-w, -h,  d);

    glEnd();
}


void PosicionaObservador(void)
{
    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();
    glTranslatef(-obsX,-obsY,-obsZ);
    glRotatef(rotX,1,0,0);
    glRotatef(rotY,0,1,0);
}




void EspecificaParametrosVisualizacao(void)
{
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    gluPerspective(angle,fAspect,0.1,1200);
    PosicionaObservador();
}




void GerenciaMouse(int button, int state, int x, int y)
{
    if(state==GLUT_DOWN)
    {
        x_ini = x;
        y_ini = y;
        obsX_ini = obsX;
        obsY_ini = obsY;
        obsZ_ini = obsZ;
        rotX_ini = rotX;
        rotY_ini = rotY;
        bot = button;
    }
    else bot = -1;
}


void GerenciaTeclado(unsigned char key, int x, int y)
{
    switch(key)
    {
        case 'w':
        case 'W':
            obsZ-=0.5f;
            printf("frente");
        break;


        case 's':
        case 'S':
            obsZ+=0.5f;
        break;


        case 'a':
        case 'A':
            obsX-=0.5f;
        break;

        case 'd':
        case 'D':
            obsX+=0.5f;
        break;
    }
    EspecificaParametrosVisualizacao();
    glutPostRedisplay();
}


#define SENS_ROT    5.0
#define SENS_OBS    15.0
#define SENS_TRANSL 30.0
void GerenciaMovim(int x, int y)
{
    if(bot==GLUT_LEFT_BUTTON)
    {
        int deltax = x_ini - x;
        int deltay = y_ini - y;
        rotY = rotY_ini - deltax/SENS_ROT;
        rotX = rotX_ini - deltay/SENS_ROT;
    }
    else if(bot==GLUT_RIGHT_BUTTON)
    {
        int deltaz = y_ini - y;
        obsZ = obsZ_ini + deltaz/SENS_OBS;
    }
    else if(bot==GLUT_MIDDLE_BUTTON)
    {
        int deltax = x_ini - x;
        int deltay = y_ini - y;
        obsX = obsX_ini + deltax/SENS_TRANSL;
        obsY = obsY_ini - deltay/SENS_TRANSL;
    }
    //PosicionaObservador();
    glutPostRedisplay();
}




void configuraIluminacao() {
    GLfloat luzAmbiente[4] = {0.2f, 0.2f, 0.2f, 1.0f};
    GLfloat luzDifusa[4] = {0.7f, 0.7f, 0.7f, 1.0f};
    GLfloat luzEspecular[4] = {1.0f, 1.0f, 1.0f, 1.0f};
    GLfloat posicaoLuz[4] = {0.0f, 10.0f, 10.0f, 1.0f};
 
    glLightfv(GL_LIGHT0, GL_AMBIENT, luzAmbiente);
    glLightfv(GL_LIGHT0, GL_DIFFUSE, luzDifusa);
    glLightfv(GL_LIGHT0, GL_SPECULAR, luzEspecular);
    glLightfv(GL_LIGHT0, GL_POSITION, posicaoLuz);
 
    glEnable(GL_LIGHTING);
    glEnable(GL_LIGHT0);
    glEnable(GL_COLOR_MATERIAL);
    glEnable(GL_NORMALIZE);
     
     
    GLfloat materialAmbiente[4] = {0.2f, 0.2f, 0.2f, 1.0f};
    GLfloat materialDifuso[4] = {0.8f, 0.8f, 0.8f, 1.0f};
    GLfloat materialEspecular[4] = {0.5f, 0.5f, 0.5f, 1.0f};
    GLfloat materialBrilho = 32.0f;
     
    glMaterialfv(GL_FRONT, GL_AMBIENT, materialAmbiente);
    glMaterialfv(GL_FRONT, GL_DIFFUSE, materialDifuso);
    glMaterialfv(GL_FRONT, GL_SPECULAR, materialEspecular);
    glMaterialf(GL_FRONT, GL_SHININESS, materialBrilho);
 }


void display() {
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    PosicionaObservador();
    //g++ -o main.exe main.cpp -I"./include" -L"./lib/x64" -lfreeglut -lopengl32 -lglu32
    glColor3f(1, 0, 0);
    desenhaParalelepipedo(16, 32, 16);
    glLoadIdentity();
    glutSwapBuffers();
}


void reshape(int width, int height) {
     glViewport(0, 0, width, height);
     
     glMatrixMode(GL_PROJECTION);
     glLoadIdentity();
     gluPerspective(45.0f, (GLfloat)width/(GLfloat)height, 0.1f, 100.0f);
     
     glMatrixMode(GL_MODELVIEW);
     glLoadIdentity();
 }


 int main(int argc, char** argv) {
    obsX = obsY = 0.0f;
    obsZ = 0.0f;
    rotX = 10.0f;
    rotY = -180.0f;

    glutInit(&argc, argv);
    glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGB | GLUT_DEPTH);
    glutInitWindowSize(800, 600);
    glutInitWindowPosition(100, 100);
    glutCreateWindow("Braco");
   
   
    glClearColor(0.0f, 0.0f, 0.1f, 1.0f);
    glEnable(GL_DEPTH_TEST);
     
     
    configuraIluminacao();
    glutMouseFunc(GerenciaMouse);
    glutMotionFunc(GerenciaMovim);
    glutKeyboardFunc(GerenciaTeclado);
     
     
    glutDisplayFunc(display);
    glutReshapeFunc(reshape);
     
     
    glutMainLoop();
     
     return 0;
 }









