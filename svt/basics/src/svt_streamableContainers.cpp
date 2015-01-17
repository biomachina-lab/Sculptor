#include <svt_streamableContainers.h>


bool svt_streamableContainers::sm_bStreamSize = true;
bool svt_streamableContainers::sm_bAppend = true;


bool svt_streamableContainers::streamSize() {return sm_bStreamSize;}
void svt_streamableContainers::streamSize(bool b) {sm_bStreamSize=b;}

bool svt_streamableContainers::append() {return sm_bAppend;}
void svt_streamableContainers::append(bool b) {sm_bAppend=b;}


