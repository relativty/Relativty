//on linux & mingw stricmp doesn't exists so i re-implemented it

#ifndef MSVC

int imp_stricmp(
   const char *string1,
   const char *string2
);

#else

#define imp_stricmp _stricmp

#endif
