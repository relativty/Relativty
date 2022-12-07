#ifndef MSVC

#include <strings.h>
#define imp_stricmp strcasecmp

#else

#define imp_stricmp _stricmp

#endif
