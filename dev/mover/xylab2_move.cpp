//modified by: Michael Kausch  
//date: 1/31/23
// - added text and multiple boxes
//date:  2/1/23
// - made it so that the particle color was random
// 
//
//author: Gordon Griesel
//date: Spring 2022
//purpose: get openGL working on your personal computer
//
#include <iostream>
#include <stdio.h>
#include <unistd.h>
#include <cstdlib>
#include <ctime>
#include <cstring>
#include <cmath>
#include <X11/Xlib.h>
#include <X11/keysym.h>
#include <GL/glx.h>
#include "fonts.h"

using namespace std;

#define PI 3.1415926535

// need glEnable(GL_TEXTURE_2D);
// need initialize_fonts();


//some structures

class Global {
public:
    int xres, yres;
    int n; // how many particles are floating around
    Global();
    bool mouse_pressed = false;
    float clickx;
    float clicky;
    float circ_rad = 200.0f;
    


    Rect r[5];
} g;

// Global Vars
const int MAX_PARTICLES = 5000;
const float GRAVITY = 0.06;

// class Color {
// public:
//     char r;
//     char g;
//     char b;
//     Color(int _r, int _g, int _b)
//     {
//         r = (char)_r;
//         g = (char)_g;
//         b = (char)_b;
//     }
//     Color()
//     {
//         r = g = b = 0;
//     }
// };



class Box {
public:
    // vars
    float w, h;
    float vel[3];
    float pos[3]; 
    unsigned char ready_color[3];
    unsigned char moving_color[3];
    unsigned char activated_color[3];
    unsigned char reseting_color[3];
    unsigned char color[3];
    bool is_activated;
    bool is_moving;
    bool is_reseting;
    bool is_ready;
    int collision_state; // 0 = hasn't yet; 1 = collided; 2 = collide end
    int collision_obj_id;

    // setters
    void set_ready_color() 
    {
        color[0] = 255;
        color[1] = 0;
        color[2] = 0;     
    }

    void set_moving_color() 
    {
        color[0] = 0;
        color[1] = 255;
        color[2] = 0;     
    }
    void set_activated_color() 
    {
        color[0] = 0;
        color[1] = 0;
        color[2] = 255;     
    }
    void set_reseting_color()
    {
        color[0] = 255;
        color[1] = 0;
        color[2] = 255;     
    }

    void fade_color()
    {
        static bool i,j,k;

        color[0] = (i? (char)((int)color[0]+1) : color[0]-1);
        color[1] = (j? (char)((int)color[1]+1) : color[1]-1);
        color[2] = (k? (char)((int)color[2]+1) : color[2]-1);

        i = ((color[0] == 254) ? 0 : color[2] == 0? 1 : i);
        j = ((color[0] == 254) ? 0 : color[2] == 0? 1 : j);
        k = ((color[0] == 254) ? 0 : color[2] == 0? 1 : k);
        
    }

    void double_fade()
    {
        fade_color();
        fade_color();
    }

    void set_color(unsigned char col[3])
    {
        memcpy(color, col, sizeof(unsigned char) * 3);
    }

    void set_vel(float x, float y, float z = 0)
    {
        vel[0] = x;
        vel[1] = y;
        vel[2] = z;
    }

    void set_location(float x, float y, float z=0) 
    {
        pos[0] = x;
        pos[1] = y;
        pos[2] = z;
    }

    // constructor
    Box()
    {
        w = 80.0f;
        h = 20.0f;
        pos[0] = g.xres/2.0f;
        pos[1] = g.yres/2.0f;
        vel[0] = 0.0f;
        vel[1] = 0.0f;
        is_activated = is_moving = is_reseting = false;
        is_ready = true;
        set_ready_color();
        collision_state = 0;
        collision_obj_id = 0;

 //       ready_color 
    }

} box[5], particle;




