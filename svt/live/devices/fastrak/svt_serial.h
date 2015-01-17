/***************************************************************************
                          svt_serial.h  -  description
                             -------------------
    begin                : Tue July 18 2000
    authors              : Herwig Zilken, Matthias Belitz
    email                : sculptor@biomachina.org
 ***************************************************************************/

#ifndef SVT_SERIAL_H
#define SVT_SERIAL_H


class svt_serial {
public:

  svt_serial();
  svt_serial(const char *ttyname);
  ~svt_serial();

  int set_vmin(int vmin);
  int set_vtime(int vtime);
  int set_baud_rate(int baud_rate);
  int set_xon(int on);
  int set_xoff(int on);
  
  int open(const char *ttyname);
  int close();
  int reopen();
  int is_open();

  int read_char(char *c);
  int read(char *buf, int bytes=1);
  float readFloat();
  
  int write_char(char c);
  int write(const char* buf, int bytes);
  int write_string(const char* buf);
  int write(const char* buf);

  
  void flush_input();
  void flush_output();
  void wait_output();

protected:

  int int_to_baud(int);
  int configure_line();
  
  int _fd;
  char* _tty_name;
  int   _baud_rate;

  int _vmin;
  int _vtime;
  int _xon;
  int _xoff;

};

#endif

