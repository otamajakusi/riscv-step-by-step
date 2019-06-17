#pragma once

#include <stdio.h>

#define ASSERT(x) { \
   if (!(x)) { \
       printf("error: assert failure \""#x"\" \n"); \
       exit(1); \
   } \
}
