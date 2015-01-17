
//#include <string.h>

#include <svt_bfstream.h>

///////////////////////////////////////////////////////////////////////////
//                                                                       //
//  class: svt_bstream_static                                            //
//                                                                       //
///////////////////////////////////////////////////////////////////////////

//
// init static members
//

svt_SwapMode svt_bstream_global::sm_eSwapMode = SWAP_BIG_ENDIAN;

static Int16 sTest=0x0102;
bool svt_bstream_global::sm_bSwapping = (*((char*)&sTest) == 0x01);
//bool svt_bstream_global::sm_bSwapping = svt_BigEndian();




void svt_bstream_global::setSwapMode(svt_SwapMode m) {
  if (m != AS_GLOBAL) {
    sm_eSwapMode = m;
	sm_bSwapping =    (  svt_BigEndian() && (m==SWAP_BIG_ENDIAN)    )
	               || ( !svt_BigEndian() && (m==SWAP_LITTLE_ENDIAN) );
  }
}



svt_SwapMode svt_bstream_global::swapMode() {
  return sm_eSwapMode;
}



bool svt_bstream_global::swapping () {
  return sm_bSwapping;
}





