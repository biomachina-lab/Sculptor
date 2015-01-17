/***************************************************************************
                          svt_serial.h  -  description
                             -------------------
    begin                : Tue July 18 2000
    authors              : Herwig Zilken, Matthias Belitz
    email                : sculptor@biomachina.org
 ***************************************************************************/

#include <svt_serial.h>
#include <iostream.h>

extern "C"{
#include <unistd.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <sys/uio.h>
#include <malloc.h>
#include <termios.h>
#include <sys/ioctl.h>
#include <errno.h>
#include <string.h>
}

svt_serial::svt_serial() :
  _fd(-1),
  _tty_name(0),
  _baud_rate(19200),
  _vmin(0),
  _vtime(0),
  _xon(0),
  _xoff(0)
{
}

svt_serial::svt_serial(const char *ttyname) :
  _fd(-1),
  _tty_name(0),
  _baud_rate(19200),
  _vmin(0),
  _vtime(0),
  _xon(0),
  _xoff(0)
{
  if (ttyname) {
    _tty_name = ::strdup(ttyname);
    open(_tty_name);
  }
}

svt_serial::~svt_serial()
{
  close();
  
  if (_tty_name) 
    free(_tty_name);
}

int
svt_serial::int_to_baud(int int_baud)
{
  int baud=int_baud;
  
  switch (int_baud) {
  case 1200:
  case 2400:
  case 4800:
  case 9600:
  case 19200:
  case 38400:
  case 57600:
    break;
  default:
    baud = 9600;
  }
  
  return baud;
}

int
svt_serial::configure_line()
{
  if (!is_open()) {
    //cerr<<"svt_serial:configure_line: line not open"<<endl;
    return 0;
  }
  struct termio _tio;
  memset(&_tio, 0, sizeof(_tio));

#ifndef linux
  int baudrate = int_to_baud(_baud_rate);
#endif
  
  _tio.c_iflag = (_xon ? IXON : 0) | (_xoff ? IXOFF : 0);
  _tio.c_oflag = 0;
  _tio.c_lflag = 0;
  _tio.c_cflag = CS8 | CREAD | CLOCAL;

#ifndef linux
  _tio.c_ospeed=baudrate;
  _tio.c_ispeed=baudrate;
#endif
  
  _tio.c_cc[VMIN] = _vmin;	// return with _vmin characters
  _tio.c_cc[VTIME] = _vtime;	// timeout after _vtime deciseconds
  
  int ret = ::ioctl(_fd, TCSETA, &_tio);
  if (ret == -1) {
    cerr<<"svt_serial: ioctl TCSETA: "<< strerror(errno)<<endl;
    return 0;
  }    
  
  return 1;
}

int
svt_serial::open(const char *ttyname)
{
  close();

  _fd = ::open(ttyname, O_RDWR);
  if (_fd < 0) {
    cerr<<"svt_serial: open: "<< strerror(errno)<<endl;
    return 0;
  }

  return configure_line();
}

int
svt_serial::is_open()
{
  return (_fd > 0);
}

int
svt_serial::close()
{
  if (is_open()) {
    ::close(_fd);
    _fd = 0;
    return 1;
  } else {
    return 0;
  }
}

int
svt_serial::reopen()
{
  if (close())
    return open(_tty_name);
  else
    return 0;
}

int
svt_serial::write_char(char c)
{
  if (::write(_fd, &c, 1) != 1) {
    cerr<<"svt_serial: write: "<< strerror(errno)<<endl;
    return 0;
  }
  return 1;
}


float svt_serial::readFloat()
{
  float f;

  read_char((char*)&f + 3);
  read_char((char*)&f + 2);
  read_char((char*)&f + 1);
  read_char((char*)&f    );
  return f;
}


int
svt_serial::read_char(char *c)
{
  int ret = ::read(_fd, c, 1);

  if (ret == -1) {
    cerr<<"svt_serial: read: "<< strerror(errno)<<endl;
    return 0;
  } else if (ret == 0) {
    cerr<<"svt_serial: read: timeout!:"<< strerror(errno)<<endl;
    return 0;
  } 
  
  return 1;
}

int
svt_serial::read(char *buf, int bytes)
{
  int bytesread = ::read(_fd, buf, bytes);
  
  if ( bytesread != bytes) {
    cerr<<"svt_serial: read: "<< strerror(errno)<<endl;
  }
  
  return bytesread;
}

int
svt_serial::write(const char *buf, int bytes)
{
  int bytesw = ::write(_fd, buf, bytes);
  
  if ( bytesw != bytes) {
    cerr<<"svt_serial: write: "<< strerror(errno)<<endl;
  }
  
  return bytesw;
}

int
svt_serial::write_string(const char* buf)
{
  int bytes = strlen(buf);
  return write(buf, bytes);
}

void
svt_serial::flush_input()
{
  if (ioctl(_fd, TCFLSH, TCIFLUSH) == -1) {
    cerr<<"svt_serial: ioctl TCIFLSH: "<<strerror(errno)<<endl;
  }
}


void
svt_serial::flush_output()
{
  if (ioctl(_fd, TCFLSH, TCOFLUSH) == -1) {
    cerr<<"svt_serial: ioctl TCOFLSH: "<< strerror(errno)<<endl;
  }
}

void svt_serial::wait_output()
{
  if (tcdrain(_fd)==-1) {
    cerr<<"SerialLine: tcdrain: "<< strerror(errno)<<endl;    
  }
}

int
svt_serial::set_vmin(int vmin)
{
  _vmin = vmin;
  return configure_line();
}

int
svt_serial::set_vtime(int vtime)
{
  _vtime = vtime;
  return configure_line();
}

int
svt_serial::set_baud_rate(int baud_rate)
{
  _baud_rate = baud_rate;
  return configure_line();
}

int
svt_serial::set_xon(int on)
{
  _xon = on;
  return configure_line();
}

int
svt_serial::set_xoff(int on)
{
  _xoff = on;
  return configure_line();
}

