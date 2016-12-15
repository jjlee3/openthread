#pragma once

#define MACRO_WIDEN_STR2(x)     L ## x
#define MACRO_WIDEN_STR(x)      MACRO_WIDEN_STR2(x)

#define MACRO_NODEN_STR2(x)     x
#define MACRO_NODEN_STR(x)      MACRO_NODEN_STR2(x)

#define MACRO_STR2(x)           # x
#define MACRO_STR(x)            MACRO_STR2(x)

#ifndef __FILEW__
#define __FILEW__               MACRO_WIDEN_STR(__FILE__)
#endif // __FILEW__

#ifndef __FILEA__
#define __FILEA__               MACRO_NODEN_STR(__FILE__)
#endif // __FILEA__

#ifndef __FUNCTIONW__
#define __FUNCTIONW__           MACRO_WIDEN_STR(__FUNCTION__)
#endif // __FUNCTIONW__

#ifndef __FUNCTIONA__
#define __FUNCTIONA__           MACRO_NODEN_STR(__FUNCTION__)
#endif // __FUNCTIONA__

#ifndef __LINEW__
#define __LINEW__               MACRO_WIDEN_STR(MACRO_STR(__LINE__))
#endif // __LINEW__

#ifndef __LINEA__
#define __LINEA__               MACRO_NODEN_STR(MACRO_STR(__LINE__))
#endif // __LINEA__

#ifndef __LOCW__
#define __LOCW__                __FILEW__ MACRO_WIDEN_STR("(") __LINEW__ MACRO_WIDEN_STR(")")
#endif // __LOCW__

#ifndef __LOCA__
#define __LOCA__                __FILEA__ MACRO_NODEN_STR("(") __LINEA__ MACRO_NODEN_STR(")")
#endif // __LOCA__

#ifndef __LOC_FUNCTION_W__
#define __LOC_FUNCTION_W__      __LOCW__ MACRO_WIDEN_STR(" ") __FUNCTIONW__
#endif // __LOC_FUNCTION_W__

#ifndef __LOC_FUNCTION_A__
#define __LOC_FUNCTION_A__      __LOCA__ MACRO_NODEN_STR(" ") __FUNCTIONA__
#endif // __LOC_FUNCTION_A__
