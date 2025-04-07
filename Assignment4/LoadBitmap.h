#ifndef LOADBITMAP_H
#define LOADBITMAP_H

// Loads a BMP image and returns its ARGB data.
// Parameters:
// - imagepath: the file path to the BMP image.
// - data: a pointer to a pointer where the image data will be allocated and returned.
// - width: a pointer where the image width will be stored.
// - height: a pointer where the image height will be stored.
void loadARGB_BMP(const char* imagepath, unsigned char** data, unsigned int* width, unsigned int* height);

#endif //LOADBITMAP_H