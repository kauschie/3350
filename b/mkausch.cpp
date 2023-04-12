/********************************************************************
*
*		Program: mkausch.cpp
*		Authors: Michael Kausch (Group 1 - DAHM)
*		Date Code Originated: 2/17/23
***********************************************************************/

/***************************************************************************
*       Classes: 
**********************************************************************
*
*               Menu
*
*       Uses:
*           Creates a menu object based on num text params, size and position
*
*               Timer
*       Uses:
*           Creates a timer that will return when its done or you can use it to
*           just count upwards
*
*
*               Sound
*       Uses:
*           encapsulates all sound and soundfx for the game
*
*              
*               PowerBar
*       Uses:
*           draws powerbars for health / energy
*
*      
*               Blocky
*
*       Uses:
*           creates either a horizontal or vertical blocky type enemy
*
*               Bomb
*       
*       Uses:
*           creates a bomb weapon that explodes and kills all enemeies 
*               in the affected area
*
*               
*               Gamerecord
*
*       Uses:
*           manages i/o of highscore / record data
*           creates and draws highscore leaderboard
*
*
*               SoundBar
*
*       Uses:
*           creates clickable soundbars for music and sound effects
*           
**********************************************************************
*       Structs:
**********************************************************************
*         
*               HighScore
*
*       Uses:
*           encapsulates a highscore datum
*
*
*
**********************************************************************
*       Functions:
**********************************************************************
*
*           checkLevel()
*
*       Uses:
*           - monitors gametime and changes the game level state as needed
*           - enables / disables enemy types depending on the game state
*
*
*           setRandColor()
*       Uses:
*           - sets a random color to the passed in item
*
*           checkSound()
*       Uses:
*           - manages sound / sound effects for the game
******************************************************************************/

//                 		INCLUDES

#include <iostream>
#include <string>
#include </usr/include/AL/alut.h>
#include <math.h>
#include <new>
#include <sstream>
#include <GL/glx.h>
#include <iostream>
#include <iomanip>
#include <chrono>
#include <time.h>
#include <cstdlib>

#include "Global.h"
#include "mkausch.h"
//#include "hzhang.h"
#include "aparriott.h"


using namespace std;

#define BSIZE 5
#define PADDING 20
#define PI 3.1415926535

// #define USE_OPENAL_SOUND



Menu::Menu(unsigned int _n_texts,
            float w, float h,
            float x, float y,
            std::string* _words,
            int _centering)
    : n_texts{_n_texts}, pos{x, y, 0}, centering(_centering)
{
    // dynamially allocate boxes/rects for text display
    mainbox.setDim(w, h);
    mainbox.setPos(pos[0], pos[1], pos[2]);
    mainbox.setColor(47, 61, 64);
    boarder.setDim(w+BSIZE, h+BSIZE);
    boarder.setPos(pos[0], pos[1], pos[2]);
    boarder.setColor(69, 85, 89);


    try {
        t_boxs = new Box[n_texts];
        texts = new Rect[n_texts];
        words = new std::string[n_texts];


        float spacing = (2*h)/(n_texts+1);
        std::cout << "spacing: " << spacing << std::endl;

        for (int i = 0; i < n_texts; i++) {
            t_boxs[i].w = mainbox.w - PADDING;
            t_boxs[i].h = (spacing/2.0) - BSIZE;
            t_boxs[i].pos[0] = mainbox.pos[0];
            t_boxs[i].pos[1] = (pos[1]+mainbox.h)-((i+1)*spacing);
            t_boxs[i].setColor(61, 90, 115);
            t_boxs[i].id=i; 

            words[i] = _words[i];

        }

    } catch (std::bad_alloc& ba) {
        // if one was allocated and not the other than delete the one that
        if (texts) 
            delete [] texts;
        if (t_boxs) 
            delete [] t_boxs;
        std::cerr << "Error allocating rectangles in Menu call\n"
                << ba.what() << '\n';
        texts = nullptr;
        t_boxs = nullptr;
    }
}

Menu::~Menu()
{
    if (texts)
        delete [] texts;

    if (t_boxs)
        delete [] t_boxs;

    if (words)
        delete [] words;

}

// draws all sub boxes and text 
void Menu::draw()
{
    // draw boarder

    glColor3ubv(boarder.color);

    glPushMatrix();
    glTranslatef(boarder.pos[0], boarder.pos[1], boarder.pos[2]);
    glBegin(GL_QUADS);
        glVertex2f(-boarder.w, -boarder.h);
        glVertex2f(-boarder.w,  boarder.h);
        glVertex2f( boarder.w,  boarder.h);
        glVertex2f( boarder.w, -boarder.h);
    glEnd();
    glPopMatrix();

    // draw mainbox

    glColor3ubv(mainbox.color);

    glPushMatrix();
    glTranslatef(mainbox.pos[0], mainbox.pos[1], mainbox.pos[2]);
    glBegin(GL_QUADS);
        glVertex2f(-mainbox.w, -mainbox.h);
        glVertex2f(-mainbox.w,  mainbox.h);
        glVertex2f( mainbox.w,  mainbox.h);
        glVertex2f( mainbox.w, -mainbox.h);
    glEnd();
    glPopMatrix();

    // draw all t_boxes

    for (size_t i = 0; i < n_texts; i++) {
        glColor3ubv(t_boxs[i].color);

        glPushMatrix();
        glTranslatef(t_boxs[i].pos[0], t_boxs[i].pos[1], 0.0f);
        glBegin(GL_QUADS);
            glVertex2f(-t_boxs[i].w, -t_boxs[i].h);
            glVertex2f(-t_boxs[i].w,  t_boxs[i].h);
            glVertex2f( t_boxs[i].w,  t_boxs[i].h);
            glVertex2f( t_boxs[i].w, -t_boxs[i].h);
        glEnd();
        glPopMatrix();

    }

    for (int i = 0; i < n_texts; i++) {
        texts[i].bot = t_boxs[i].pos[1] - 5;
        if (!centering)
            texts[i].left = t_boxs[i].pos[0]-t_boxs[i].w + 100;
        else
            texts[i].left = t_boxs[i].pos[0];
        texts[i].center = centering;

        ggprint8b(texts+i, 0, 0x00ffffff, words[i].c_str());
    }

}

// pass in mouse coords to check and see if they are within the bounds
// of the menu's text boxes
Box* Menu::checkTBox(int x, int y)
{
    Box * box_ptr = nullptr;
    size_t i;

    for (i = 0; i < n_texts; i++) {
        // mouse coords are
        if ((x > (t_boxs[i].pos[0]-t_boxs[i].w)) &&
            (x < (t_boxs[i].pos[0]+t_boxs[i].w)) &&
            (y > (t_boxs[i].pos[1]-t_boxs[i].h)) &&
            (y < (t_boxs[i].pos[1]+t_boxs[i].h))) {

            box_ptr = (t_boxs+i);
            break;
        }
    }

    return box_ptr;
}

// sets the highlight color of a sub_box
void Menu::setHighlight(Box * b)
{
    b->setColor(33,136,171);
}

// sets all sub boxes to the original color
void Menu::setOrigColor()
{
    for (size_t i = 0; i < n_texts; i++) {
        t_boxs[i].setColor(61,90,115);
    }
}



void Menu::setBcolor(int r, int g, int b)
{
    bcolor[0] = (char)r;
    bcolor[1] = (char)g;
    bcolor[2] = (char)b;
}

void Menu::setColor(int r, int g, int b)
{
    color[0] = (char)r;
    color[1] = (char)g;
    color[2] = (char)b;
}

void Menu::setPos(float x, float y, float z)
{
    pos[0] = x;
    pos[1] = y;
    pos[2] = z;
}

std::string Menu::getInfo()
{
    std::ostringstream temp;
    temp << std::endl;

    return temp.str();
}


/***************************************************************************
*       Class: Timer
*       Uses:
*           Creates a Timer object with duration based on input
******************************************************************************/

Timer::Timer() : duration(-1), pause_duration(0.00),
                pause_timer(nullptr), paused(false)
{    // set starting time
    start = std::chrono::system_clock::now();
}

Timer::Timer(double sec) : duration(sec), pause_duration(0.00),
                pause_timer(nullptr), paused(false)
{    // set starting time
    start = std::chrono::system_clock::now();
}

// delete pause timer if it were active
Timer::~Timer()
{
    cerr << "in Timer destructor\n";
    if (pause_timer) {
        cerr << "deleting pause timer\n";
        delete pause_timer;
        pause_timer = nullptr;
    }
}

/****************************** Setters *************************************/

