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
using namespace std;
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

const int MAX_PARTICLES = 5000;
class Box{

public:
    // vars
    float w, h;
    float vel[2];
    float pos[2]; 
    unsigned char color[3];

    // setters
    void set_color(unsigned char col[3]){
        memcpy(color, col, sizeof(unsigned char) * 3);
    }
    void set_location(float x, float y){
        pos[0] = x;
        pos[1] = y;
    }

    // constructor
    Box() {
        w = 80.0f;
        h = 20.0f;
        pos[0] = g.xres/2.0f;
        pos[1] = g.yres/2.0f;
        vel[0] = 0.0f;
        vel[1] = 0.0f;
    }
    
    // parameterized constructor from class (didn't use)
    Box(float wid, float hgt, float x, float y, float v0, float v1) {
        w = wid;
        h = hgt;
        pos[0] = x;
        pos[1] = y;
        vel[0] = v0;
        vel[1] = v1;
    }
    

//} box[5], particle[MAX_PARTICLES];
} box[5], particle[MAX_PARTICLES];




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

//Function prototypes
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
    n = 0;


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

void make_particle(int x, int y){
    if (g.n < MAX_PARTICLES){
        particle[g.n].w = 4;
        particle[g.n].h = 4;
        particle[g.n].pos[0] = x;
        particle[g.n].pos[1] = y;
        //particle[g.n].vel[0] = ((float)rand() / (float)RAND_MAX) * 0.2 - 0.1;
        //particle[g.n].vel[1] = ((float)rand() / (float)RAND_MAX) * 0.2 - 0.1;
        particle[g.n].vel[0] = rnd() * 0.2 - 0.1;
        particle[g.n].vel[1] = rnd() * 0.2 - 0.1;
        unsigned char c[3] = {0, (unsigned char)((int)rand() % 50 + 150), (unsigned char)((int)rand() % 100 + 150)};
        particle[g.n].set_color(c);
        //particle[g.n].vel[1] = 0.0f;
        ++g.n;

    }
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
	//
	if (e->type == ButtonRelease) {
		return;
	}
	if (e->type == ButtonPress) {
		if (e->xbutton.button==1) {
			//Left button was pressed.
            // origin is flipped between display and mouse origin
            //     so we need to flip the y coord
			//int y = g.yres - e->xbutton.y;
            make_particle(e->xbutton.x, g.yres - e->xbutton.y);
            
            /*
            if (g.n < MAX_PARTICLES){
                particle[g.n].w = 4;
                particle[g.n].h = 4;
                particle[g.n].pos[0] = e->xbutton.x;
                particle[g.n].pos[1] = g.yres - e->xbutton.y;
                particle[g.n].vel[0] = particle[g.n].vel[1] = 0.0f;
                ++g.n;
            }
            */
			return;
		}
		if (e->xbutton.button==3) {
			//Right button was pressed.
            g.clickx = e->xbutton.x;
            g.clicky = e->xbutton.y;
            g.mouse_pressed = !g.mouse_pressed;
			return;
		}
	}
	if (e->type == MotionNotify) {
		//The mouse moved!
		if (savex != e->xbutton.x || savey != e->xbutton.y) {
			savex = e->xbutton.x;
			savey = e->xbutton.y;
			//Code placed here will execute whenever the mouse moves.
            
            for (int i = 0; i < 5; i++){
                make_particle(e->xbutton.x, g.yres - e->xbutton.y);

                /*
                if (g.n < MAX_PARTICLES){
                    particle[g.n].w = 4;
                    particle[g.n].h = 4;
                    particle[g.n].pos[0] = e->xbutton.x;
                    particle[g.n].pos[1] = g.yres - e->xbutton.y;
                    particle[g.n].vel[0] = particle[g.n].vel[1] = 0.0f;
                    ++g.n;
                }
                */
            }

		}
	}

    
}

int X11_wrapper::check_keys(XEvent *e)
{
	if (e->type != KeyPress && e->type != KeyRelease)
		return 0;
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
    unsigned char c[3] = {100, 200, 100};
    for (int i = 0; i < 5; i++) {

        box[i].set_color(c);

    }


    // position the text boxes
    box[0].set_location(box[0].w + 50, g.yres - box[0].h - 75);
    box[1].set_location(box[0].pos[0] + 75, box[0].pos[1] - 75);
    box[2].set_location(box[1].pos[0] + 75, box[1].pos[1] - 75);
    box[3].set_location(box[2].pos[0] + 75, box[2].pos[1] - 75);
    box[4].set_location(box[3].pos[0] + 75, box[3].pos[1] - 75);

	//Do this to allow fonts
	glEnable(GL_TEXTURE_2D);
	initialize_fonts();
}

