/***************************************************************************
                          svt_pic_reader_bmp.cpp  -  description
                             -------------------
    begin                : July 11 08:45:00 CET 2000
    author               : Stefan Birmanns
    email                : sculptor@biomachina.org
 ***************************************************************************/

// svt includes
#include <svt_basics.h>
#include <svt_pic_reader_bmp.h>
#include <svt_swap.h>
#include <svt_types.h>
// clib includes
#include <string.h>
#include <math.h>

#define MAX_EXT_LEN 10

svt_pic_reader_bmp::svt_pic_reader_bmp(char* filename) : svt_pic_reader_lib(filename)
{
    FILE* inFile;
    data = NULL;

    // This next section finds the extension of the filename
    int i = strlen(filename) - 1, j = 0;
    char temp[3], extension[MAX_EXT_LEN + 1];

    while (i > 0 && filename[i] != '.') {
        i--;
    }
    if (i == 0) { // we need an extension for simple format testing
	fprintf(stderr, "svt_pic_reader_bmp: No extension specified!\n");
        return;
    }
    i += 1;

    while (j < MAX_EXT_LEN && filename[i] != '\0') {
        extension[j] = filename[i];
        j++;
        i++;
    }
    extension[j] = '\0'; // and extension now holds the extension (wow)

    inFile = fopen(filename, "rb"); // open the file
    if (!inFile) {
	fprintf(stderr, "svt_pic_reader_bmp: File %s could not be opened!\n", filename);
        return;
    }

    // Check if the file is a BMP and pass to ReadBMP if it is
    fread(&temp, sizeof(char), 2, inFile);
    temp[2] = '\0';
    if (!strcmp(temp,"BM") && !strcmp(extension, "bmp")) {
        ReadBMP(inFile);
        return;
    }

    // We don't support the file format so return false
    fprintf(stderr, "svt_pic_reader_bmp: File format unrecognised!\n");
    fclose(inFile);
    inFile = NULL;
    return;
}

svt_pic_reader_bmp::~svt_pic_reader_bmp()
{
    if (data != NULL)
        delete data;
}

// ReadBMP - reads a BMP 
bool svt_pic_reader_bmp::ReadBMP(FILE* inFile) {

    Int16 planes; // image planes
    Int16 bpp; // image bpp
    UInt32 size; // data size

    if (inFile == NULL) { // if we don't have an input file we bail
        fprintf(stderr, "svt_pic_reader_bmp: Internal error (ReadBMP): No file open!\n");
        return false;
    }

    Int32 offset = 0;
    Int32 header_size = 0;
    unsigned i = 0;

    // Read the offset of the image data area
    fseek(inFile,10,SEEK_SET);
    fread(&offset, sizeof(char), 4, inFile);
    if (svt_bigEndianMachine())
        svt_swapInt32(&offset);

    // Find the size of the header, and the x and y resolutions
    fseek(inFile,14,SEEK_SET);
    fread(&header_size, sizeof(char), 4, inFile);
    if (svt_bigEndianMachine())
        svt_swapInt32(&header_size);
    fread(&size_x, sizeof(char), 4, inFile);
    if (svt_bigEndianMachine())
        svt_swapInt32(&size_x);
    fread(&size_y, sizeof(char), 4, inFile);
    if (svt_bigEndianMachine())
        svt_swapInt32(&size_y);

    // Read the planes (this *should* always be one)
    fread(&planes, sizeof(char), 2, inFile);
    if (svt_bigEndianMachine())
        svt_swapInt16((short *)&planes);

    if (planes != 1) {
        fprintf(stderr, "svt_pic_reader_bmp: More than 1 plane is not currently supported!\n");        
		return false;
    }

    // Read BPP (8 and 24 are currently supported)
    fread(&bpp, sizeof(char), 2, inFile);
    if (svt_bigEndianMachine())
        svt_swapInt16((short*)&bpp);
    if (bpp != 24 && bpp != 8) {
        fprintf(stderr, "svt_pic_reader_bmp: The image must be 8 or 24 BPP!\n");
        return false;
    }

    // Calculate the image buffer size (24bpp)
    size = size_x * size_y * 3;

    // Allocate the image buffer
    data = new unsigned char[size];
    if (data == NULL) {
        fprintf(stderr, "Memory allocation error!\n");
        return false;
    }

    char temp;

    if (bpp == 24) { // if the image is 24bpp

        fseek(inFile,offset,SEEK_SET);
        fread((void*)data, 1, size, inFile);

        for (i = 0; i < size; i += 3) { // convert from BGR to RGB
            temp = data[i];
            data[i] = data[i+2];
            data[i+2] = temp;
        }
        fclose(inFile); // close the file
        inFile = NULL;
        return true;
    }

    // if the image is 8bpp we have the colourmap to deal with
    char *colourmap = NULL;

    fseek(inFile,header_size + 14,SEEK_SET); // move to the colour map
    unsigned int colmapsize = 4 * (int)pow((float)2, bpp); // calculate & allocate size
    colourmap = new char[colmapsize];
    if (colourmap == NULL) {
	fprintf(stderr, "Memory allocation error!\n");
        return false;
    }

    // read in colour map, then convert from BGR to RGB
    fread(colourmap, 1, colmapsize, inFile);
    for (i = 0; i < colmapsize; i += 4) {
        temp = colourmap[i];
        colourmap[i] = colourmap[i + 2];
        colourmap[i + 2] = temp;
    }

    // now move to the image area
    fseek(inFile,offset,SEEK_SET);
    char colour;
    for (i = 0; i < size; i += 3) {
        fread(&colour, 1, 1, inFile);
        data[i] = colourmap[colour * 4]; // insert RGB values of colour to image buffer
        data[i + 1] = colourmap[(colour * 4) + 1];
        data[i + 2] = colourmap[(colour * 4) + 2];
    }

    // close the file
    fclose(inFile);
    inFile = NULL;

    // delete the colourmap
    delete [] colourmap;
    return true;
}