// resets timer to current time
void Timer::reset()
{
    if (isPaused()) {
        unPause();
    }
    pause_duration = 0;
    start = std::chrono::system_clock::now();
}

/****************************** Getters *************************************/

// returns time that has elapsed since the start of the timer
int Timer::getTime(char time_code)
{
    int net_time = 0;
    int time = net_time;
    std::chrono::duration<double> total_elapsed = std::chrono::system_clock::now() - start;

    if (paused) {
        net_time = (total_elapsed.count() - pause_duration - pause_timer->getTime('n'));
    } else {
        net_time = (total_elapsed.count()-pause_duration);
    }

    // D.T - retrieve minutes, seconds, or net time
    // based on time_code passed in getTime parameter
    switch(time_code) {
        case 'm': time = net_time/60;
                  break;
        case 's': time = net_time % 60;
                  break;
        case 'n': time = net_time;
    }
    return time;
}

// checks if the timer has elapsed
// true if the timer has finished
// false if the timer hasn't
bool Timer::isDone()
{
    if (duration == -1) {   // return false for count up timers
        return false;
    } else {    // return net time for countdown timers
        return (getTime('n') > duration);
    }
}

void Timer::pause()
{
    paused = true;
    pause_timer = new Timer();
}

bool Timer::isPaused()
{
    return paused;
}

void Timer::unPause()
{
    if (paused) {
        paused = false;
        pause_duration += pause_timer->getTime('n');
        delete pause_timer;
        pause_timer = nullptr;
    }
}

#ifdef USE_OPENAL_SOUND
Sound::Sound()
{
    //Buffer holds the sound information.
    initOpenal();
    current_track = -1;  // starting track number at splash screen
    is_music_paused = false;
    user_pause = false;
    is_intro = is_game = false;
    m_vol = g.m_vol;
    sfx_vol  = g.sfx_vol;
    


    // make individual buffers of all sounds
    alBuffers[0] = 
        alutCreateBufferFromFile(buildSongPath(sound_names[0]).c_str());
    alBuffers[1] = 
        alutCreateBufferFromFile(buildSongPath(sound_names[1]).c_str());
    alBuffers[2] = 
        alutCreateBufferFromFile(buildSongPath(sound_names[2]).c_str());
    alBuffers[3] = 
        alutCreateBufferFromFile(buildSongPath(sound_names[3]).c_str());
    alBuffers[4] = 
        alutCreateBufferFromFile(buildSongPath(sound_names[4]).c_str());
    alBuffers[5] = 
        alutCreateBufferFromFile(buildSongPath(sound_names[5]).c_str());
    alBuffers[6] = 
        alutCreateBufferFromFile(buildSongPath(sound_names[6]).c_str());
    alBuffers[7] = 
        alutCreateBufferFromFile(buildSongPath(sound_names[7]).c_str());
    alBuffers[8] = 
        alutCreateBufferFromFile(buildSongPath(sound_names[8]).c_str());
    alBuffers[9] = 
        alutCreateBufferFromFile(buildSongPath(sound_names[9]).c_str());
    alBuffers[10] = 
        alutCreateBufferFromFile(buildSongPath(sound_names[10]).c_str());
    alBuffers[11] = 
        alutCreateBufferFromFile(buildSongPath(sound_names[11]).c_str());
    alBuffers[12] = 
        alutCreateBufferFromFile(buildSongPath(sound_names[12]).c_str());
    alBuffers[13] = 
        alutCreateBufferFromFile(buildSongPath(sound_names[13]).c_str());
    alBuffers[14] = 
        alutCreateBufferFromFile(buildSongPath(sound_names[14]).c_str());
    alBuffers[15] = 
        alutCreateBufferFromFile(buildSongPath(sound_names[15]).c_str());


    // songBuffers[0] = alBuffers[3];
    buffersDone = buffersQueued = 0;

    // generate number of sources
    alGenSources(NUM_SOUNDS, alSources);    // keep individual sources for now
    alGenSources(1, &menuQueueSource);

    alSourceQueueBuffers(menuQueueSource, 1, (alBuffers+1));
    alSourcef(menuQueueSource, AL_GAIN, 1.0f);
    alSourcef(menuQueueSource, AL_PITCH, 1.0f);

    alSourcei(alSources[0], AL_BUFFER, alBuffers[0]);   // bullet_fire
    alSourcef(alSources[0], AL_GAIN, g.sfx_vol);
    alSourcef(alSources[0], AL_PITCH, 1.0f);
    alSourcei(alSources[0], AL_LOOPING, AL_TRUE);

    alSourcei(alSources[1], AL_BUFFER, alBuffers[1]);   // intro
    alSourcef(alSources[1], AL_GAIN, g.m_vol);
    alSourcef(alSources[1], AL_PITCH, 1.0f);
    alSourcei(alSources[1], AL_LOOPING, AL_FALSE);

    alSourcei(alSources[2], AL_BUFFER, alBuffers[2]);   // beep
    alSourcef(alSources[2], AL_GAIN, g.sfx_vol);
    alSourcef(alSources[2], AL_PITCH, 1.0f);
    alSourcei(alSources[2], AL_LOOPING, AL_FALSE);

    alSourcei(alSources[3], AL_BUFFER, alBuffers[3]); // boop
    alSourcef(alSources[3], AL_GAIN, g.sfx_vol);
    alSourcef(alSources[3], AL_PITCH, 1.0f);
    alSourcei(alSources[3], AL_LOOPING, AL_FALSE);

    alSourcei(alSources[4], AL_BUFFER, alBuffers[4]); // intro's loop
    alSourcef(alSources[4], AL_GAIN, g.m_vol);
    alSourcef(alSources[4], AL_PITCH, 1.0f);
    alSourcei(alSources[4], AL_LOOPING, AL_TRUE);

    alSourcei(alSources[5], AL_BUFFER, alBuffers[5]); // AdhesiveWombat song
    alSourcef(alSources[5], AL_GAIN, g.m_vol);
    alSourcef(alSources[5], AL_PITCH, 1.0f);
    alSourcei(alSources[5], AL_LOOPING, AL_TRUE);

    alSourcei(alSources[6], AL_BUFFER, alBuffers[6]); // zap1
    alSourcef(alSources[6], AL_GAIN, g.sfx_vol);
    alSourcef(alSources[6], AL_PITCH, 1.0f);
    alSourcei(alSources[6], AL_LOOPING, AL_TRUE);


    alSourcei(alSources[7], AL_BUFFER, alBuffers[7]); // zap2
    alSourcef(alSources[7], AL_GAIN, g.sfx_vol);
    alSourcef(alSources[7], AL_PITCH, 1.0f);
    alSourcei(alSources[7], AL_LOOPING, AL_TRUE);


    alSourcei(alSources[8], AL_BUFFER, alBuffers[8]); // zap3
    alSourcef(alSources[8], AL_GAIN, g.sfx_vol);
    alSourcef(alSources[8], AL_PITCH, 1.0f);
    alSourcei(alSources[8], AL_LOOPING, AL_TRUE);


    alSourcei(alSources[9], AL_BUFFER, alBuffers[9]); // zap4
    alSourcef(alSources[9], AL_GAIN, g.sfx_vol);
    alSourcef(alSources[9], AL_PITCH, 1.0f);
    alSourcei(alSources[9], AL_LOOPING, AL_TRUE);

    alSourcei(alSources[10], AL_BUFFER, alBuffers[10]); // shield
    alSourcef(alSources[10], AL_GAIN, g.sfx_vol);
    alSourcef(alSources[10], AL_PITCH, 1.0f);
    alSourcei(alSources[10], AL_LOOPING, AL_FALSE);

    alSourcei(alSources[11], AL_BUFFER, alBuffers[11]); // doosh
    alSourcef(alSources[11], AL_GAIN, g.sfx_vol);
    alSourcef(alSources[11], AL_PITCH, 1.0f);
    alSourcei(alSources[11], AL_LOOPING, AL_FALSE);

    alSourcei(alSources[12], AL_BUFFER, alBuffers[12]); // doosh2
    alSourcef(alSources[12], AL_GAIN, g.sfx_vol);
    alSourcef(alSources[12], AL_PITCH, 1.0f);
    alSourcei(alSources[12], AL_LOOPING, AL_FALSE);

    alSourcei(alSources[13], AL_BUFFER, alBuffers[13]); // doosh
    alSourcef(alSources[13], AL_GAIN, g.sfx_vol);
    alSourcef(alSources[13], AL_PITCH, 1.0f);
    alSourcei(alSources[13], AL_LOOPING, AL_FALSE);

    alSourcei(alSources[14], AL_BUFFER, alBuffers[14]); // doosh2
    alSourcef(alSources[14], AL_GAIN, g.sfx_vol);
    alSourcef(alSources[14], AL_PITCH, 1.0f);
    alSourcei(alSources[14], AL_LOOPING, AL_FALSE);

    alSourcei(alSources[15], AL_BUFFER, alBuffers[15]); // zap2 - noloop
    alSourcef(alSources[15], AL_GAIN, g.sfx_vol);
    alSourcef(alSources[15], AL_PITCH, 1.0f);
    alSourcei(alSources[15], AL_LOOPING, AL_FALSE);

    // check for errors after setting sources
    if (alGetError() != AL_NO_ERROR) {
        throw "ERROR: setting source\n";
    }
    cout << "does it reach here???" << endl;

}

