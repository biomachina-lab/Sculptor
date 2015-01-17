
#include <svt_matrix4.h>


bool svt_scaleInfo::sm_bEverScaled = false;

void svt_scaleInfo::setEverScaled() {sm_bEverScaled=true;}
bool svt_scaleInfo::everScaled() {return sm_bEverScaled;}

