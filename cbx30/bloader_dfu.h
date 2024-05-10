/** @file bloader_dfu.h
*
* @brief See source file.
*
* @par
* COPYRIGHT NOTICE: (c) 2017 Byte Lab Grupa d.o.o.
* All rights reserved.
*/

#ifndef CROSSBOX_BLOADER_DFU_H
#define CROSSBOX_BLOADER_DFU_H

#ifdef __cplusplus
extern "C" {
#endif

//------------------------------ INCLUDES -------------------------------------
#include <stdint.h>

//-------------------------- CONSTANTS & MACROS -------------------------------

//----------------------------- DATA TYPES ------------------------------------

//---------------------- PUBLIC FUNCTION PROTOTYPES ---------------------------
int check_start_dfu(void);
int check_bloader_status(void);

#ifdef __cplusplus
}
#endif

#endif //CROSSBOX_BLOADER_DFU_H
