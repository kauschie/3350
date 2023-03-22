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