const float GRAVITY = 0.05;
void physics()
{
    bool is_box_collision, is_circ_collision;
    float radius_vec[2], resultant;
    //if (g.n > 0){

    if (g.mouse_pressed == true){
        make_particle(g.clickx, g.yres - g.clicky);
    }
    for (int i = 0; i < g.n; i++){
        is_circ_collision = is_box_collision = false;
        particle[i].vel[1] -= GRAVITY;
        particle[i].pos[0] += particle[i].vel[0];
        particle[i].pos[1] += particle[i].vel[1];
    // check if particle went off screen...
        if (particle[i].pos[1] < 0.0f) {
            particle[i] = particle[--g.n];
        }           // subtract from box.h because that is middle of the box

        // check of it hits either box 0,1,2,3, or 4
        // if (((particle[i].pos[1] <= box[0].pos[1]+box[0].h) &&
        //                 (particle[i].pos[0] > box[0].pos[0]-box[0].w) &&
        //                 (particle[i].pos[0] < box[0].pos[0]+box[0].w)) ||
        //         ((particle[i].pos[1] <= box[1].pos[1]+box[1].h) &&
        //                     (particle[i].pos[0] > box[1].pos[0]-box[1].w) &&
        //                     (particle[i].pos[0] < box[1].pos[0]+box[1].w)) || 
        //         ((particle[i].pos[1] <= box[2].pos[1]+box[2].h) &&
        //                     (particle[i].pos[0] > box[2].pos[0]-box[2].w) &&
        //                     (particle[i].pos[0] < box[2].pos[0]+box[2].w)) || 
        //         ((particle[i].pos[1] <= box[3].pos[1]+box[3].h) &&
        //                     (particle[i].pos[0] > box[3].pos[0]-box[3].w) &&
        //                     (particle[i].pos[0] < box[3].pos[0]+box[3].w)) || 
        //         ((particle[i].pos[1] <= box[4].pos[1]+box[4].h) &&
        //                     (particle[i].pos[0] > box[4].pos[0]-box[4].w) &&
        //                     (particle[i].pos[0] < box[4].pos[0]+box[4].w)))
        // {
        //     particle[i].vel[1] = -particle[i].vel[1] * 0.3f;
        //     particle[i].vel[0] += 0.01; // so it has a tendency to move right
            
        // }
        
        // attempt at cleaning up the above code
        for (int j = 0; j < 5; j++){
            if ((particle[i].pos[1] <= box[j].pos[1]+box[j].h) &&
                        (particle[i].pos[0] > box[j].pos[0]-box[j].w) &&
                        (particle[i].pos[0] < box[j].pos[0]+box[j].w)){
                is_box_collision = true;
                break;
            }
            else{
                is_box_collision = false;
            }
        }

        radius_vec[0] = (g.xres - particle[i].pos[0]); // x
        radius_vec[1] = (particle[i].pos[1]);  // y
        resultant = sqrt( pow(radius_vec[0], 2) + pow(radius_vec[1],2) );

        if (resultant < g.circ_rad){
           // vector to center of circle == radius of circle
            is_circ_collision = true;
        }
        else{
            is_circ_collision = false;
        }

        if (is_box_collision)
        {
            particle[i].vel[1] = -particle[i].vel[1] * 0.3f;
            particle[i].vel[0] += 0.01; // so it has a tendency to move right
        }
        else if (is_circ_collision)
        {
            particle[i].vel[1] = -particle[i].vel[1] * 0.3f;
            particle[i].vel[0] = -particle[i].vel[0];

            // cout << "radius_vec[x]: " << radius_vec[0] << ", radius_vec[y]: " << radius_vec[1] << endl;
            // cout << "particle_vel[x]: " << particle[i].vel[0] << ", particle_vel[y]: " << particle[i].vel[1] << endl;
            // particle[i].vel[1] -= radius_vec[1];
            // particle[i].vel[0] -= radius_vec[0];


        }
    }

    
    
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
	
    for (int i = 0; i < 5; i++){
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
        g.r[i].center = box[i].pos[0];
        
        ggprint8b(g.r+i, 16, 0x00ffff00, words[i].c_str());
        
    }

    //int b = rand() % 50 + 200;
    // Draw particle
    for (int i = 0; i < g.n; i++) {
        glPushMatrix();
        
        // glColor3ub(150, 160, 220);
        glColor3ubv(particle[i].color);

        glTranslatef(particle[i].pos[0], particle[i].pos[1], 0.0f);

        glBegin(GL_QUADS);
            glVertex2f(-particle[i].w, -particle[i].h);
            glVertex2f(-particle[i].w,  particle[i].h);
            glVertex2f( particle[i].w,  particle[i].h);
            glVertex2f( particle[i].w, -particle[i].h);
        glEnd();
        glPopMatrix();
    }

    


    // trying to get a circle to work

    glPushMatrix();
    glColor3f(255,255,0);

    glTranslatef(g.xres, 0, 0.0f);

    float angle;
    glBegin(GL_TRIANGLE_FAN);
        for(int i = 0; i < 100; i++){
            angle = (2 * PI * i)/100;
            glVertex2f(cos(angle) * g.circ_rad, sin(angle) * g.circ_rad);
        }
    glEnd();
    glPopMatrix();

}






