//https://botsikas.blogspot.com/2011/12/strcasecmp-identifier-not-found-when.html

#if defined(_WIN32) || defined(_WIN64)

#define snprintf _snprintf
#define vsnprintf _vsnprintf
#define strcasecmp _stricmp
#define strncasecmp _strnicmp
#define imp_stricmp strcasecmp
#else

#define imp_stricmp _stricmp

#endif
