/** @file
 *
 * @brief BLJFlash target.
 *
 * @par
 * COPYRIGHT NOTICE: (c) 2017 Byte Lab Grupa d.o.o.
 * All rights reserved.
 */


//-------------------------- CONSTANTS & MACROS -------------------------------

#ifndef BURNTO
# error BURNTO not defined!
# define BURNTO
#endif

#define _STRINGIFY(x...) #x
#define STRINGIFY(x) _STRINGIFY(x)


//----------------------------- STATIC DATA -----------------------------------

static const char burnto[] __attribute__((section(".burnto"), used)) =
    "BURNTO:" STRINGIFY(BURNTO);