class X11_wrapper {
private:
    Display *dpy;
    Window win;
    GLXContext glc;
public:
    ~X11_wrapper();
    X11_wrapper();
    void set_title();
    bool getXPending();
    XEvent getXNextEvent();
    void swapBuffers();
    void reshape_window(int width, int height);
    void check_resize(XEvent *e);
    void check_mouse(XEvent *e);
    int check_keys(XEvent *e);
} x11;

// Global Function prototypes
void init_opengl(void);
void physics(void);
void render(void);
void make_particle(int x, int y);



//=====================================
// MAIN FUNCTION IS HERE
//=====================================
int main()
{
    init_opengl();

    //Main loop
    int done = 0;
    while (!done) {
        //Process external events.
        while (x11.getXPending()) {
            XEvent e = x11.getXNextEvent();
            x11.check_resize(&e);
            x11.check_mouse(&e);
            done = x11.check_keys(&e);
        }
        physics();
        render();
        x11.swapBuffers();
        usleep(200);
    }
    return 0;
}

Global::Global()
{
    xres = 700;
    yres = 700;
    // n = 0; // number of particles
}

X11_wrapper::~X11_wrapper()
{
    XDestroyWindow(dpy, win);
    XCloseDisplay(dpy);
}

X11_wrapper::X11_wrapper()
{
    GLint att[] = { GLX_RGBA, GLX_DEPTH_SIZE, 24, GLX_DOUBLEBUFFER, None };
    int w = g.xres, h = g.yres;
    dpy = XOpenDisplay(NULL);
    if (dpy == NULL) {
        cout << "\n\tcannot connect to X server\n" << endl;
        exit(EXIT_FAILURE);
    }
    Window root = DefaultRootWindow(dpy);
    XVisualInfo *vi = glXChooseVisual(dpy, 0, att);
    if (vi == NULL) {
        cout << "\n\tno appropriate visual found\n" << endl;
        exit(EXIT_FAILURE);
    } 
    Colormap cmap = XCreateColormap(dpy, root, vi->visual, AllocNone);
    XSetWindowAttributes swa;
    swa.colormap = cmap;
    swa.event_mask =
        ExposureMask | KeyPressMask | KeyReleaseMask |
        ButtonPress | ButtonReleaseMask |
        PointerMotionMask |
        StructureNotifyMask | SubstructureNotifyMask;
    win = XCreateWindow(dpy, root, 0, 0, w, h, 0, vi->depth,
            InputOutput, vi->visual, CWColormap | CWEventMask, &swa);
    set_title();
    glc = glXCreateContext(dpy, vi, NULL, GL_TRUE);
    glXMakeCurrent(dpy, win, glc);
}

void X11_wrapper::set_title()
{
    //Set the window title bar.
    XMapWindow(dpy, win);
    XStoreName(dpy, win, "3350 Lab1");
}

bool X11_wrapper::getXPending()
{
    //See if there are pending events.
    return XPending(dpy);
}

XEvent X11_wrapper::getXNextEvent()
{
    //Get a pending event.
    XEvent e;
    XNextEvent(dpy, &e);

    return e;
}

void X11_wrapper::swapBuffers()
{
    glXSwapBuffers(dpy, win);
}

void X11_wrapper::reshape_window(int width, int height)
{
    //window has been resized.
    g.xres = width;
    g.yres = height;
    //
    glViewport(0, 0, (GLint)width, (GLint)height);
    glMatrixMode(GL_PROJECTION); glLoadIdentity();
    glMatrixMode(GL_MODELVIEW); glLoadIdentity();
    glOrtho(0, g.xres, 0, g.yres, -1, 1);
}

void X11_wrapper::check_resize(XEvent *e)
{
    //The ConfigureNotify is sent by the
    //server if the window is resized.
    if (e->type != ConfigureNotify)
        return;
    XConfigureEvent xce = e->xconfigure;

    if (xce.width != g.xres || xce.height != g.yres) {
        //Window size did change.
        reshape_window(xce.width, xce.height);
    }
}
//-----------------------------------------------------------------------------

