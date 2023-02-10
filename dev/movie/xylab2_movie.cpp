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
#include <sstream>
#include "fonts.h"
#include <assert.h>

using namespace std;

#define PI 3.1415926535

class Global {
public:
    int xres, yres;
    int n; // how many particles are floating around
    Global();
    bool mouse_pressed = false;
    float clickx;
    float clicky;
    int state; // 0 = hasn't yet; 1 = collided; 2 = collide end

    Rect r[6]; // 1-5 for block texts, 6 for key prompt
} g;

// Global Vars
const int MAX_PARTICLES = 5000;
const float GRAVITY = 0.5;

class Box {
public:
    // vars
    float w, h;
    float accel[3];
    float vel[3];
    float key_vel[3];
    float pos[3]; 
    unsigned char color[3];
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

    void noclip_y(const Box & obj)
    {
        pos[1] = (obj.pos[1] + obj.h + h);
    }

    void noclip_x(const Box & obj, string side)
    {
        assert(side == "left" || side == "right");
        if (side == "left")
            pos[0] = (obj.pos[0] - obj.w + w);
        else // side == "right"
            pos[0] = (obj.pos[0] + obj.w - w);
    }

    void set_accel(float x, float y, float z)
    {
        accel[0] = x;
        accel[1] = y;
        accel[2] = z;
    }

    void apply_gravity()
    {
        vel[1] -= GRAVITY;
    }

    void set_vel(float x, float y, float z)
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

    void stop()
    {
        set_accel(0,0,0);
        set_vel(0,0,0);
    }

    void move()
    {
        // accelerate
        vel[0] += accel[0];
        vel[1] += accel[1];
        vel[2] += accel[2];
        
        // move
        pos[0] += vel[0];
        pos[1] += vel[1];
        pos[2] += vel[2];
    }

    // for behavior, "left" or "right" need to be passed in
    // to indicate which sides of the box are being tested
    bool x_equal(const Box &obj, string side)
    {
        assert(side == "left" || side == "right");

        if (side == "left")
            return ( ((pos[0] - w) <= (obj.pos[0] + obj.w)) &&
                      ((pos[0] - w) > (obj.pos[0] + obj.w) - 10) );
        else // if (side == "right")
            return ( ((pos[0] + w) >= (obj.pos[0] - obj.w)) && 
                    ((pos[0] + w) < (obj.pos[0] - obj.w) + 10) );
    }

    bool y_equal(const Box &obj)
    {
        return (( (pos[1] - h) <= (obj.pos[1] + obj.h) ) &&
            (pos[1] - h) > (obj.pos[1] + obj.h - 10));
    }

    string get_info()
    {
        std::ostringstream temp;
        temp << "state: " << g.state << endl
            << "particle.vel[x]: " << vel[0] << endl
            << "particle.vel[y]: " << vel[1] << endl
            << "particle.vel[z]: " << vel[2] << endl
            << "particle.pos[x]: " << pos[0] << endl
            << "particle.pos[y]: " << pos[1] << endl
            << "particle.pos[z]: " << pos[2] << endl
            << "particle.w: " << w << endl
            << "particle.h: " << h << endl;

        return temp.str();
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
        // is_activated = is_moving = is_reseting = false;
        // is_ready = true;
        set_ready_color();
        set_accel(0,0,0);
        set_vel(0,0,0);
        
        collision_obj_id = 0;

 //       ready_color 
    }

} box[5], particle, cloud[MAX_PARTICLES];




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
void move_all();



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
        move_all();
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
    state = 0;
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
    XStoreName(dpy, win, "Mover");
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
    // particle.vel[0] = rnd() * 0.2 - 0.1;
    // particle.vel[1] = rnd() * 0.2 - 0.1;

    // color of particles, randomized r g and b for fade

    unsigned char c[3] = {(unsigned char)((int)rand() % 50), 
                            (unsigned char)((int)rand() % 50 + 85), 0 };
                            // (unsigned char)((int)rand() % 50 + 170) };

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

            // click to display coords to std::cout
            cout << "x: " << e->xbutton.x << endl;
            cout << "y: " << e->xbutton.y << endl;
            cout << particle.get_info();

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
            // spawn_cloud(e->xbutton.x, e->xbutton.y, 0);
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
                // make a particle when left mouse button is pressed
                make_particle(box[0].pos[0], box[1].pos[1]+50);
                g.n = 1;
                g.state = 1;
                
                break;
            case XK_Escape:
                //Escape key was pressed
                return 1;

            case XK_a:
                particle.set_vel(-2,0,0);

                break;
            case XK_d:
                particle.set_vel(-2,0,0);


                break;
            case XK_space:

                break;
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

    // box[0].set_vel(0.5,0.5);
    // box[1].set_vel(0,5);
    // box[2].set_vel(-2,5);
    // box[3].set_vel(2,5);
    // box[4].set_vel(0,5);
    for (int i = 0; i < 5; i++)
    {
        box[i].stop();
    }
    
    

    //Do this to allow fonts
    glEnable(GL_TEXTURE_2D);
    initialize_fonts();


}

