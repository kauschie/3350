
all: lab2 

lab2: xylab2.cpp
	g++ xylab2.cpp libggfonts.a -Wall -olab2 -lX11 -lGL -lGLU -lm 

mover: xylab2_move.cpp
	g++ xylab2_move.cpp libggfonts.a -Wall -omover -lX11 -lGL -lGLU -lm 

clean:
	rm -f lab2 