Sound::~Sound()
{
    for (int i = 0; i < NUM_SOUNDS; i++) {
        // delete sources
        alDeleteSources(i+1, (alSources+i));
        // delete buffers
        alDeleteBuffers(i+1, (alBuffers+i));
    }

    alDeleteSources(1, &menuQueueSource);

    closeOpenal();

}

void Sound::initOpenal()
{
	alutInit(0, NULL);
	if (alGetError() != AL_NO_ERROR) {
		throw "ERROR: alutInit()\n";
		// printf("ERROR: alutInit()\n");
		// return 0;
	}

	//Clear error state.
	alGetError();

	//Setup the listener.
	//Forward and up vectors are used.
	float vec[6] = {0.0f,0.0f,1.0f, 0.0f,1.0f,0.0f};
	alListener3f(AL_POSITION, 0.0f, 0.0f, 0.0f);
	alListenerfv(AL_ORIENTATION, vec);
	alListenerf(AL_GAIN, 1.0f);
}

void Sound::closeOpenal()
{
	//Close out OpenAL itself.
	//Get active context.
	ALCcontext *Context = alcGetCurrentContext();
	//Get device for active context.
	ALCdevice *Device = alcGetContextsDevice(Context);
	//Disable context.
	alcMakeContextCurrent(NULL);
	//Release context(s).
	alcDestroyContext(Context);
	//Close device.
	alcCloseDevice(Device);
}

void Sound::rewindGameMusic()
{
    alSourceStop(alSources[5]);
    alSourceRewind(alSources[5]);
    alSourcePlay(alSources[5]);
}

string Sound::buildSongPath(string s)
{
    // format of the song
    // ./Songs/Edzes-64TheMagicNumber16kHz.wav"

    ostringstream path;
    string song_dir = "Songs";

    path << "./" << song_dir << "/" << s;

    return path.str();
}

void Sound::gunPlay(int btype)
{
    // static int gun_start = 6;

    cerr << "gun shooting..." << endl;
    if (btype == 1) {
        alSourcePlay(alSources[6]);
    } else if (btype == 2) {
        alSourcePlay(alSources[7]);
        // alSourcePlay(alSources[gun_start+1]);
    } else if (btype == 3) {
        alSourcePlay(alSources[8]);
        // alSourcePlay(alSources[gun_start+1]);
        // alSourcePlay(alSources[gun_start+2]);
    } else if (btype == 4) {
        alSourcePlay(alSources[9]);
        // alSourcePlay(alSources[gun_start+3]);
    }
}

void Sound::gunStop()
{
    static int gun_start = 6;
    static int num_guns = 4;
    // cerr << "gun not shooting..." << endl;
    for (int i = 0; i < num_guns; i++) {
        alSourceStop(alSources[i+gun_start]);
    }
}

void Sound::beep()
{
    alSourcePlay(alSources[2]);
}

void Sound::boop()
{
    alSourcePlay(alSources[3]);
}

void Sound::doosh(int select)
{
    int index; // index of doosh sound effect 11 or 12 currently

    if (select == 0) {

        index = 12; // index of doosh sound effect 11 or 12 currently
    } else {
        index = 11;
    }
    alSourcePlay(alSources[index]);
}


void Sound::shieldSFX()
{
    int index = 10; // index of doosh sound effect 
    alSourcePlay(alSources[index]);
}

void Sound::exploSFX()
{
    int index = 14; // index of doosh sound effect 13 or 14 currently
    alSourcePlay(alSources[index]);
}

void Sound::playZap2()
{
    int index = 15; // index of zap2 (non-loop version) 
                    // for blocky crashes, 2nd gun
    alSourcePlay(alSources[index]);
}

bool Sound::checkIntroBufferDone()
{
    resetBufferDone();
    alGetSourcei(menuQueueSource, AL_BUFFERS_PROCESSED, &buffersDone);
    return (buffersDone == 1);
}

// resets buffers_done variable for further checks
void Sound::resetBufferDone()
{
    buffersDone = 0;
}

// unqueue's intro beat so that only loop track is in the buffer queue
// loops buffer queue at this point
void Sound::loopIntro()
{

    alSourceStop(menuQueueSource);
    alSourceRewind(menuQueueSource);
    alSourcePlay(alSources[4]);
}

void Sound::setupGameMode()
{
    // change bools for music state
    is_intro = false; is_game = true;

    // stop both the intro and loop if either are playing
    alSourceStop(alSources[4]);
    alSourceStop(menuQueueSource);
    alSourceRewind(alSources[4]);
    alSourceRewind(menuQueueSource);

    // play the game song
    alSourcePlay(alSources[5]);
}

void Sound::playStartTrack()
{
    // stop game music if it's playing
    if (is_game == true) {
        alSourceStop(alSources[5]);
        alSourceRewind(alSources[5]);
    }

    is_intro = true; is_game = false;

    // begin playing menu music
    alSourcePlay(menuQueueSource);
}

// returns song names, only 2 songs for now
string Sound::getSongName()
{
    string name;
    if (is_intro) {
        name = "Edzes-64TheMagicNumber";
    }
    if (is_game) {
        name = sound_names[5];
    }
    return name;
}

// pauses song (when going to pause menu for instance)
void Sound::pause()
{
    if (!is_music_paused) {
        is_music_paused = true;
        alSourcePause(alSources[5]);
    }
}

// unpauses
void Sound::unpause()
{
    if (!user_pause) {
        if (is_music_paused) {
            is_music_paused = false;
            alSourcePlay(alSources[5]);
        }
    }
}

// separate pause state for when user explicity mutes music
void Sound::toggleUserPause()
{
    user_pause = (user_pause == true) ? false : true;
    if (user_pause)
        pause();
    else
        unpause();
}

// getter to return the pause state
bool Sound::getPause()
{
    return is_music_paused;
}


void Sound::updateMusicVol()
{
    int n_songs = 3;
    int song_indices[n_songs] = {1, 4, 5};

    for (int i = 0; i < n_songs; i++) {
        alSourcef(alSources[song_indices[i]], AL_GAIN, g.m_vol);
    }

    alSourcef(menuQueueSource, AL_GAIN, g.m_vol);
}

void Sound::updateSFXVol()
{
    int n_sfx = 12;
    int sfx_indices[n_sfx] = {0, 2, 3, 6, 7, 8, 9, 10, 11, 12, 13, 14};

    for (int i = 0; i < n_sfx; i++) {
        alSourcef(alSources[sfx_indices[i]], AL_GAIN, g.sfx_vol);
    }
}


void Sound::bombExplosion()
{
    int bomb_index = 13;

    alSourcePlay(alSources[bomb_index]);
}

#endif

PowerBar::PowerBar(const Item & _itm_, PBType _type_, float x, float y)
{
    // maybe put max_health of each enemy type in case were going to
    // use this healthbar for the boss as well
    itm = &_itm_;
    type = _type_;

    if (type == HEALTH) {
        total.setColor(255,0,0);   // set lost health to red
        health.setColor(0,255,0);  // set health to green
        total.setDim(75,10);
        total.setPos(x, y, 0);
    } else if (type == COOLDOWN) {
        total.setColor(108,122,137);
        health.setColor(0,0,0);
        total.setDim(75,4);
        total.setPos(x, y, 0);
    }
    // mimic other bar based on what health was set to
    health.setDim(total.w,total.h);
    health.setPos(total.pos[0],total.pos[1],total.pos[2]);

    text.bot = total.pos[1]-5;
    text.left = total.pos[0];
    text.center = 1;
    cerr << "finished itm constructor" << endl;
}

