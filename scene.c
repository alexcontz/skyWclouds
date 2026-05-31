#include <GL/glos.h>
#include <GL/gl.h>
#include <GL/glu.h>
#include <GL/glaux.h>

void myinit(void);
void CALLBACK display(void);
void CALLBACK myReshape(GLsizei w, GLsizei h);
void CALLBACK updateAnimation(void);
void CALLBACK keyLeft(void);
void CALLBACK keyRight(void);
void CALLBACK keyW(void);
void CALLBACK keyS(void);
void CALLBACK keyA(void);
void CALLBACK keyD(void);
void CALLBACK keyQ(void);
void CALLBACK keyE(void);
static void drawCloud(float cx, float cy, float cz);
static void makeShadowMatrix(GLfloat m[16], GLfloat yg);

// Camera yaw rotation around the Y axis.
static GLfloat rotY = 0.0f;

// Light position for the "sun" (x, y, z, w).
static GLfloat lightPos[4] = { 3.0f, 5.0f, 0.0f, 1.0f };

#define NRAIN   12
#define YGROUND -3.0f
#define YCLOUD   1.5f

// Per-raindrop positions used by the animation loop.
static GLfloat rainX[NRAIN], rainY[NRAIN], rainZ[NRAIN];

void myinit(void)
{
    int i;
    GLfloat ambient[]  = { 0.2f, 0.2f, 0.2f, 1.0f };
    GLfloat diffuse[]  = { 1.0f, 1.0f, 0.8f, 1.0f };
    GLfloat specular[] = { 0.5f, 0.5f, 0.5f, 1.0f };

    // Basic lighting setup (ambient + warm diffuse + neutral specular).
    glEnable(GL_LIGHTING);
    glEnable(GL_LIGHT0);
    glLightfv(GL_LIGHT0, GL_AMBIENT,  ambient);
    glLightfv(GL_LIGHT0, GL_DIFFUSE,  diffuse);
    glLightfv(GL_LIGHT0, GL_SPECULAR, specular);
    glDepthFunc(GL_LESS);
    glEnable(GL_DEPTH_TEST);
    // Sky background color.
    glClearColor(0.53f, 0.81f, 0.98f, 1.0f);

    // Seed raindrops under each cloud cluster.
    for (i = 0; i < 6; i++)
    {
        rainX[i] = -2.5f + (i % 3) * 0.5f;
        rainZ[i] = -1.3f + (i / 3) * 0.6f;
        rainY[i] = YCLOUD - 0.3f - i * 0.3f;
    }
    for (i = 6; i < 12; i++)
    {
        int j = i - 6;
        rainX[i] = 1.5f + (j % 3) * 0.5f;
        rainZ[i] = 0.2f  + (j / 3) * 0.6f;
        rainY[i] = YCLOUD - 0.3f - j * 0.3f;
    }
}

void CALLBACK keyLeft(void)
{
    rotY -= 5.0f;
}
void CALLBACK keyRight(void)
{
    rotY += 5.0f;
}
void CALLBACK keyW(void)
{
    lightPos[1] += 0.3f;
}
void CALLBACK keyS(void)
{
    lightPos[1] -= 0.3f;
    if (lightPos[1] <= YGROUND + 0.1f)
        lightPos[1] = YGROUND + 0.1f;
}
void CALLBACK keyA(void)
{
    lightPos[0] -= 0.3f;
}
void CALLBACK keyD(void)
{
    lightPos[0] += 0.3f;
}
void CALLBACK keyQ(void)
{
    lightPos[2] -= 0.3f;
}
void CALLBACK keyE(void)
{
    lightPos[2] += 0.3f;
}

void CALLBACK updateAnimation(void)
{
    int i;
    // Drop all raindrops and reset when they hit the ground.
    for (i = 0; i < NRAIN; i++) {
        rainY[i] -= 0.05f;
        if (rainY[i] < YGROUND)
            rainY[i] = YCLOUD - 0.3f;
    }
}

static void drawCloud(float cx, float cy, float cz)
{
    glPushMatrix();

    // Build a single cloud from overlapping spheres.
    glTranslatef(cx, cy, cz);
    auxSolidSphere(0.6);

    // 2nd cloud piece (left)
    glPushMatrix();
    glTranslatef(-0.7f,  0.1f, 0.0f);
    auxSolidSphere(0.45);
    glPopMatrix();

    // 3rd cloud piece (right)
    glPushMatrix();
    glTranslatef( 0.7f,  0.1f, 0.0f);
    auxSolidSphere(0.45);
    glPopMatrix();

    // 4th cloud piece (top)
    glPushMatrix();
    glTranslatef( 0.0f,  0.5f, 0.0f);
    auxSolidSphere(0.40);
    glPopMatrix();

    glPopMatrix();
}

