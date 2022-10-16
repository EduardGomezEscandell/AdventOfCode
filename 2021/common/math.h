#ifndef MATH_INCLUDED_H
#define MATH_INCLUDED_H

#define MAX(a,b) (((a) > (b)) ? (a) : (b))
#define MIN(a,b) (((a) < (b)) ? (a) : (b))

#define CONSTRAIN(a,x,b) MAX(a, MIN(x, b))

#endif
