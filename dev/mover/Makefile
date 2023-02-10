
all: mover color

mover: xylab2_move.cpp
	g++ xylab2_move.cpp libggfonts.a -Wall -omover -lX11 -lGL -lGLU -lm 

color: xylab2_move_color.cpp
	g++ xylab2_move_color.cpp libggfonts.a -Wall -ocolor -lX11 -lGL -lGLU -lm 

clean:
	rm -f mover color