#define rnd() ((float)rand() / (float)RAND_MAX)

void make_particle(int x, int y)
{
    
    particle.w = 4;
    particle.h = 4;
    particle.pos[0] = x;
    particle.pos[1] = y;
    particle.vel[0] = rnd() * 0.2 - 0.1;
    particle.vel[1] = rnd() * 0.2 - 0.1;

    // color of particles, randomized r g and b for fade

    unsigned char c[3] = {(unsigned char)((int)rand() % 50), 
                            (unsigned char)((int)rand() % 50 + 85), 
                            (unsigned char)((int)rand() % 50 + 170) };

    particle.set_color(c);
}

void X11_wrapper::check_mouse(XEvent *e)
{
    static int savex = 0;
    static int savey = 0;

    //Weed out non-mouse events
    if (e->type != ButtonRelease &&
            e->type != ButtonPress &&
            e->type != MotionNotify) {
        //This is not a mouse event that we care about.
        return;
    }

    if (e->type == ButtonRelease) {
        return;
    }

    if (e->type == ButtonPress) {
        //Left button was pressed.
        // origin is flipped between display and mouse origin
        //     so we need to flip the y coord
        if (e->xbutton.button==1) {

            // make a particle when left mouse button is pressed
            make_particle(e->xbutton.x, g.yres - e->xbutton.y);
            g.n = 1;

            return;
        }
        if (e->xbutton.button==3) {
            //Right button was pressed.
            //g.clickx = e->xbutton.x;
            //g.clicky = e->xbutton.y;

            // toggles make_particle at mouse location
            //g.mouse_pressed = !g.mouse_pressed; 
            return;
        }
    }

    //Code placed here will execute whenever the mouse moves.
    if (e->type == MotionNotify) {
        //The mouse moved!
        if (savex != e->xbutton.x || savey != e->xbutton.y) {
            savex = e->xbutton.x;
            savey = e->xbutton.y;

            // make 5 particles when mouse moves
            //for (int i = 0; i < 5; i++) {
            //    make_particle(e->xbutton.x, g.yres - e->xbutton.y);
            //}
        }
        return;
    }
}

int X11_wrapper::check_keys(XEvent *e)
{
    if (e->type != KeyPress && e->type != KeyRelease) {
        return 0;
    }

    int key = XLookupKeysym(&e->xkey, 0);

    if (e->type == KeyPress) {
        switch (key) {
            case XK_1:
                //Key 1 was pressed
                break;
            case XK_Escape:
                //Escape key was pressed
                return 1;
        }
    }

    return 0;
}

// won't have to mess with this much in the project

void init_opengl(void)
{
    //OpenGL initialization
    // make the viewport the size of the whole window
    glViewport(0, 0, g.xres, g.yres);
    //Initialize matrices
    // most likely this will always be set this way in all our programs
    // vertices are drawn and then filled in
    glMatrixMode(GL_PROJECTION); glLoadIdentity();
    glMatrixMode(GL_MODELVIEW); glLoadIdentity();
    //Set 2D mode (no perspective)
    // modifies the matrices to be in orthographic mode
    // think of it as a camera looking at things from so far away
    //     that it is in 2 dimensions
    glOrtho(0, g.xres, 0, g.yres, -1, 1);
    //Set the screen background color
    // makes it almost black
    glClearColor(0.1, 0.1, 0.1, 1.0);

    // set box color
    //unsigned char c[3] = {100, 200, 100};

    //for (int i = 0; i < 5; i++) {
     //   box[i].set_color(c);
    //}

    // position the text boxes
    box[0].set_location(box[0].w + 50, g.yres - box[0].h - 75);
    box[1].set_location(box[0].pos[0] + 425, box[0].pos[1]);
    box[2].set_location(box[1].pos[0], box[1].pos[1] - 450);
    box[3].set_location(box[0].pos[0], box[2].pos[1]);
    box[4].set_location( (box[3].pos[0] + box[2].pos[0])/2.0f, 
                            (box[0].pos[1] + box[2].pos[1])/2.0f);

    // box[0].set_vel(20,50);
    // box[1].set_vel(0,50);
    // box[2].set_vel(-20,50);
    // box[3].set_vel(20,50);
    // box[4].set_vel(0,50);

    box[0].set_vel(0.5,0.5);
    box[1].set_vel(0,5);
    box[2].set_vel(-2,5);
    box[3].set_vel(2,5);
    box[4].set_vel(0,5);
    
    

    //Do this to allow fonts
    glEnable(GL_TEXTURE_2D);
    initialize_fonts();


}