PowerBar::PowerBar(const Toaster & _tos_, PBType _type_, float x, float y)
{
    // maybe put max_health of each enemy type in case were going to 
    // use this healthbar for the boss as well
    tos = &_tos_;
    itm = &_tos_;
    type = _type_;

    if (type == HEALTH) {
        total.setColor(255,0,0);   // set lost health to red
        health.setColor(0,255,0);  // set health to green
        total.setDim(75,10);
        total.setPos(x, y, 0);
    } else if (type == COOLDOWN) {
        total.setColor(196,145,2);
        health.setColor(255,255,0);
        total.setDim(75,8);
        total.setPos(x, y, 0);
    }
    // mimic other bar based on what health was set to
    health.setDim(total.w,total.h);
    health.setPos(total.pos[0],total.pos[1],total.pos[2]);

    text.bot = total.pos[1]-5;
    text.left = total.pos[0];
    text.center = 1;

    cerr << "finished tos constructor" << endl;
}


void PowerBar::draw()
{
    static int max_energy = 100;
    
    if (type == HEALTH) {
        glColor3ubv(total.color);
        glPushMatrix();
        glTranslatef(total.pos[0], total.pos[1], total.pos[2]);
        glBegin(GL_QUADS);
            glVertex2f(-total.w, -total.h);
            glVertex2f(-total.w,  total.h);
            glVertex2f( total.w,  total.h);
            glVertex2f( total.w, -total.h);
        glEnd();
        glPopMatrix();

        // draw mainbox
        // hp_resize();
        glColor3ubv(health.color);
        
        glPushMatrix();
        glTranslatef(health.pos[0]-health.w, health.pos[1], health.pos[2]);
        glBegin(GL_QUADS);
            glVertex2f(0, -health.h);
            glVertex2f(0,  health.h);
            glVertex2f((((float)(itm->hp))/(float)(itm->starting_hp))*2.0f*health.w,  health.h);
            glVertex2f((((float)(itm->hp))/(float)(itm->starting_hp))*2.0f*health.w, -health.h);
            
        glEnd();
        glPopMatrix();

        ggprint8b(&text, 0, 0x00000000, "%i/%i  Lives: %i", 
                            (int)(itm->hp), (int)itm->starting_hp, itm->lives);
    } else if (type == COOLDOWN) {

        glColor3ubv(total.color);
        glPushMatrix();
        glTranslatef(total.pos[0], total.pos[1], total.pos[2]);
        glBegin(GL_QUADS);
            glVertex2f(-total.w, -total.h);
            glVertex2f(-total.w,  total.h);
            glVertex2f( total.w,  total.h);
            glVertex2f( total.w, -total.h);
        glEnd();
        glPopMatrix();



        glColor3ubv(health.color);
        glPushMatrix();
        glTranslatef(health.pos[0]-health.w, health.pos[1], health.pos[2]);
        glBegin(GL_QUADS);
            glVertex2f(0, -health.h);
            glVertex2f(0,  health.h);
            glVertex2f( (((tos->energy))/((float)(max_energy)))*2.0f*health.w,  health.h);
            glVertex2f( (((tos->energy))/((float)(max_energy)))*2.0f*health.w, -health.h);
            
        glEnd();
        glPopMatrix();

        ggprint8b(&text, 0, 0x00FF0000, "Jump Energy: %i/%i", (int)tos->energy, max_energy);
        // cerr << "tos->energy: " << tos->energy << " max_energy: " << max_energy << endl;
    }
}

// modified from hzhang's file by mkausch
bool Entity::collision(Item & a)
{
    // for (x0,y0,x1,y1)and(x2,y2,x3,y3) squares
    // if collison -->(x0-x3)*(x1-x2)<0
    // same for y
    bool x = (((pos[0]+dim[0])-(a.pos[0]-a.w))*((pos[0]-dim[0])-(a.pos[0]+a.w))) < 0;
  	bool y = (((pos[1]+dim[1])-(a.pos[1]-a.h))*((pos[1]-dim[1])-(a.pos[1]+a.h))) < 0;
  	return x&&y;
}

void Entity::setHP(float life)
{
    hp = life;
}

void Entity::setDamage(float x)
{
    damage = x;
}

void Entity::hpDamage(Item & a) 
{
    hp = hp - a.damage;
}

bool Entity::hpCheck()
{
    return (hp < 0.01);
}

void Item::hpDamage(Entity & e)
{
    hp = hp - e.damage;
}

Blocky::Blocky(char type)
{
    srand(time(NULL));
    float sub_blocky_size = sqrt((25.0*100.0)/SUB_BLOCK_N);
    if (type == 'v') {
        setDim(25.0f, 100.0f);
    } else if (type == 'h') {
        setDim(100.0f, 25.0f);
    }
    setRandColor(*this);
    setRandPosition();
    setAcc(0.0f,-0.25f,0.0f);
    setVel(0.0f, -4.0f, 0.0f);
    setDamage(20);
    starting_hp = 10;
    setHP(starting_hp);
    point = starting_hp;
    was_hit = false;
    lives = 2;
    explode_done = true;

    // sub box assignment
    // assignes itself and it's mirror image (i+4 in this case)
    int angle = 80;
    float angle_offset = (angle*2/SUB_BLOCK_N);
    angle = 70;
    int rvel = 8;
    float deg_to_rad = (PI / 180.0f);
    for (int i = 0; i < SUB_BLOCK_N; i++) {
        sub_boxes[i].setDim(sub_blocky_size, sub_blocky_size);
        sub_boxes[i].setColor(255,0,0);    // make them red for now
        sub_boxes[i].setAcc(0, 0, 0);
        sb_angles[i] = angle;
        angle -= angle_offset;
        sub_boxes[i].setVel((rvel*cos(deg_to_rad * sb_angles[i])),
                                    (rvel*sin(deg_to_rad * sb_angles[i])), 0);
    }

    initRotationVel();
}

Blocky::~Blocky()
{

}

void Blocky::initRotationVel()
{
    // 0 the starting angle and assign random change in rotation angle
    for (int i = 0; i < SUB_BLOCK_N; i++) {
        rot_angle[i] = 0;
        rot_speed[i] = -40 + (rand() % 41);
    }
}

void Blocky::setRandPosition()
{
    static int pm_dir = 1;
    float curr_player_xpos = tos.pos[0];
    int delta_from_xpos = rand() % 50;
    float block_xpos = curr_player_xpos + (delta_from_xpos * pm_dir);

    // set to be this new random position situated near the player char
    // that is above the yres and out of view of the screen
    setPos(block_xpos, g.yres+h,0);

    // if this block was generated in front of the player then
    // next time make it randomly behind the player (it'll keep switching)
    pm_dir *= -1;
}

void setRandColor(Item & it)
{
    // colors based on color scheme defined at the bottom
    // int color[5][3] = {{61, 89, 114},
    //                     {47, 61, 63},
    //                     {68, 84, 89},
    //                     {40, 63, 61},
    //                     {24, 38, 36}};

    static int color[5][3] =   {{242, 4, 159},
                        {4, 177, 216},
                        {4, 216, 78},
                        {242, 202, 4},
                        {242, 135, 4}};
    static int index = rand() % 5;
    it.setColor(color[index][0], color[index][1], color[index][2]);
    index = (index + 1) % 5;
}

bool Blocky::subScreenIn()
{
    bool subs_onscreen = false;

    for (int i = 0; i < SUB_BLOCK_N; i++) {
        subs_onscreen = sub_boxes[i].screenIn();
        if (subs_onscreen)
            break;
    }

    return subs_onscreen;
}

void Blocky::draw()
{
    // static int rot_angle = 0;
    // draw item

        // reset blocky if he's out of screen

    // draw big blocky
    if (isAlive() && explode_done) {
        if (screenOut()) {
            reset();
        }

        setRandColor(*this);
        glPushMatrix();
        glColor3ub(color[0], color[1], color[2]);
        glTranslatef(pos[0], pos[1], pos[2]);
        glBegin(GL_QUADS);
                glVertex2f(-w, -h);
                glVertex2f(-w,  h);
                glVertex2f( w,  h);
                glVertex2f( w, -h);
        glEnd();
        glPopMatrix();

    } else {    // draw little blockies
        // cerr << "checking if sub boxes are in the screen...\n";
        if (subScreenIn()) {

            for (int i = 0; i < SUB_BLOCK_N; i++) {
                setRandColor(sub_boxes[i]);
                glPushMatrix();
                glColor3ub(sub_boxes[i].color[0],
                            sub_boxes[i].color[1],
                            sub_boxes[i].color[2]);
                glTranslatef(sub_boxes[i].pos[0], sub_boxes[i].pos[1], sub_boxes[i].pos[2]);
                glMatrixMode(GL_MODELVIEW);
                glRotatef(rot_angle[i], 0, 0, 1.0);
                glBegin(GL_QUADS);
                        glVertex2f(-sub_boxes[i].w, -sub_boxes[i].h);
                        glVertex2f(-sub_boxes[i].w,  sub_boxes[i].h);
                        glVertex2f( sub_boxes[i].w,  sub_boxes[i].h);
                        glVertex2f( sub_boxes[i].w, -sub_boxes[i].h);
                glEnd();
                glPopMatrix();
                rot_angle[i] -= rot_speed[i];
            }
        } else {
            // rot_angle = 0;
            initRotationVel();
            explode_done = true;
            // reset_sub_boxes();
        }
    }
}

