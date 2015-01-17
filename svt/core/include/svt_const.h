/***************************************************************************
                          svt_const.h  -  description
                             -------------------
    begin                : Tue Feb 29 2000
    author               : Stefan Birmanns
    email                : sculptor@biomachina.org
 ***************************************************************************/

#ifndef SVT_CONST_H
#define SVT_CONST_H

#ifndef SVT_CONST_CPP

//#ifdef _WINDOWS
/** predefined color: red */
#define svt_color_red new svt_color(1.0f,0.0f,0.0f)
/** predefined color: green */
#define svt_color_green new svt_color(0.0f,1.0f,0.0f)
/** predefined color: blue */
#define svt_color_blue new svt_color(0.0f,0.0f,1.0f)
/** predefined color: white */
#define svt_color_white new svt_color(1.0f,1.0f,1.0f)
/** predefined color: yellow */
#define svt_color_yellow new svt_color(1.0f,1.0f,0.0f)
/** predefined color cyan */
#define svt_color_cyan new svt_color(0.0f,0.75f,0.75f)
/** predefined color tan */
#define svt_color_tan new svt_color(0.5f,0.5f,0.2f) 
/** predefined color silver */
#define svt_color_silver new svt_color(0.6f,0.6f,0.6f)

/** predefined property solid red */
#define svt_properties_solid_red new svt_properties(svt_color_red)
/** predefined property solid green */
#define svt_properties_solid_green new svt_properties(svt_color_green)
/** predefined property solid blue */
#define svt_properties_solid_blue new svt_properties(svt_color_blue)
/** predefined property solid white */
#define svt_properties_solid_white new svt_properties(svt_color_white)
/** predefined property solid yellow */
#define svt_properties_solid_yellow new svt_properties(svt_color_yellow)
/** predefined property solid cyan */
#define svt_properties_solid_cyan new svt_properties(svt_color_cyan)
/** predefined property solid tan */
#define svt_properties_solid_tan new svt_properties(svt_color_tan)
/** predefined property solid silver */
#define svt_properties_solid_silver new svt_properties(svt_color_silver)
//#endif

#define SVT_DL_SPHERE 100

//make new sphere objects
#define NEW_RED_SPHERE(x,y,z) new svt_sphere(new svt_pos(x,y,z), svt_properties_solid_red)
#define NEW_GREEN_SPHERE(x,y,z) new svt_sphere(new svt_pos(x,y,z), svt_properties_solid_green)
#define NEW_BLUE_SPHERE(x,y,z) new svt_sphere(new svt_pos(x,y,z), svt_properties_solid_blue)
//new hemisphere objects
#define NEW_RED_HEMISPHERE(x,y,z) new svt_sphere(new svt_pos(x,y,z), svt_properties_solid_red)
#define NEW_GREEN_HEMISPHERE(x,y,z) new svt_sphere(new svt_pos(x,y,z), svt_properties_solid_green)
#define NEW_BLUE_HEMISPHERE(x,y,z) new svt_sphere(new svt_pos(x,y,z), svt_properties_solid_blue)
//new laser objects
#define NEW_LASER(x,y,z) new svt_laser(new svt_pos(x,y,z), svt_properties_solid_yellow)
#define NEW_LOGO(x,y,z) new svt_logo(new svt_pos(x,y,z), svt_properties_solid_blue)

#endif
#endif
