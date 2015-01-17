/***************************************************************************
                          main.cpp
                          --------
    begin                : 02.02.2001
    author               : Stefan Birmanns
    email                : sculptor@biomachina.org
 ***************************************************************************/

// svt includes
#include <svt_system.h>
#include <svt_window.h>
#include <svt_threads.h>
#include <svt_opengl.h>
#include <svt_teapot.h>
// clib includes
#include <stdio.h>

double scene_x = 0;
double scene_y = 0;
svt_win_handle win1, win2;

void renderTestScene(int width, int height)
{
    glLineWidth(1.3);
    glShadeModel(GL_SMOOTH);
    glEnable(GL_DEPTH_TEST);
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    gluPerspective((GLdouble)(35.0), ((GLdouble)width)/((GLdouble)height), (GLdouble)(1.0), (GLdouble)(400.0));
    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();

    glViewport(0,0,width,height);

    glClearColor(0.0, 0.0, 0.0, 1.0);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    glRotatef(scene_x,0.0,1.0,0.0);
    glRotatef(scene_y,1.0,0.0,0.0);

    glEnable(GL_LIGHT0);
    glEnable(GL_LIGHTING);

    glColor3f(0.0, 1.0, 0.0);
    glTranslatef(0.0,0.0,-10.0f);
    svt_drawSolidTeapot(2.0f);
}

void render(svt_win_handle, int width, int height )
{
    renderTestScene(width,height);
};

void resize(svt_win_handle win, int, int )
{
    svt_redrawWindow(win);
};

void drag(svt_win_handle, int, int, int mouseb)
{
    if (mouseb == SVT_LEFT_MOUSE_BUTTON)
        printf("Left Mouse Button\n");
    if (mouseb == SVT_MIDDLE_MOUSE_BUTTON)
        printf("Middle Mouse Button\n");
    if (mouseb == SVT_RIGHT_MOUSE_BUTTON)
        printf("Right Mouse Button\n");
};

void lbutton(svt_win_handle win, int, int)
{
    if (win == win1)
    {
        scene_x += 5.0f;
    } else {
        scene_y += 5.0f;
    }
    svt_redrawWindow(win);
};

void rbutton(svt_win_handle win, int, int)
{
    if (win == win1)
    {
        scene_x -= 5.0f;
    } else {
        scene_y -= 5.0f;
    }
    svt_redrawWindow(win);
};

void key_down(svt_win_handle, int, int, char c)
{
    printf("Key %c pressed!\n", c);
};
void special_key_down(svt_win_handle win, int, int, int key)
{
    if (key == SVT_KEY_LEFT)
        scene_x -= 5.0f;
    if (key == SVT_KEY_RIGHT)
        scene_x += 5.0f;

    svt_redrawWindow(win);
};

void key_up(svt_win_handle, int, int, char c)
{
    printf("Key %c released!\n", c);
};

int main()
{
    svt_system_init();

    // some global settings

    svt_createThreads(false);
    //svt_setDisplay("zam434:0.0");
    svt_setDoublebuffer(true);
    //svt_setSwapSync(true);
    //svt_setStereo(true);

    svt_setRGBA(true);
    svt_setDepthBufferBits(12);


    // now create window 1

    svt_setWindowTitle("SVT_System Testwindow 1");
    int win1 = svt_createWindow(10,10,640,200);

    svt_setDrawingFunc(win1, &render);
    svt_setResizeWindowFunc(win1, &resize);
    svt_setLMBDownFunc(win1, &lbutton);
    svt_setRMBDownFunc(win1, &rbutton);
    svt_setMouseDragFunc(win1, &drag);
    svt_setKeyDownFunc(win1, &key_down);
    svt_setKeyUpFunc(win1, &key_up);
    svt_setSpecialKeyDownFunc(win1, &special_key_down);

    svt_openWindow(win1);


/*
    // now create window 2

    svt_setWindowTitle("SVT_System Testwindow 2");
    int win2 = svt_createWindow(10,300,640,200);

    svt_setDrawingFunc(win2, &render);
    svt_setResizeWindowFunc(win2, &resize);
    svt_setLMBDownFunc(win2, &lbutton);
    svt_setRMBDownFunc(win2, &rbutton);
    svt_setMouseDragFunc(win2, &drag);
    svt_setKeyDownFunc(win2, &key_down);
    svt_setKeyUpFunc(win2, &key_up);
    svt_setSpecialKeyDownFunc(win2, &special_key_down);

    svt_openWindow(win2);
*/

    // and go!

    svt_system_mainloop();

    return 0;
};