// void Blocky::reset_sub_boxes()
// {

// }

void Blocky::reset()
{
    if (hpCheck()) {
        lives--;
        explode();
        // cerr << "explode called\n";
        explode_done = false;
        if (lives > 0) {
            hp = starting_hp;   // give back full health
        }

    }
    was_hit = false;

    setVel(0.0f, -4.0f, 0.0f);
    setRandPosition();    // put at a new random position
    // was_hit = false;
    // cerr << "was_hit set to " << boolalpha << was_hit << endl;
}

void Blocky::gamereset()
{
    lives = 2;
    hp = starting_hp;
    setVel(0.0f, -4.0f, 0.0f);
    setRandPosition();    // put at a new random position
    was_hit = false;
}

bool Blocky::didDamage()
{
    return was_hit;
}

void Blocky::move()
{
        // move main blocky
    if (isAlive() && explode_done) {
        pos[0] += vel[0];
        pos[1] += vel[1];
        pos[2] += vel[2];
        vel[0] += acc[0];
        vel[1] += acc[1];
        vel[2] += acc[2];
    } else if (!explode_done) { // move sub boxes until they fall off screen
        if (subScreenIn()) {
            for (int i = 0; i < SUB_BLOCK_N; i++) {
                sub_boxes[i].pos[0] += sub_boxes[i].vel[0];
                sub_boxes[i].pos[1] += sub_boxes[i].vel[1];
                sub_boxes[i].pos[2] += sub_boxes[i].vel[2];
                sub_boxes[i].vel[0] += sub_boxes[i].acc[0];
                sub_boxes[i].vel[1] += sub_boxes[i].acc[1];
                sub_boxes[i].vel[2] += sub_boxes[i].acc[2];
            }
        }
    }
}

bool Blocky::subBoxCollision(Item & itm)
{
    for (int i = 0; i < SUB_BLOCK_N; i++) {
        if (sub_boxes[i].collision(itm)) {
            return true;
        }
        // if (itm.collision(sub_boxes[i])) {
        //     return true;
        // }
    }
    return false;
}

bool Blocky::subBoxCollision(Entity & ent)
{
    for (int i = 0; i < SUB_BLOCK_N; i++) {
        if (ent.collision(sub_boxes[i])) {
            return true;
        }
    }
    return false;
}

void Item::hpDamage(Blocky & bf)
{
    // cerr << "blocky's hpDamage called" << endl;
    if (!bf.was_hit) {
        if (item_type == 0) {   // toaster collision
            hp = hp - bf.damage;
            bf.was_hit = true;
        } else {
            hp = hp - bf.damage;
        }
        // cerr << "blocky hit something" << endl;
    }
}

bool Blocky::isAlive()
{
    return (lives > 0);
}

void Blocky::setHit()
{
    was_hit = true;
    // cerr << "setting hit in blocky" << endl;
}

void Blocky::explode()
{
    int rvel = 8;
    float deg_to_rad = PI/180.0f;
    int pixel_offset = 8;   // sets origin of offset to be 8 left and 8 down
    int xcoord = pos[0] - pixel_offset;
    int ycoord = pos[1] - pixel_offset;
    int rand_offset; // pixel_offset pixel offset randomly from center of blocky

    for (int i = 0; i < SUB_BLOCK_N; i++) {
        rand_offset = rand() % (pixel_offset * 2);
        sub_boxes[i].setPos(pos[0]+rand_offset, pos[1]+rand_offset, 0);
        sub_boxes[i].setVel((rvel*cos(deg_to_rad * sb_angles[i])),
                                    (rvel*sin(deg_to_rad * sb_angles[i])), 0);
    }
}

#ifdef USE_OPENAL_SOUND

void checkSound(void)
{
	static bool initial_play = false;
	static bool loop_set = false;
	static bool initial_game_setup = false;
	static int prev_btype = 1;
    static int exploded = 0;
    static int bhit_occured = 0;
    static bool bomb_playing = false;


    // Main menu / music SFX loop check
	if (g.state == SPLASH || g.state == MAINMENU || g.state == GAMEOVER) {
		// init_game_setup will unque intro buffers and queue game songs
		initial_game_setup = false;	// switch to false if it was prev true

		if (initial_play == false) {
			// cerr << "calling playStartTrack()" << endl;
			sounds.playStartTrack();	// queues intro songs and plays
			initial_play = true;
		}

		if (sounds.checkIntroBufferDone() && !loop_set) {
			// sounds.resetBufferDone();
			// cerr << "sounds.checkintobuffer == true" << endl;
			// cerr << "calling loopIntro()" << endl;
			sounds.loopIntro();
			loop_set = true;
		}
	} else if (g.state == GAME && initial_game_setup == false) {
			// reset initial play so that intro plays
		initial_play = loop_set = false;
		initial_game_setup = true;
		// cerr << "calling setupGameMode()" << endl;
		sounds.setupGameMode();

	}

	// *******     SFX NOISES      **********//

    if (g.state == GAME) {
        // start playing new sound if leveled up gun
        if ((tos.bullet_type_prime != prev_btype) && (sounds.gun_shooting)) {
            sounds.gunStop();
            sounds.gunPlay(tos.bullet_type_prime);
            prev_btype = tos.bullet_type_prime;
        }
        // if space bar is pressed down and gun not already shooting
        if ((g.keys[XK_space] == 1) && (!sounds.gun_shooting)) {
            cerr << "tos.bullet_type_prime: " << tos.bullet_type_prime << endl;
            sounds.gunPlay(tos.bullet_type_prime);
            sounds.gun_shooting = true;
            // if spacebar not pressed down and gun noise currently set to shoot
        } else if (g.keys[XK_space] == 0 && (sounds.gun_shooting)) {
            sounds.gunStop();
            sounds.gun_shooting = false;
        }

        if (blocky->explode_done == false && exploded == 0) {
            sounds.exploSFX();
            exploded = true;
        } else if (blocky->explode_done == true && exploded == 1) {
            exploded = false;
        }

        if (blocky->was_hit == true && bhit_occured == 0) {
            cerr << "should be playing doosh";
            sounds.doosh(1);
            bhit_occured = true;
            blocky->was_hit = false;
        } else if (blocky->was_hit == false && bhit_occured == 1) {
            bhit_occured = false;
        }

        // bomb explosion noise
        if (bomb.is_thrown && !bomb_playing) {
            sounds.bombExplosion();
            bomb_playing = true;
        } else if (!bomb.is_thrown && 
                    !bomb.is_exploding && 
                        bomb_playing) {
            bomb_playing = false;
        }

    } else {
        if (sounds.gun_shooting == true) {
            sounds.gunStop();
            sounds.gun_shooting = false;
        }
    }
}
#endif

