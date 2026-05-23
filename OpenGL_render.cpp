#include "physics.h"
#include <GL/glut.h>
#include <cfloat>
using namespace std;

int windowWidth = 1200, windowHeight = 800;

float cameraDistance = 18.0f;
float cameraYaw      = 0.0f;
float cameraPitch    = -20.0f;
bool  rotatingView   = false;
int   lastRotateX    = 0, lastRotateY = 0;
Vector3 cameraTarget(0.0f, 4.0f, 0.0f);

SPHParticle* draggedBody = nullptr;
Vector3 dragOffset(0, 0, 0);
Vector3 lastDragPos(0, 0, 0);
int lastMouseX = 0, lastMouseY = 0;

void drawWalls(const Bounds& b) {
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    glColor4f(0.4f, 0.6f, 1.0f, 0.15f);
    glDisable(GL_LIGHTING);

    float x0=b.minX, x1=b.maxX;
    float y0=b.minY, y1=b.maxY;
    float z0=b.minZ, z1=b.maxZ;

    glBegin(GL_QUADS);
    glVertex3f(x0,y0,z0); glVertex3f(x1,y0,z0); glVertex3f(x1,y1,z0); glVertex3f(x0,y1,z0);
    glVertex3f(x0,y0,z1); glVertex3f(x1,y0,z1); glVertex3f(x1,y1,z1); glVertex3f(x0,y1,z1);
    glVertex3f(x0,y0,z0); glVertex3f(x0,y0,z1); glVertex3f(x0,y1,z1); glVertex3f(x0,y1,z0);
    glVertex3f(x1,y0,z0); glVertex3f(x1,y0,z1); glVertex3f(x1,y1,z1); glVertex3f(x1,y1,z0);
    glVertex3f(x0,y1,z0); glVertex3f(x1,y1,z0); glVertex3f(x1,y1,z1); glVertex3f(x0,y1,z1);
    glVertex3f(x0,y0,z0); glVertex3f(x1,y0,z0); glVertex3f(x1,y0,z1); glVertex3f(x0,y0,z1);
    glEnd();

    glColor3f(0.6f, 0.7f, 0.9f);
    glBegin(GL_LINES);
    glVertex3f(x0,y0,z0); glVertex3f(x1,y0,z0);
    glVertex3f(x1,y0,z0); glVertex3f(x1,y0,z1);
    glVertex3f(x1,y0,z1); glVertex3f(x0,y0,z1);
    glVertex3f(x0,y0,z1); glVertex3f(x0,y0,z0);
    glVertex3f(x0,y1,z0); glVertex3f(x1,y1,z0);
    glVertex3f(x1,y1,z0); glVertex3f(x1,y1,z1);
    glVertex3f(x1,y1,z1); glVertex3f(x0,y1,z1);
    glVertex3f(x0,y1,z1); glVertex3f(x0,y1,z0);
    glVertex3f(x0,y0,z0); glVertex3f(x0,y1,z0);
    glVertex3f(x1,y0,z0); glVertex3f(x1,y1,z0);
    glVertex3f(x1,y0,z1); glVertex3f(x1,y1,z1);
    glVertex3f(x0,y0,z1); glVertex3f(x0,y1,z1);
    glEnd();

    glEnable(GL_LIGHTING);
    glDisable(GL_BLEND);
}

void display() {
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    glLoadIdentity();

    float yawRad   = cameraYaw   * PI / 180.0f;
    float pitchRad = cameraPitch * PI / 180.0f;
    Vector3 cameraPos(
        cameraTarget.x + cameraDistance * cosf(pitchRad) * sinf(yawRad),
        cameraTarget.y + cameraDistance * sinf(pitchRad),
        cameraTarget.z + cameraDistance * cosf(pitchRad) * cosf(yawRad)
    );
    gluLookAt(cameraPos.x, cameraPos.y, cameraPos.z,
              cameraTarget.x, cameraTarget.y, cameraTarget.z,
              0, 1, 0);

    glDisable(GL_LIGHTING);
    glColor3f(0.4f, 0.4f, 0.4f);
    glBegin(GL_QUADS);
    glVertex3f(-10,0,-10); glVertex3f(10,0,-10);
    glVertex3f(10,0,10);   glVertex3f(-10,0,10);
    glEnd();

    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    glColor4f(0.0f, 0.0f, 0.0f, 0.25f);
    Vector3 lightPos(8.0f, 15.0f, 8.0f);
    const float shadowCullY = bounds.maxY * 0.9f;
    for (size_t i = 0; i < bodies.size(); i++) {
        Vector3& bp = bodies[i]->position;
        if (bp.y > shadowCullY) continue;
        float floorY = 0.0f;
        float denom  = bp.y - lightPos.y;
        if (fabsf(denom) < 0.001f) continue;
        float t = (floorY - lightPos.y) / denom;
        float sx = lightPos.x + t * (bp.x - lightPos.x);
        float sz = lightPos.z + t * (bp.z - lightPos.z);
        float sr = bodies[i]->radius * 0.8f;
        glBegin(GL_TRIANGLE_FAN);
        glVertex3f(sx, 0.001f, sz);
        for (int k = 0; k <= 20; k++) {
            float angle = k * 2.0f * PI / 20.0f;
            glVertex3f(sx + cosf(angle) * sr, 0.001f, sz + sinf(angle) * sr);
        }
        glEnd();
    }
    glDisable(GL_BLEND);

    glDisable(GL_LIGHTING);
    glEnable(GL_POINT_SMOOTH);
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    glPointSize(12.0f);

    glBegin(GL_POINTS);
    for (size_t i = 0; i < bodies.size(); i++) {
        float t = bodies[i]->temperature;
        glColor3f(t, 0.2f, 1.0f - t);
        glVertex3f(bodies[i]->position.x,
                   bodies[i]->position.y,
                   bodies[i]->position.z);
    }
    glEnd();

    glDisable(GL_POINT_SMOOTH);
    glDisable(GL_BLEND);
    glEnable(GL_LIGHTING);

    drawWalls(bounds);
    glutSwapBuffers();
}

