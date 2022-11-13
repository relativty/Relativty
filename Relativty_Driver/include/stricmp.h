//on linux stricmp doesn't exists so i re-implemented it

#ifdef __unix__

int _stricmp(
   const char *string1,
   const char *string2
);

#endif
