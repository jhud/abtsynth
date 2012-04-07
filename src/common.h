#ifndef COMMON_H
#define COMMON_H

#define RandFloat() (double(qrand()) / double(RAND_MAX))
#define RandFloatNeg() (2.0 * (double(qrand()) / double(RAND_MAX)) - 1.0)

#endif // COMMON_H