void move_all()
{
    particle.move();
    for (int i = 0; i < 5; i++) {
        box[i].move();
    }
    if (g.n > 1) {
        for (int i = 1; i < g.n; i++)
        {
          
            if (cloud[i].vel[0] > 100) {
                cloud[i].vel[0] -= 25;
            } else if (cloud[i].vel[0] < -100) {
                cloud[i].vel[0] += 25;
            } 

            if (cloud[i].vel[1] > 100) {
                cloud[i].vel[1] -= 25;
            } else if (cloud[i].vel[1] < -100) {
                cloud[i].vel[1] += 25;
            } 

            cloud[i].move();
        }
    }
}

void physics()
{
    switch (g.state)
    {
    case 0: // waiting for particle to drop
        
        
        
        break;
        
    case 1: // particle is dropping to box[0]
        particle.apply_gravity();
        
        if (particle.y_equal(box[0])) {
                particle.vel[1] = 0;
                particle.noclip_y(box[0]);
                box[0].set_vel(2,0,0);
                particle.set_vel(2,0,0);
                g.state = 2;
            }

        break;
    case 2: // box[0] is moving to box[1]
        // box[0].move();
        // particle.move();
        // move_all();

        // if ( (box[0].pos[0] + box[0].w) >= (box[1].pos[0] - box[1].w) ) {
        if ( box[0].x_equal(box[1], "right") ) {
            // box 0 is touching box 1
            box[0].stop();
            particle.stop();
            particle.set_vel(8,6,0);
            particle.move();
            g.state = 3;
        }
        break;
    case 3: // particle is jumping to box[1] while box[0] falls;
        box[0].apply_gravity();
        // box[0].move();
        particle.apply_gravity();
        // particle.move();
        // move_all();


        // if (( (particle.pos[1] - particle.h) < (box[1].pos[1] + box[1].h) ) &&
        //     (particle.pos[1] - particle.h) > (box[1].pos[1] + box[1].h - 10)) {
        if (particle.y_equal(box[1])){
                particle.stop();
                particle.noclip_y(box[1]);
                cout << "hit box...\n" << particle.get_info();
                // box[1].move();
                // particle.move();
                g.state = 4;
        }

        break;

    case 4: // particle and box[1] are falling to box[2]
        // move_all();
        particle.apply_gravity();
        box[1].apply_gravity();
        // particle.move();
        // box[1].move();

        if (particle.y_equal(box[2])) {
            particle.stop();
            // particle.pos[1] = (box[2].pos[1] + box[2].h + particle.h);
            particle.noclip_y(box[2]);
            particle.set_vel(-2,0,0);
            box[2].set_vel(-2,0,0);
            g.state = 5;

        }
        break;
    case 5: // particle and box[2] are moving to box[3]
        // box[1].move(); // to keep it falling off screen
        // box[2].move();
        // particle.move();
        // move_all();

        if (box[2].x_equal(box[3], "left")) {
            box[2].stop();
            box[2].move();
            particle.stop();
            particle.set_vel(-8,6,0);
            // particle.move();
            
            g.state = 6;
        }

        break;
    case 6:     // particle is jumping to box[3] while box[2] falls
        // particle.move();
        // move_all();
        particle.apply_gravity();
        box[2].apply_gravity();
        // box[2].move();


        if (particle.y_equal(box[3]) && 
                (particle.pos[0] > (box[3].pos[0]-box[3].w) &&
                particle.pos[0] < (box[3].pos[0]+box[3].w))) {
            cout << "y's were equal bro" << endl;
            particle.noclip_y(box[3]);
            particle.set_vel(2,2,0);
            box[3].set_vel(0,2,0);
            g.state = 7;
        }
        
        break;
    case 7: // particle and box[3] are rising to box[4]'s level
        // particle.move();
        // box[3].move();
        // move_all();

        if ( ((particle.pos[0]+ particle.w) >= (box[3].pos[0] + box[3].w)) &&
            ((particle.pos[0]+ particle.w) <= (box[3].pos[0] + box[3].w + 10)) ) {
            particle.vel[0]=0;
            // particle.stop();
            particle.noclip_x(box[3], "right");
        }
        if ( ((box[3].pos[1]) >= (box[4].pos[1])) &&
                ((box[3].pos[1]) <= (box[4].pos[1] + 10)) ) {
            box[3].stop();
            // box[3].vel[1]=0;
            box[3].pos[1] = box[4].pos[1];
            // particle.stop();
            
        }
            // both are stopped
        if ((particle.vel[0] < 0.01f) &&
            (box[3].vel[1] < 0.01f)) {

                particle.set_vel(4,4,0);
                g.state = 8;
        }
        

        break;

    case 8: // particle jumping to box[4] 
        // move_all();
        box[3].apply_gravity();
        particle.apply_gravity();
        // box[3].move();
        // particle.move();
        if (particle.y_equal(box[4])) {
            particle.vel[0] = 2;
            // particle.set_vel(2,0,-0.05);
            particle.noclip_y(box[4]);
            // box[4].vel[2] = -0.05;
            g.state = 9;
        }
        

        break;
    
    case 9:
        
        // if (particle.pos[0] < box[4].pos[0]) {
        //     particle.vel[0] = 2;
        // } else {
        //     particle.vel[0] = -2;
        // }
        // spawn_cloud();
        particle.noclip_y(box[4]);
        particle.apply_gravity();
        box[4].apply_gravity();
        if (((particle.pos[1] + particle.h ) < 0.0f) &&
            ((box[4].pos[1] + box[4].h) < 0.0f)) {
            g.state = 10;
        }

        break;

    case 10:
        // spawn_cloud();
        for (int i = 1; i < g.n; i++) {
            // accel
            if (cloud[i].vel[0] > 20) {
                cloud[i].accel[0] = -1;
            } else if (cloud[i].vel[0] < -20) {
                cloud[i].accel[0] = 1;
            } 

            if (cloud[i].vel[1] > 20) {
                cloud[i].accel[1] = -1;
            } else if (cloud[i].vel[1] < -20) {
                cloud[i].accel[1] += 1;
            } 
        }

        break;

    default:
        break;
    }
    
    
        

}

