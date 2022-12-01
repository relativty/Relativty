#include "../include/stricmp.hpp"
#include <stdlib.h>
#include <string.h>
#include <ctype.h>

//TODO: remake with c++
int imp_stricmp(const char *string1const, const char *string2const) {
  char * string1 = strdup(string1const);
  char * string2 = strdup(string2const);


  for(int i = 0; string1[i]; i++)
    string1[i] = tolower(string1[i]);

  for(int i = 0; string2[i]; i++)
    string2[i] = tolower(string2[i]);

  int cmp = strcmp(string1, string2);

  free(string1);
  free(string2);
  return cmp;
}
