//gordon griesel
//spring 2023
//
//Find width and height of your Linux terminal.
//
//Source:
//https://stackoverflow.com/questions/23369503/
//get-size-of-terminal-window-rows-columns
//
#include <stdio.h>
#include <sys/ioctl.h>
#include <unistd.h>
#include <iostream>
using namespace std;

int main(void)
{
	//declare this struct.
	struct winsize w;
	int prev_rows = -1;
	int prev_cols = -1;

	while (1) {
		//Call this function. Dimensions are in w struct. See below.
		ioctl(0, TIOCGWINSZ, &w);
		if (w.ws_row != prev_rows || w.ws_col != prev_cols) { 
			printf ("lines %d\n", w.ws_row);
		    printf ("columns %d\n", w.ws_col);

            for (int i = 0; i < w.ws_col; i++) {
                cout << "0";
            }
            cout << endl;
		}
		prev_rows = w.ws_row;
	    prev_cols = w.ws_col;
		usleep(10000);
	}
	return 0;
}