// Projects geometry onto the ground plane (yg) from the point lightPos.
static void makeShadowMatrix(GLfloat m[16], GLfloat yg)
{
    GLfloat lx = lightPos[0], ly = lightPos[1], lz = lightPos[2];
    GLfloat d = ly - yg;

    m[0]=d;      m[1] = 0.0f;     m[2] = 0.0f;     m[3] = 0.0f;
    m[4]=-lx;    m[5] = -yg;      m[6] = -lz;      m[7] = -1.0f;
    m[8]=0.0f;   m[9] = 0.0f;     m[10] = d;       m[11] = 0.0f;
    m[12]=lx*yg; m[13] = ly * yg; m[14] = lz * yg; m[15] = ly;
}

void CALLBACK display(void)
{
    int i;
    GLfloat shadowMat[16];
    GLfloat cloudMat[] = { 1.0f, 1.0f, 1.0f, 1.0f };

    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();

    // Cam
    glTranslatef(0.0f, 0.0f, -10.0f);
    glRotatef(rotY, 0.0f, 1.0f, 0.0f);

    // Light pos
    glLightfv(GL_LIGHT0, GL_POSITION, lightPos);

    // Ground
    glDisable(GL_LIGHTING);
    glColor3f(0.3f, 0.5f, 0.2f);
    glBegin(GL_QUADS);
        glVertex3f(-10.0f, YGROUND, -10.0f);
        glVertex3f( 10.0f, YGROUND, -10.0f);
        glVertex3f( 10.0f, YGROUND,  10.0f);
        glVertex3f(-10.0f, YGROUND,  10.0f);
    glEnd();
    glEnable(GL_LIGHTING);

    // Cloud shadow (projected onto the ground plane).
    makeShadowMatrix(shadowMat, YGROUND);
    glEnable(GL_POLYGON_OFFSET_FILL);
    glPolygonOffset(-1.0f, -1.0f);
    glDisable(GL_LIGHTING);
    glColor3f(0.15f, 0.25f, 0.1f);
    glPushMatrix();
        glMultMatrixf(shadowMat);
        drawCloud(-2.0f, YCLOUD,        -1.0f);
        drawCloud( 2.0f, YCLOUD + 0.5f,  0.5f);
    glPopMatrix();
    glEnable(GL_LIGHTING);
    glDisable(GL_POLYGON_OFFSET_FILL);

    // Clouds
    glMaterialfv(GL_FRONT, GL_AMBIENT_AND_DIFFUSE, cloudMat);
    glMaterialf(GL_FRONT, GL_SHININESS, 15.0f);
    drawCloud(-2.0f, YCLOUD,        -1.0f);
    drawCloud( 2.0f, YCLOUD + 0.5f,  0.5f);

    // Rain
    glDisable(GL_LIGHTING);
    glColor3f(0.5f, 0.6f, 1.0f);
    for (i = 0; i < NRAIN; i++) {
        glPushMatrix();
        glTranslatef(rainX[i], rainY[i], rainZ[i]);
        glScalef(0.05f, 0.25f, 0.05f);
        auxSolidSphere(1.0);
        glPopMatrix();
    }
    glEnable(GL_LIGHTING);

    // Sun
    glDisable(GL_LIGHTING);
    glColor3f(1.0f, 1.0f, 0.0f);
    glPushMatrix();
        glTranslatef(lightPos[0], lightPos[1], lightPos[2]);
        auxWireCube(0.4);
    glPopMatrix();
    glEnable(GL_LIGHTING);

	// For Double Buffering
    auxSwapBuffers();
}

void CALLBACK myReshape(GLsizei w, GLsizei h)
{
    if (!h) return;
    glViewport(0, 0, w, h);
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    gluPerspective(45.0, (GLfloat)w / (GLfloat)h, 1.0, 50.0);
    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();
}

int main(int argc, char** argv)
{
    auxInitDisplayMode(AUX_DOUBLE | AUX_RGB | AUX_DEPTH16);
    auxInitPosition(0, 0, 600, 500);
    auxInitWindow("Clouds with Sky");
    myinit();

    auxKeyFunc(AUX_LEFT,  keyLeft);
    auxKeyFunc(AUX_RIGHT, keyRight);

    auxKeyFunc(AUX_W, keyW);
    auxKeyFunc(AUX_A, keyA);
    auxKeyFunc(AUX_S, keyS);
    auxKeyFunc(AUX_D, keyD);
    auxKeyFunc(AUX_Q, keyQ);
    auxKeyFunc(AUX_E, keyE);

    auxKeyFunc(AUX_w, keyW);
    auxKeyFunc(AUX_a, keyA);
    auxKeyFunc(AUX_s, keyS);
    auxKeyFunc(AUX_d, keyD);
    auxKeyFunc(AUX_q, keyQ);
    auxKeyFunc(AUX_e, keyE);

    auxIdleFunc(updateAnimation);
    auxReshapeFunc(myReshape);
    auxMainLoop(display);
    return 0;
}
