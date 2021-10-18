#include "signal.h"

signal sig_0 = 0.0F;
signal sig_HALF = 0.5F;
signal sig_1 = 1.0F;
signal sig_2 = 2.0F;
signal sig_4 = 4.0F;
signal sig_12 = 12.0F;
signal sig_64 = 64.0F;
signal sig_128 = 128.0F;

signal sig_QUATER_PI = 0.78539816339F;
signal sig_HALF_PI = 1.57079632675F;
signal sig_PI = 3.1415926535F;
signal sig_TWO_PI = 6.2831853072F;

signal sig_NEG_1 = -1.0F;

signal sig_B_ = 1.2732395447F; //B = 4.0 / PI;
signal sig_C_ = -0.405284735F; //C = -4.0 /(PI_SQUARED);

const int32_t multDivider = 32767;
const int32_t neg_multDivider = -32767;
const float f_multDivider = 32767;

signal lastTestPhase = 0.0F;