void physics()
{

        

}

void render()
{
    //ggprint8b(&r, 16, 0x00ffff00, "n bullets: %i", g.nbullets);
    //ggprint8b(&r, 16, 0x00ffff00, "n asteroids: %i", g.nasteroids);

    glClear(GL_COLOR_BUFFER_BIT);
    //Rect r[5]; // was laggy so i moved it up to Global so that it's
    // only created once

    string words[5] = {"Requirements", "Design", "Coding",
        "Testing", "Maintenance"};

    // Rect r4;

    // r4.bot = g.yres - 20;
    // r4.left = 10;
    // r4.center = 0;
    // ggprint8b(&r4, 16, 0x00ff0000, "Waterfall Mode"); 

    for (int i = 0; i < 5; i++) {
        // Draw box
        glPushMatrix();
        //glColor3ub(150, 160, 220);
        glColor3ubv(box[i].color);
            
        glTranslatef(box[i].pos[0], box[i].pos[1], 0.0f);

        glBegin(GL_QUADS);
            glVertex2f(-box[i].w, -box[i].h);
            glVertex2f(-box[i].w,  box[i].h);
            glVertex2f( box[i].w,  box[i].h);
            glVertex2f( box[i].w, -box[i].h);
        glEnd();
        glPopMatrix();

        //Requirements, Design, Coding, Testing, Maintenance

        g.r[i].bot = box[i].pos[1] - 5;
        g.r[i].left = box[i].pos[0];
        //g.r[i].center = box[i].pos[0];

        g.r[i].center = 1;

        ggprint8b(g.r+i, 16, 0x00ffff00, words[i].c_str());


        // if (!box[i].is_ready) {
        //     g.r[i].bot = box[i].pos[1] - 5;
        //     g.r[i].left = box[i].pos[0];
        //     //g.r[i].center = box[i].pos[0];

        //     g.r[i].center = 1;

        //     ggprint8b(g.r+i, 16, 0x00ffff00, words[i].c_str());
        // }

    }

    //int b = rand() % 50 + 200;
    // Draw particle

    if (g.n == 1) {
        glPushMatrix();

        // glColor3ub(150, 160, 220);
        // particle.fade_color();
        particle.double_fade();
        glColor3ubv(particle.color);

        glTranslatef(particle.pos[0], particle.pos[1], 0.0f);

        glBegin(GL_QUADS);
            glVertex2f(-particle.w, -particle.h);
            glVertex2f(-particle.w,  particle.h);
            glVertex2f( particle.w,  particle.h);
            glVertex2f( particle.w, -particle.h);
        glEnd();
        glPopMatrix();
    }


    // trying to get a circle to work

    // glPushMatrix();
    // glColor3f(255,255,0);

    // glTranslatef(g.xres, 0, 0.0f);

    // float angle;

    // glBegin(GL_TRIANGLE_FAN);
    // for(int i = 0; i < 100; i++) {
    //     angle = (2 * PI * i)/100;
    //     glVertex2f(cos(angle) * g.circ_rad, sin(angle) * g.circ_rad);
    // }
    // glEnd();
    // glPopMatrix();

}






