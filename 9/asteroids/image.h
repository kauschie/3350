// Modified by: Michael Kausch
// Modified On: 3/22/23
// Lab 9 continuation 
// image.h - Image class definitions


//Original author:  Gordon Griesel
//Original date:    2014 - 2021

#pragma once

#include <cstring>
#include <unistd.h>
#include <cstdlib>
#include <iostream>


//Put image file colors into a data stream.
class Image {
public:
	int width, height;
	unsigned char *data;
	~Image();
	Image(const char *fname); 
};

