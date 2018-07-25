/*
 * HMI.c
 *
 *  Created on: 29 Oct 2017
 *      Author: 99141145
 */

#include "HMI.h"
#include "MK70F12.h"
#include "OS.h"
#include "RTC.h"
#include "measure.h"
#include "types.h"
#include "packet.h"
#include "UART.h"
#include "FTM.h"
#include "selftest.h"
#include <stdio.h>
#include <math.h>

#define DORMANT 0
#define METER_TIME 1
#define AVG_POWER 2
#define TOTAL_ENERGY 3
#define TOTAL_COST 4

static int currentmode = 0;
int displaytimer = 16;

/*! @brief Changes the display mode by cycling through the different modes
 *
 *  @return void
 */
void display_Mode();

//NFormat HMIValue;
OS_ECB *HMISemaphore;

bool HMI_Init()
{
  HMISemaphore = OS_SemaphoreCreate(0);

  //enable portd
  SIM_SCGC5 |= SIM_SCGC5_PORTD_MASK;
  //set pin1 portd
  PORTD_PCR0 &= ~PORT_PCR_MUX_MASK;
  PORTD_PCR0 |= PORT_PCR_MUX(0x1);
  //set interrupts for pin0
  PORTD_PCR0 |= PORT_PCR_ISF_MASK;
  PORTD_PCR0 |= PORT_PCR_IRQC(10);
  //pullup resistors
  PORTD_PCR0 |= PORT_PCR_PE_MASK;
  PORTD_PCR0 |= PORT_PCR_PS_MASK;
  //initialize nvic
  NVICICPR2 = (1 << (90 % 32));
  NVICISER2 = (1 << (90 % 32));


  return true;
}

//unused function
//void HMI_Display_Listener()
//{
//  for(;;)
//  {
//    OS_SemaphoreWait(HMISemaphore, 0);
//    display_Mode();
//  }
//}

void display_Mode()
{
  switch(currentmode)
  {
    case DORMANT:
      currentmode = METER_TIME;
      break;
    case METER_TIME:
      currentmode = AVG_POWER;
      break;
    case AVG_POWER:
      currentmode = TOTAL_ENERGY;
      break;
    case TOTAL_ENERGY:
      currentmode = TOTAL_COST;
      break;
    case TOTAL_COST:
      currentmode = METER_TIME;
      break;
  }
  displaytimer = 0;
  HMI_Display();
}

void HMI_Display()
{
  uint8_t days, hours, minutes, seconds, HMIbuffer[128];
  float tempvalue;

  //counts HMI display
  if(!SelftestMode) //disable counter when is selftest mode
  {
    if(displaytimer < 15)
    {
      displaytimer++;
    }
    else
    {
      currentmode = DORMANT;
    }
  }

  switch(currentmode)
  {
    int real, frac;
    case METER_TIME:
      Measure_UpTimeFunction(&days, &hours, &minutes, &seconds);
      if(!(days > 99))
      {
        sprintf(HMIbuffer, "Metering Time: %02d:%02d:%02d:%02d\n", days, hours, minutes, seconds);
      }
      else
      {
        sprintf(HMIbuffer, "Metering Time: xx:xx:xx:xx\n", days, hours, minutes, seconds);
      }
      UART_OutString(HMIbuffer);
      break;
    case AVG_POWER:
      Measure_PowerFunction(&tempvalue);
      real = floor(tempvalue);
      frac = HMI_Formatnumber(tempvalue);
      sprintf(HMIbuffer, "Average Power: %d.%03d kWh\n", real, frac);
      UART_OutString(HMIbuffer);
      break;
    case TOTAL_ENERGY:
      Measure_EnergyFunction(&tempvalue);
      real = floor(tempvalue);
      frac = HMI_Formatnumber(tempvalue);
      if (!(real > 999))
      {
        sprintf(HMIbuffer, "Total Energy: %d.%03d kW\n", real, frac);
      }
      else
      {
        sprintf(HMIbuffer, "Total Energy: xxx.xxx kW\n", real, frac);
      }
      UART_OutString(HMIbuffer);
      break;
    case TOTAL_COST:
      Measure_CostFunction(&tempvalue);
      real = floor(tempvalue);
      frac = HMI_Formatcurrency(tempvalue);
      if(!(real > 9999) && !(frac > 99))
      {
        sprintf(HMIbuffer, "Total Cost: $ %d.%02d \n", real, frac);
      }
      else
      {
        sprintf(HMIbuffer, "Total Cost: $ xxxx.xx \n", real, frac);
      }
      UART_OutString(HMIbuffer);
      break;
    case DORMANT:
      break;
  }
}


int HMI_Formatcurrency(float value)
{
  float floatnumber = value;
  float wholepart = floor(value);
  return (floatnumber - wholepart)*100;
}

int HMI_Formatnumber(float value)
{
  float floatnumber = value;
  float wholepart = floor(value);
  return (floatnumber - wholepart)*1000;
}


void __attribute__ ((interrupt)) SW1_ISR(void)
{
  OS_ISREnter();
  if(PORTD_PCR0 & PORT_PCR_ISF_MASK)
  {
    PORTD_ISFR |= PORT_ISFR_ISF(0);
    //put function here to output
    display_Mode();
    //OS_SemaphoreSignal(HMISemaphore);
  }
  OS_ISRExit();
}
