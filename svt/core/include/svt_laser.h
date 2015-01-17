/***************************************************************************
                          svt_laser.h  -  description
                             -------------------
    begin                : Tue Feb 22 2000
    author               : Jan Deiterding
    email                : sculptor@biomachina.org
 ***************************************************************************/

#ifndef SVT_LASER_H
#define SVT_LASER_H

#define X1 0.33f*0.2
#define X2 0.75f*0.2
#define Y1 1.0f
#define Y2 0.0f

/** A laser object (for physic2k app)
  *@author Jan Deiterding
  */
class DLLEXPORT svt_laser : public svt_node  {

protected:
	float x;
	float y;
	float z;
		
public:
	svt_laser() : svt_node(){};
	svt_laser(svt_pos* _tpos) : svt_node(NULL,NULL,_tpos){};
	svt_laser(svt_pos* _tpos, svt_properties* _tprop) : svt_node(NULL,NULL,_tpos, _tprop){};
        virtual ~svt_laser() {};
	/** draw the laser */
	void drawGL();

protected:
	void printName() { cout << "Laser" << endl; };
};

#endif