void reshape(int w, int h) {
    windowWidth  = w;
    windowHeight = h;
    glViewport(0, 0, w, h);
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    gluPerspective(45.0f, (float)w / h, 0.1f, 100.0f);
    glMatrixMode(GL_MODELVIEW);
}

Vector3 mouseToWorld(int mouseX, int mouseY, float depth) {
    GLdouble modelview[16], projection[16];
    GLint viewport[4];
    glGetDoublev(GL_MODELVIEW_MATRIX, modelview);
    glGetDoublev(GL_PROJECTION_MATRIX, projection);
    glGetIntegerv(GL_VIEWPORT, viewport);
    GLdouble worldX, worldY, worldZ;
    gluUnProject(mouseX, windowHeight - mouseY, depth,
                 modelview, projection, viewport,
                 &worldX, &worldY, &worldZ);
    return Vector3(worldX, worldY, worldZ);
}

void mouse(int button, int state, int x, int y) {
    lastMouseX = x;
    lastMouseY = y;

    if (button == GLUT_LEFT_BUTTON) {
        if (state == GLUT_DOWN) {
            GLdouble modelview[16], projection[16];
            GLint viewport[4];
            glGetDoublev(GL_MODELVIEW_MATRIX, modelview);
            glGetDoublev(GL_PROJECTION_MATRIX, projection);
            glGetIntegerv(GL_VIEWPORT, viewport);

            float        closestDist = FLT_MAX;
            SPHParticle* closestBody = nullptr;

            for (auto body : bodies) {
                GLdouble sx, sy, sz;
                gluProject(body->position.x, body->position.y, body->position.z,
                           modelview, projection, viewport, &sx, &sy, &sz);
                sy = windowHeight - sy;
                float dx = x - sx, dy = y - sy;
                float dist = sqrtf(dx*dx + dy*dy);
                if (dist < closestDist && dist < 50.0f) {
                    closestDist = dist;
                    closestBody = body;
                }
            }

            if (closestBody) {
                draggedBody  = closestBody;
                dragOffset   = draggedBody->position;
                lastDragPos  = draggedBody->position;
                rotatingView = false;
            } else {
                rotatingView = true;
                lastRotateX  = x;
                lastRotateY  = y;
            }
        } else if (state == GLUT_UP) {
            if (draggedBody)
                draggedBody->velocity = (draggedBody->position - lastDragPos) * 10.0f;
            draggedBody  = nullptr;
            rotatingView = false;
        }
    }
}

void motion(int x, int y) {
    if (rotatingView) {
        float dx = (x - lastRotateX) * 0.3f;
        float dy = (y - lastRotateY) * 0.3f;
        cameraYaw   += dx;
        cameraPitch += dy;
        if (cameraPitch >  85.0f) cameraPitch =  85.0f;
        if (cameraPitch < -85.0f) cameraPitch = -85.0f;
        lastRotateX = x;
        lastRotateY = y;
    } else if (draggedBody) {
        GLdouble modelview[16], projection[16];
        GLint viewport[4];
        glGetDoublev(GL_MODELVIEW_MATRIX, modelview);
        glGetDoublev(GL_PROJECTION_MATRIX, projection);
        glGetIntegerv(GL_VIEWPORT, viewport);

        GLdouble origSX, origSY, origSZ;
        gluProject(dragOffset.x, dragOffset.y, dragOffset.z,
                   modelview, projection, viewport,
                   &origSX, &origSY, &origSZ);

        GLdouble newWX, newWY, newWZ;
        gluUnProject(x, windowHeight - y, origSZ,
                     modelview, projection, viewport,
                     &newWX, &newWY, &newWZ);

        lastDragPos           = draggedBody->position;
        draggedBody->position = Vector3(newWX, newWY, newWZ);
        draggedBody->velocity = Vector3(0,0,0);
    }
    lastMouseX = x;
    lastMouseY = y;
}