// directs enemies to be present during specified states
// also changes enemy settings if it's relevent
void checkLevel()
{
    static bool lvl_change = false;

    if (g.substate != DEBUG) {
        int level_duration = 10; // 20 second levels at the moment
        int level_time = g.gameTimer.getTime('n');
        
        static int lvl_change_time;

        // wait until the next clock tick
        if (lvl_change && lvl_change_time != level_time) {
            lvl_change = false;
            cerr << "lvl_change toggled to false\n";
        }

        if (g.state == GAME && 
            lvl_change == false && 
            level_time != 0 &&
            ((level_time % (level_duration)) == 0)) {

            lvl_change = true;
            cerr << "lvl change being toggled to true\n";
            lvl_change_time = level_time; 
            // level up handler
            switch (g.level) {
                case LEVEL1:
                    // Level2: Bread(2)
                    g.level = LEVEL2;
                    // change bread vars
                    break;
                case LEVEL2:
                    // Level3: Entities(1) + Bread(1)
                    g.level = LEVEL3;
                    g.entity_active = true;
                    break;
                case LEVEL3:
                    // Level4: Entities(2) + Bread(2)
                    g.level = LEVEL4;
                    g.entity_active = true;
                    // change entity vars
                    break;
                case LEVEL4:
                    // Level5: Blocky(1) + Bread(2) + Entities(2)
                    g.level = LEVEL5;
                    g.entity_active = true;
                    g.mike_active = true;
                    break;
                case LEVEL5:
                    // Level6: Blocky(2) + Bread(2) + Entities(2)
                    g.level = LEVEL6;
                    blocky->gamereset();
                    g.entity_active = true;
                    g.mike_active = true;
                    // change blocky vars
                    break;
                case LEVEL6:
                    // Level7: HBlocky(1) + Bread(2) + Entities(2)
                    g.level = LEVEL7;
                    g.entity_active = true;
                    blocky = &hblocky;
                    blocky_health = &hblocky_health;
                    // change blocky to horizontal
                    blocky->gamereset();
                    g.mike_active = true;
                    break;
                case LEVEL7:
                    // Level8: HBlocky(2) + Bread(2) + Entities(2)
                    g.level = LEVEL8;
                    g.entity_active = true;
                    // change HBlocky vars
                    blocky->gamereset();
                    g.mike_active = true;
                    break;
                case LEVEL8:
                    // Level9: Boss
                    g.level = LEVEL9;
                    g.entity_active = true;
                    // unleash bossman randy savage
                    g.huaiyu_active = true;

                    break;
                case LEVEL9:
                    // should transition to game over
                    // g.level = LEVEL1;
                    break;
                default:    // Level 1 behavior (Bread(1))   // shouldn't need
                    g.level = LEVEL1;

                    break;
            }
        }        
    }

    if (g.state == GAMEOVER) {
            g.level = LEVEL1;
            g.huaiyu_active = false;
            g.entity_active = false;
            g.dtorres_active = false;
            g.mike_active = false;
            blocky = &vblocky;
            blocky_health = &vblocky_health;
    }

}

HighScore::HighScore(string n, int s)
    : uname(n), score(s) { }

// overloaded < operator to compare scores in algorithm lib
bool HighScore::operator < (const HighScore & rhs)
{
    return (score < rhs.score);
}

// overloaded = operator used to see if it's exactly equal to the last
// score in the highscores list.. if it is then it keeps the new val
bool HighScore::operator == (const HighScore & rhs)
{
    return (score == rhs.score);
}

bool HighScore::operator == (int val)
{
    return score == val;
}

bool HighScore::operator == (string str)
{
    return uname == str;
}

Gamerecord::Gamerecord()
{
	bool read_success = getRecord();
	
	// make new blank record with fake names
	if (!read_success) {
        // generate and load fake scores into file so that there's always 10
		genFakeNames();			
        // write file to disk so that it now exists for the future
        writeRecord();	
	}

	memset(gamer,' ',9); gamer[9] = '\0';
	highscore = scores[scores.size()-1].score;
	user_score = nullptr;
    hs_menu = nullptr;
    place = -1;
    makeMenu();
}

Gamerecord::~Gamerecord()
{
    cerr << "constructor called..." << endl;
	writeRecord();
	if (user_score) {
		delete user_score;
		user_score = nullptr;
	}

    if (hs_menu) {
        delete hs_menu;
        hs_menu = nullptr;
    }

    cerr << "gamerecord constructor finishing..." << endl;
}

// reads highscores from local file and loads them into the scores vector
bool Gamerecord::getRecord()
{
	// ****--------------------------[[ TODO: ]]---------------------------****
	// replace this code with the query to odin to retrieve high scores
	ifstream fin("Highscore.txt");

	if (!fin) {
		return false;
	}

	string user;
	int score;
	int count = 0;

	while (fin >> user >> score) {
        // cerr << user << " : " << score << endl;
		HighScore entry = HighScore(user, score);
		scores.push_back(entry);
		count++;
	}

	if (count < 10) {
		genFakeNames();
	}

	return true;
}

// writes top ten records to disk
void Gamerecord::writeRecord()
{
	ofstream fout("Highscore.txt");

	if (!fout) {
		throw "could not write to highscore file";
	}

	// only write top 10 scores
	for (int i = 0; i < scores.size(); i++) {
		fout << scores[i].uname << "\t" << scores[i].score;
        if (i != (scores.size() - 1)) {
            fout << endl;
        }
	}

	cerr << "Highscore.txt written successfully...\n";
}


void Gamerecord::submitRecord(int s)
{
	if (user_score == nullptr) {
		user_score = new HighScore(string(gamer), s);
	} else {
        delete user_score;
		user_score = new HighScore(string(gamer), s);
	}

    if (user_score) {
        cerr << user_score->uname << "'s score is " 
                << user_score->score << endl;

        cerr << "adding to records..." << endl;
        addRecord(*user_score);
        for (int i = 0; i < scores.size(); i++) {
            if ((scores[i].score == user_score->score) && 
                    (scores[i].uname == user_score->uname))
                place = i;
        }
    }

    if (isHighScore()) {
        highscore = s;
    }

    cerr << "making new menu... " << endl;
    makeMenu(); // make the menu with 11 people

	scores.pop_back();	// delete the lowest
}

// sorts the records
void Gamerecord::sortRecord()
{
	sort(scores.begin(), scores.end());	// sorts in ascending
	reverse(scores.begin(), scores.end());	// changes to descending

    cerr << "finished sorting scores\n";
}

// adds a record then sorts the records
void Gamerecord::addRecord(HighScore s)
{
	scores.push_back(s);
    cerr << "finished adding score\n";
	sortRecord();
}

// tests to see if the user's score is equal to the high score
bool Gamerecord::isHighScore()
{
	// return (place == (*user_score).score);
    if (place == -1)
        return false;
    
    return (place == 0);     
}

bool Gamerecord::isTopTen()
{
    if (place == -1)
        return false;

	return (place >= 0 && place < 11);
}

void Gamerecord::genFakeNames()
{
	string names[10] = {"Amy", "Mike", "Grayson", "Gavin", "Dan", 
						"Huaiyu", "Ailand", "Newb1234", "tehBest", "Gordon"};
	int nums[10] = {100, 200, 300, 400, 500, 600, 700, 800, 900, 1000};
	int i = 0;

	while (scores.size() < 10) {
		// make sure the dummy value is unique,.
		auto it = find(scores.begin(), scores.end(), nums[i]);
		if (it == scores.end())
			scores.push_back(HighScore(names[i],nums[i]));
		i++;
	}
	
	sortRecord();
}

void Gamerecord::makeMenu()
{
    ostringstream temp;
    string name_list[scores.size()];

    for (int i = 0; i < scores.size(); i++) {
        temp << left << setw(12) << scores[i].uname 
             << right << setw(60) << scores[i].score;
        cerr << left << setw(12) << scores[i].uname 
             << right << setw(60) << scores[i].score << endl;

        name_list[i] = temp.str();
        temp.str("");
    }
    
    // allocate mem for new menu
    if (!hs_menu) {
        // cerr << "menu set to null so far" << endl;
        hs_menu = new Menu(scores.size(), 300.0, 300.0, 
                            g.xres/2.0, g.yres/2.0, name_list, 1);
    } else if (hs_menu) {   // make a new menu
        // cerr << "deleting the prev menu" << endl;
        delete hs_menu;
        // cerr << "now making a new menu" << endl;
        hs_menu = new Menu(scores.size(), 300.0, 300.0, 
                            g.xres/2.0, g.yres/2.0, name_list, 1);
    }

    // cerr << "checking if high score" << endl;
    if (isHighScore()) {
        // cerr << "setting high score color" << endl;
        (hs_menu->t_boxs[0]).setColor(124,10,2);
    } else if (isTopTen()) {
        // cerr << "setting top ten color" << endl;
        (hs_menu->t_boxs[place]).setColor(178,222,39);
    }

    // set 11th element to yellow (will be deleted)
    // cerr << "setting 11th element color" << endl;
    if (scores.size() == 11)
        (hs_menu->t_boxs[scores.size()-1]).setColor(189,195,199);

    // cerr << "finished making menu...\n" << endl;
}

