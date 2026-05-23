#include "physics.h"
#include "config.h"
#include <GL/glut.h>
#include <iostream>
using namespace std;

// Declares modules from render.cpp file
void display();
void reshape(int w, int h);
void mouse(int button, int state, int x, int y);
void motion(int x, int y);

GLUquadric* sharedQuad = nullptr;

void timer(int value) {
    // I've used elapsed time to ensure frame count is based off physics
    static float lastTime = glutGet(GLUT_ELAPSED_TIME) * 0.001f;
    float currentTime = glutGet(GLUT_ELAPSED_TIME) * 0.001f;
    float dt = currentTime - lastTime;
    if (dt > 0.016f) dt = 0.016f;
    lastTime = currentTime;

    updatePhysics(dt);
    frameCount++;

    glutPostRedisplay();
    glutTimerFunc(16, timer, 0);
}

void init() {
    sharedQuad = gluNewQuadric();
    gluQuadricNormals(sharedQuad, GLU_SMOOTH);
    gluQuadricDrawStyle(sharedQuad, GLU_FILL);

    glClearColor(0.1f, 0.1f, 0.15f, 1.0f);
    glEnable(GL_DEPTH_TEST);
    glEnable(GL_LIGHTING);
    glEnable(GL_LIGHT0);
    glEnable(GL_COLOR_MATERIAL);
    glColorMaterial(GL_FRONT_AND_BACK, GL_AMBIENT_AND_DIFFUSE);

    GLfloat light_position[] = { 5.0f, 10.0f, 5.0f, 0.0f };
    GLfloat light_ambient[]  = { 0.5f, 0.5f,  0.5f, 1.0f };
    GLfloat light_diffuse[]  = { 1.0f, 1.0f,  1.0f, 1.0f };
    glLightfv(GL_LIGHT0, GL_POSITION, light_position);
    glLightfv(GL_LIGHT0, GL_AMBIENT,  light_ambient);
    glLightfv(GL_LIGHT0, GL_DIFFUSE,  light_diffuse);

    initBalls();
}

int main(int argc, char** argv) {
    glutInit(&argc, argv);
    glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGB | GLUT_DEPTH);
    glutInitWindowSize(1200, 800);
    glutCreateWindow("SPH - kernel interpolation");

    init();

    glutDisplayFunc(display);
    glutReshapeFunc(reshape);
    glutTimerFunc(16, timer, 0);
    glutMouseFunc(mouse);
    glutMotionFunc(motion);

    glutMainLoop();

    gluDeleteQuadric(sharedQuad);
    return 0;
}