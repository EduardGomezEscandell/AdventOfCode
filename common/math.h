#ifndef MATH_INCLUDED_H
#define MATH_INCLUDED_H

 #define MAX(a,b) \
   ({ __typeof__(a) _a = (a); \
      __typeof__(b) _b = (b); \
      _a > _b ? _a : _b; })

#define MIN(a,b) \
   ({ __typeof__(a) _a = (a); \
      __typeof__(b) _b = (b); \
      _a < _b ? _a : _b; })

#define CONTAINER_LEN(c) ((c).end - (c).begin)

#endif