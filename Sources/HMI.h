/*
 * HMI.h
 *
 *  Created on: 29 Oct 2017
 *      Author: 99141145
 */

#ifndef SOURCES_HMI_H_
#define SOURCES_HMI_H_

#include "types.h"
#include "../Library/OS.h"

typedef struct
{
  float floatnumber;
  int wholepart;
  float fractionpart;
}NFormat;

//extern NFormat HMIValue;
extern OS_ECB *HMISemaphore;

/*! @brief Initializes the HMI class
 *
 *  @return bool
 */
bool HMI_Init();

/*! @brief Outputs string to terminal
 *
 *  @return void
 */
void HMI_Display();

/*! @brief Formats number for output
 *
 *  @return int
 */
int HMI_Formatnumber(float value);

/*! @brief Formats number into currency for output
 *
 *  @return int
 */
int HMI_Formatcurrency(float value);

/*! @brief Interrupt for switch 1
 *
 *  @return void
 */
void __attribute__ ((interrupt)) SW1_ISR(void);

#endif /* SOURCES_HMI_H_ */
