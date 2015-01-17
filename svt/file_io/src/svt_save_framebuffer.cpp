/***************************************************************************
                          svt_save_framebuffer.cpp  -  description
                             -------------------
    begin                : Mon August 28 2000
    author               : Herwig Zilken
    email                : sculptor@biomachina.org
 ***************************************************************************/

// svt_includes
#include <svt_basics.h>
#include <svt_save_framebuffer.h>
#include <svt_file_utils.h>
#include <svt_iostream.h>
#include <svt_opengl.h>

int svt_save_framebuffer::save(char *filename, int scr_width, int scr_height)
{
    int j, i, size;
    int size_pic;
    char *data;
    char *data_pic;
    FILE* outFile;
    char temp;

  size = scr_width*scr_height*4;
  size_pic = scr_width*scr_height*3;

  if( (data = new char[size]) == NULL) {
      cerr << "svt_save_framebuffer: failed to allocate memory (" << size << " bytes)" << endl;
      cerr << "svt_save_framebuffer: failed saving framebuffer to file '" << filename << "'" << endl;
      return false;
  }
  if( (data_pic = new char[size_pic]) == NULL) {
      cerr << "svt_save_framebuffer: failed to allocate memory (" << size_pic << " bytes)" << endl;
      cerr << "svt_save_framebuffer: failed saving framebuffer to file '" << filename << "'" << endl;
      return false;
  }

  //cout << "saving framebuffer, size (" << scr_width << "x" << scr_height << "), type RGB to file '" << filename << "'" << endl;

  glPixelTransferi(GL_MAP_COLOR, GL_FALSE);
  glPixelTransferi(GL_RED_SCALE, 1);
  glPixelTransferi(GL_RED_BIAS, 0);
  glPixelTransferi(GL_GREEN_SCALE, 1);
  glPixelTransferi(GL_GREEN_BIAS, 0);
  glPixelTransferi(GL_BLUE_SCALE, 1);
  glPixelTransferi(GL_BLUE_BIAS, 0);
  glPixelTransferi(GL_ALPHA_SCALE, 1);
  glPixelTransferi(GL_ALPHA_BIAS, 0);
  //if (svt_getConfig()->getValue("Stereo",(bool)false)) {
  //  glReadBuffer(GL_FRONT_LEFT);
  //} else {
    glReadBuffer(GL_FRONT);
  //}
    
  glReadPixels(0, 0, scr_width, scr_height, GL_RGBA, GL_UNSIGNED_BYTE, data);
  
  outFile = fopen(filename, "wb"); // open the file
  if (!outFile) {
        cerr << "svt_save_framebuffer: File '" << filename << "' could not be opened!\n";
        return false;
  }
  // ID Length
  fputc((char) 0, outFile);
  // Color Map Type, 0=no Color Map
  fputc((char) 0, outFile);
  // Image Type, 2=uncompressed, true color image
  fputc((char) 2, outFile);
  // Color Map Specification, here: 5 Bytes set to Zero
  fputc((char) 0, outFile);
  fputc((char) 0, outFile);
  fputc((char) 0, outFile);
  fputc((char) 0, outFile);
  fputc((char) 0, outFile);
  // Image Specification
  //   - x-origin
  svt_writeInt16(outFile, (Int16) 0);
  //   - y-origin
  svt_writeInt16(outFile, (Int16) 0);
  //   - image width
  svt_writeInt16(outFile, (Int16) scr_width);
  //   - image height
  svt_writeInt16(outFile, (Int16) scr_height);
  //   - pixel depth, here: 24 bits per pixel
  fputc((char) 24, outFile);
  //   - image descriptor
  fputc((char) 0, outFile);

  // copy RGB out of RGBA screen
  j = 0;
  for (i = 0; i < size; i+=4)
  {
      data_pic[j] = data[i];
      data_pic[j+1] = data[i+1];
      data_pic[j+2] = data[i+2];
      j+=3;
  }

  // write picture data
  for (i = 0; i < size_pic; i += 3) { // convert from RGB to BGR
    temp = data_pic[i];
    data_pic[i] = data_pic[i+2];
    data_pic[i+2] = temp;
  }
  fwrite(data_pic, 1, size_pic, outFile);
  fclose(outFile);

  //cout << "file '" << filename << "' successfully written" << endl;
  delete [] data;
  delete [] data_pic;
  return true;
}