SoundBar::SoundBar(float * _val, float _x, float _y, std::string _bn_)
    : value(_val), pos{_x,_y,0}, bar_name(_bn_)
{
    const float total_box_length = 200.0f;
    const float button_box_width = 20.0f; 
    const float sound_bar_thickness = 60.0f;
    const float line_thickness = 5.0f;
    const float line_length = total_box_length 
                                            -((2*button_box_width) + (10*2));
    const float half_length = line_length / 2.0f;


    bckgr.setDim(total_box_length, sound_bar_thickness);
    bckgr.setPos(pos[0], pos[1], pos[2]);
    bckgr.setColor(47, 61, 64);

    const float left_pos = bckgr.pos[0]-line_length+button_box_width;
    const float right_pos = bckgr.pos[0]+line_length-button_box_width;
    slider_left_stop_pos = left_pos + 2*button_box_width;
    slider_right_stop_pos = right_pos - 2*button_box_width;


    boarder.setDim(total_box_length+BSIZE, sound_bar_thickness + BSIZE);
    boarder.setPos(pos[0], pos[1], pos[2]);
    boarder.setColor(69, 85, 89);

    leftb.setDim(button_box_width, button_box_width);
    leftb.setPos(left_pos, bckgr.pos[1], pos[2]);
    leftb.setColor(61, 90, 115);
    leftb.id=0;

    rightb.setDim(button_box_width, button_box_width);
    rightb.setPos(right_pos, bckgr.pos[1], pos[2]);
    rightb.setColor(61, 90, 115);
    rightb.id=1;

    line.setDim(line_length, line_thickness);
    line.setPos((bckgr.pos[0]), bckgr.pos[1], pos[2]);
    line.setColor(27, 27, 27);

    slider.setDim(button_box_width, 30);
            //    (beginnning of line ) + (proportion of volume to full vol)
    slider_position = getSliderPosition();
    slider.setPos(slider_position,pos[1],0); 
    slider.setColor(69,85,89);

    words[0] = "<";
    words[1] = ">";
    
    // button text
    texts[0].bot = leftb.pos[1]-4;
    texts[0].left = leftb.pos[0];
    texts[0].center = 1;

    texts[1].bot = rightb.pos[1]-4;
    texts[1].left = rightb.pos[0];
    texts[1].center = 1;

    // bar title
    texts[2].bot = bckgr.pos[1] + 5;
    texts[2].left = bckgr.pos[0];
    texts[2].center = 1;
}

void SoundBar::draw()
{
    // draw boarder

    glColor3ubv(boarder.color);

    glPushMatrix();
    glTranslatef(boarder.pos[0], boarder.pos[1], boarder.pos[2]);
    glBegin(GL_QUADS);
        glVertex2f(-boarder.w, -boarder.h);
        glVertex2f(-boarder.w,  boarder.h);
        glVertex2f( boarder.w,  boarder.h);
        glVertex2f( boarder.w, -boarder.h);
    glEnd();
    glPopMatrix();

    // draw mainbox

    glColor3ubv(bckgr.color);

    glPushMatrix();
    glTranslatef(bckgr.pos[0], bckgr.pos[1], bckgr.pos[2]);
    glBegin(GL_QUADS);
        glVertex2f(-bckgr.w, -bckgr.h);
        glVertex2f(-bckgr.w,  bckgr.h);
        glVertex2f( bckgr.w,  bckgr.h);
        glVertex2f( bckgr.w, -bckgr.h);
    glEnd();
    glPopMatrix();

    // draw line

    glColor3ubv(line.color);

    glPushMatrix();
    glTranslatef(line.pos[0], line.pos[1], 0.0f);
    glBegin(GL_QUADS);
        glVertex2f(-line.w, -line.h);
        glVertex2f(-line.w,  line.h);
        glVertex2f( line.w,  line.h);
        glVertex2f( line.w, -line.h);
    glEnd();
    glPopMatrix();

    // draw leftb

    glColor3ubv(leftb.color);

    glPushMatrix();
    glTranslatef(leftb.pos[0], leftb.pos[1], 0.0f);
    glBegin(GL_QUADS);
        glVertex2f(-leftb.w, -leftb.h);
        glVertex2f(-leftb.w,  leftb.h);
        glVertex2f( leftb.w,  leftb.h);
        glVertex2f( leftb.w, -leftb.h);
    glEnd();
    glPopMatrix();

    // draw rightb

    glColor3ubv(rightb.color);

    glPushMatrix();
    glTranslatef(rightb.pos[0], rightb.pos[1], 0.0f);
    glBegin(GL_QUADS);
        glVertex2f(-rightb.w, -rightb.h);
        glVertex2f(-rightb.w,  rightb.h);
        glVertex2f( rightb.w,  rightb.h);
        glVertex2f( rightb.w, -rightb.h);
    glEnd();
    glPopMatrix();

    // draw slider

    glColor3ubv(slider.color);

    glPushMatrix();
    glTranslatef(slider.pos[0], slider.pos[1], 0.0f);
    glBegin(GL_QUADS);
        glVertex2f(-slider.w, -slider.h);
        glVertex2f(-slider.w,  slider.h);
        glVertex2f( slider.w,  slider.h);
        glVertex2f( slider.w, -slider.h);
    glEnd();
    glPopMatrix();

    ggprint8b(&texts[0], 0, 0x00ffffff, words[0].c_str());
    ggprint8b(&texts[1], 0, 0x00ffffff, words[1].c_str());
    ggprint12(&texts[2], 0, 0x00ffffff, bar_name.c_str());
}

float SoundBar::getSliderPosition()
{
    return (slider_left_stop_pos + ((*value)*(slider_right_stop_pos-
                                                        slider_left_stop_pos)));
}

// pass in mouse coords to check and see if they are within the bounds
// of the menu's text boxes
Box* SoundBar::checkButtons(float x, float y)
{
    Box * box_ptr = nullptr;

    size_t i;

        if ((x > (leftb.pos[0]-leftb.w)) &&
            (x < (leftb.pos[0]+leftb.w)) &&
            (y > (leftb.pos[1]-leftb.h)) &&
            (y < (leftb.pos[1]+leftb.h))) {
            
            box_ptr = &leftb;
        }
        else if ((x > (rightb.pos[0]-rightb.w)) &&
            (x < (rightb.pos[0]+rightb.w)) &&
            (y > (rightb.pos[1]-rightb.h)) &&
            (y < (rightb.pos[1]+rightb.h))) {
            
            box_ptr = &rightb;
        }

    return box_ptr;
}

void SoundBar::setOrigColor()
{
    leftb.setColor(61, 90, 115);
    rightb.setColor(61, 90, 115);
}

void SoundBar::setHighlight(Box * b)
{
    b->setColor(33,136,171);
}

void SoundBar::moveSliderDown()
{
    // if (slider_position > slider_left_stop_pos) {
    if (*value >= 0.18) {
        *value -= 0.1f;
        slider_position = getSliderPosition();
        slider.setPos(slider_position, slider.pos[1], slider.pos[2]);
    } else {
        *value = 0;
        slider_position = getSliderPosition();
        slider.setPos(slider_position, slider.pos[1], slider.pos[2]);
    }
}

void SoundBar::moveSliderUp()
{

    // if (slider_position < slider_right_stop_pos) {
    if (*value < 0.82f) {
        *value += 0.1f;
        slider_position = getSliderPosition();
        slider.setPos(slider_position, slider.pos[1], slider.pos[2]);
    } else {
        *value = 1;
        slider_position = getSliderPosition();
        slider.setPos(slider_position, slider.pos[1], slider.pos[2]);
    }
}


Bomb::Bomb()
{
    curr_radius = start_radius = 6;
    stop_radius = 450;
    pos[0] = g.xres/2.0;
    pos[1] = g.yres/2.0;
    pos[2] = 0;
    setPos(g.xres/2.0f, g.yres/2.0f, 0);
    setColor(color, 255, 91, 20);
    setColor(launch_color, 255, 255, 255);
    // is_gone = false;
    is_thrown = false;
    is_exploding = false;
    num_bombs = 99;
    w = 6;
    h = 6;
    tex = &g.bomb_texture;
}

Bomb::~Bomb()
{
    // destructor
    if (bomb_timer) {
        delete bomb_timer;
        bomb_timer = nullptr;
    }
}