void render()
{
    

    glClear(GL_COLOR_BUFFER_BIT);
    //Rect r[5]; // was laggy so i moved it up to Global so that it's
    // only created once

    string words[5] = {"Requirements", "Design", "Coding",
        "Testing", "Maintenance"};

    if (g.state == 0) {
        g.r[5].bot  = g.yres - 20;
        g.r[5].left = 10;
        g.r[5].center = 0;
        ggprint8b(g.r+5, 16, 0x00ffff00, "Press 1 to start the Machine");
    }

    // if (g.state < 9) {

    for (int i = 0; i < 5; i++) {
        // Draw box
        glPushMatrix();
        //glColor3ub(150, 160, 220);
        glColor3ubv(box[i].color);
            
        glTranslatef(box[i].pos[0], box[i].pos[1], box[i].pos[2]);

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
    } else if (g.n > 1) {
        
        for (int i = 1; i < g.n; i++)
        {
            glPushMatrix();

            // glColor3ub(150, 160, 220);
            // particle.fade_color();
            
            glColor3ubv(cloud[i].color);

            glTranslatef(cloud[i].pos[0], cloud[i].pos[1], 0.0f);

            glBegin(GL_QUADS);
                glVertex2f(-cloud[i].w, -cloud[i].h);
                glVertex2f(-cloud[i].w,  cloud[i].h);
                glVertex2f( cloud[i].w,  cloud[i].h);
                glVertex2f( cloud[i].w, -cloud[i].h);
            glEnd();
            glPopMatrix();

        }

    }


    }



    // } else if (g.state == 9) {

    //     glPushMatrix();
    //     //glColor3ub(150, 160, 220);
    //     glColor3ubv(box[4].color);
            
    //     glTranslatef(box[4].pos[0], box[4].pos[1], box[4].pos[2]);

    //     glBegin(GL_QUADS);
    //         glVertex2f(-box[4].w, -box[4].h);
    //         glVertex2f(-box[4].w,  box[4].h);
    //         glVertex2f( box[4].w,  box[4].h);
    //         glVertex2f( box[4].w, -box[4].h);
    //     glEnd();

    //     glPopMatrix();

       

    //     glPushMatrix();

    //         // glColor3ub(150, 160, 220);
    //         // particle.fade_color();
    //         particle.double_fade();
    //         glColor3ubv(particle.color);

    //         glTranslatef(particle.pos[0], particle.pos[1], particle.pos[2]);

    //         glBegin(GL_QUADS);
    //             glVertex2f(-particle.w, -particle.h);
    //             glVertex2f(-particle.w,  particle.h);
    //             glVertex2f( particle.w,  particle.h);
    //             glVertex2f( particle.w, -particle.h);
    //         glEnd();
    //         glPopMatrix();

    //     if (particle.w > 0.24) {
    //         box[4].w *= 0.98;
    //         box[4].h *= 0.98;
    //         particle.w *= 0.98;
    //         particle.h *= 0.98;
    //     } else  {// if (particle.pos[2] <= -200) 
    //         g.state = 10;
    //     }
    // } else if (g.state == 10) {
    //     particle.double_fade();
    //     glColor3ubv(particle.color);

    //     glTranslatef(particle.pos[0], particle.pos[1], 0.0f);

    //     glBegin(GL_QUADS);
    //         glVertex2f(-particle.w, -particle.h);
    //         glVertex2f(-particle.w,  particle.h);
    //         glVertex2f( particle.w,  particle.h);
    //         glVertex2f( particle.w, -particle.h);
    //     glEnd();
    //     glPopMatrix();

    //     particle.w *= 1.2;
    //     particle.h *= 1.2;
    // }

// }







