/* PS3 SDK math.h wrapper - fixes _FSin namespace issues when _NO_CPP_INLINES is defined */
#ifndef _PS3_MATH_FIX_H_
#define _PS3_MATH_FIX_H_

/* Include the real SDK math.h */
#include_next <math.h>

/* When _NO_CPP_INLINES is defined, the SDK math.h uses macros that expand to _FSin etc.
 * These are declared in the std namespace via _C_STD_BEGIN, but the macros don't use
 * the _CSTD prefix to access them. Fix by bringing them into global namespace. */
#if defined(__cplusplus) && defined(_NO_CPP_INLINES)

/* Import the internal math functions from std namespace to global namespace */
using _CSTD _FSin;
using _CSTD _FCosh;
using _CSTD _FLog;
using _CSTD _FSinh;
using _CSTD _Sin;
using _CSTD _Cosh;
using _CSTD _Log;
using _CSTD _Sinh;
using _CSTD _LSin;
using _CSTD _LCosh;
using _CSTD _LLog;
using _CSTD _LSinh;

#endif /* __cplusplus && _NO_CPP_INLINES */

#endif /* _PS3_MATH_FIX_H_ */
