/** @file dma.h
*
* @brief See source file.
*
* @par
* COPYRIGHT NOTICE: (c) 2019 Byte Lab Grupa d.o.o.
* All rights reserved.
*/

#ifndef CROSSBOX_BSP_DMA_H
#define CROSSBOX_BSP_DMA_H


//------------------------------ INCLUDES -------------------------------------
#include <stdbool.h>

//-------------------------- CONSTANTS & MACROS -------------------------------

//----------------------------- DATA TYPES ------------------------------------

//---------------------- PUBLIC FUNCTION PROTOTYPES ---------------------------

void bsp_dma_init(void);

void bsp_dma_process_data(void);

#endif //CROSSBOX_BSP_DMA_H