void Bomb::draw()
{
    if (is_thrown && !is_exploding) {

        int size = 50;

        glPushMatrix();
        glBindTexture(GL_TEXTURE_2D, *(bomb.tex));
        // glColor3ub(color[0], color[1], color[2]);
        glEnable(GL_ALPHA_TEST);
        glAlphaFunc(GL_GREATER, 0.0f);
        glColor4f(1.0f, 1.0f, 1.0f, 1.0f);
        glTranslatef(pos[0], pos[1], pos[2]);
        glBegin(GL_QUADS);
                glTexCoord2f(0.0f, 0.0f);
                glVertex2f(0,0);
                glTexCoord2f(0.0f, 1.0f);
                glVertex2f(0, size);
                glTexCoord2f(1.0f, 1.0f);
                glVertex2f(size, size);
                glTexCoord2f(1.0f, 0.0f);
                glVertex2f(size, 0);
        glEnd();
        glBindTexture(GL_TEXTURE_2D, 0);
        glDisable(GL_ALPHA_TEST);
        glPopMatrix();

        /*
            glColor3ubv(launch_color);


            glPushMatrix();
            glTranslatef(pos[0], pos[1], pos[2]);
            glBegin(GL_QUADS);
                glVertex2f(-6, -6);
                glVertex2f(-6,  6);
                glVertex2f( 6,  6);
                glVertex2f( 6, -6);
            glEnd();
            glPopMatrix();
        */
    } else if (is_exploding) {
        float angle1 = (2 * PI * 1)/100;
        float angle2 = (2 * PI * 2)/100;
        float vert1[2] = {cos(angle1) * curr_radius, sin(angle1) * curr_radius};
        float vert2[2] = {cos(angle2) * curr_radius, sin(angle2) * curr_radius};
        float center[3] = {pos[0], pos[1], pos[2]};

        // cerr << "center: " << pos[0] << "," << pos[1] << "," << pos[2] << endl; 

        glPushMatrix();
        glTranslatef(center[0], center[1], center[2]);
        glBegin(GL_TRIANGLE_FAN);
        glColor3ub(204, 204, 0);
        glVertex2f(0,0);
        for (int i = 0; i < 103; i++) {
            glColor3ubv(color);
            glVertex2f(vert1[0], vert1[1]);
            // cerr << "vert1: " << vert1[0] << "," << vert1[1] << endl; 

            glVertex2f(vert2[0], vert2[1]);
            // cerr << "vert2: " << vert2[0] << "," << vert2[1] << endl; 

            glColor3ub(0, 0, 0);
            glVertex2f(0, 0);

            // move vert2 to vert1 vertices
            vert1[0] = vert2[0];
            vert1[1] = vert2[1];

            // calc new vert for vert2
            angle2 = (2 * PI * i)/100;
            vert2[0] = cos(angle2) * curr_radius;
            vert2[1] = sin(angle2) * curr_radius;
        }
        glEnd();
        glPopMatrix();
    }

}

void Bomb::setColor(unsigned char * col, int r, int g, int b)
{
    col[0] = (char)r;
    col[1] = (char)g;
    col[2] = (char)b;
}

void Bomb::setPos(float x, float y, float z)
{
    pos[0] = x;
    pos[1] = y;
    pos[2] = z;
}

void Bomb::explode()
{
    if (bomb_timer) {
        delete bomb_timer;
        bomb_timer = nullptr;
    }

    if (curr_radius >= stop_radius) {
        // is_gone = true;
        is_exploding = false;
        // cerr << "is_exploding to false" << endl;
        is_thrown = false;
        curr_radius = start_radius;

    } else {
        // cerr << "is_exploding to true" << endl;
        is_exploding = true;
        // is_gone = false;
        curr_radius += 12;
        updateHitbox();
    }
}

void Bomb::move()
{
    if (bomb_timer && !(bomb_timer->isDone())) {
        pos[0] += 7;
    } else if (bomb_timer && bomb_timer->isDone()) {
        explode();
    } else if (is_exploding) {
        explode();
    }
}

void Bomb::launch()
{
    // if (!is_thrown && num_bombs > 0) {
    if (!is_thrown && (tos.energy > 20)) {
        is_thrown = true;
        tos.energy -= 20;
        setPos(tos.pos[0],tos.pos[1],tos.pos[2]);
        bomb_timer = new Timer(0.7);
#ifdef USE_OPENAL_SOUND
        sounds.bombExplosion();
#endif
        // num_bombs--;
    } else {
        // cerr << "no bombs or one already active" << endl;
    }
}

void Bomb::updateHitbox()
{
    w = h = 2*curr_radius;
}

// hitbox collision with item class
// check this first before checking resultant to bomb center from vertices
bool Bomb::hitboxCollision(Item & itm)
{
    bool x = (((pos[0]+w)-(itm.pos[0]-itm.w))*((pos[0]-w)-(itm.pos[0]+itm.w))) < 0;
  	bool y = (((pos[1]+h)-(itm.pos[1]-itm.h))*((pos[1]-h)-(itm.pos[1]+itm.h))) < 0;
  	
    return x&&y;
}

// hitbox collision with ent class
// check this first before checking resultant to bomb center from vertices
bool Bomb::hitboxCollision(Entity & ent)
{
    bool x = (((pos[0]+w)-(ent.pos[0]-ent.dim[0]))*((pos[0]-w)-(ent.pos[0]+ent.dim[0]))) < 0;
  	bool y = (((pos[1]+h)-(ent.pos[1]-ent.dim[1]))*((pos[1]-h)-(ent.pos[1]+ent.dim[0]))) < 0;
  	
    return x&&y;
}

// tests collision with hitbox first then itm's corners 
bool Bomb::collision(Item & itm)
{
    // cerr << "checking bomb collision with " << &itm << endl;
    if (!hitboxCollision(itm)) {
        // cerr << "not a hitbox collision on " << &itm << endl;
        return false;
    }

    // cerr << "hitbox collision on " << endl;
/*
    This would be for center of the box:

    double xvec = itm.pos[0]-pos[0];
    double yvec = itm.pos[1]-pos[1];

    We're going to calculate it for the 4 corners though so we need to
    do +/- the width and height

        0               3
        *---------------*
        |               |
        |               |
        |               |
        *---------------*
        1               2

*/

    // double xvec[4] = { itm.pos[0] - (itm.w/2.0f) - pos[0],
    //                     itm.pos[0] - (itm.w/2.0f) - pos[0],
    //                     (itm.pos[0] + (itm.w/2.0f)) - pos[0],
    //                     (itm.pos[0] + (itm.w/2.0f)) - pos[0] }

    double xvec[4] = { itm.pos[0] - (itm.w/2.0f) - pos[0],
                        xvec[0],
                        xvec[0]+itm.w,
                        xvec[2]};

    // double yvec[4] = { itm.pos[1] + (itm.h/2.0f) - pos[1],
    //                     itm.pos[1] - (itm.h/2.0f) - pos[1],
    //                     (itm.pos[1] - (itm.h/2.0f)) - pos[1],
    //                     (itm.pos[1] + (itm.h/2.0f)) - pos[1] };

    double yvec[4] = { itm.pos[1] + (itm.h/2.0f) - pos[1],
                    yvec[0] - itm.h,
                    yvec[1],
                    yvec[0]};

    double resultant;

    for (int i = 0; i < 4; i++) {
        resultant = sqrt(pow(xvec[i],2) + pow(yvec[i],2));
        if (resultant <= curr_radius)
            return true;
    }

    return false;
}

// tests collision with hitbox first then ent's corners 
bool Bomb::collision(Entity & ent)
{
    if (!hitboxCollision(ent)) {
        return false;
    }

/*
    This would be for center of the box:

    double xvec = itm.pos[0]-pos[0];
    double yvec = itm.pos[1]-pos[1];

    We're going to calculate it for the 4 corners though so we need to
    do +/- the width and height

        0               3
        *---------------*
        |               |
        |               |
        |               |
        *---------------*
        1               2

*/

    // double xvec[4] = { itm.pos[0] - (itm.w/2.0f) - pos[0],
    //                     itm.pos[0] - (itm.w/2.0f) - pos[0],
    //                     (itm.pos[0] + (itm.w/2.0f)) - pos[0],
    //                     (itm.pos[0] + (itm.w/2.0f)) - pos[0] }

    double xvec[4] = { ent.pos[0] - (ent.dim[0]/2.0f) - pos[0],
                        xvec[0],
                        xvec[0]+ent.dim[0],
                        xvec[2]};

    // double yvec[4] = { itm.pos[1] + (itm.h/2.0f) - pos[1],
    //                     itm.pos[1] - (itm.h/2.0f) - pos[1],
    //                     (itm.pos[1] - (itm.h/2.0f)) - pos[1],
    //                     (itm.pos[1] + (itm.h/2.0f)) - pos[1] };

    double yvec[4] = { ent.pos[1] + (ent.dim[1]/2.0f) - pos[1],
                    yvec[0] - ent.dim[1],
                    yvec[1],
                    yvec[0]};

    double resultant;

    for (int i = 0; i < 4; i++) {
        resultant = sqrt(pow(xvec[i],2) + pow(yvec[i],2));
        if (resultant <= curr_radius)
            return true;
    }

    return false;
}
