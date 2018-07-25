/*
 * measure.c
 *
 *  Created on: 25 Oct 2017
 *      Author: 99141145
 */

#include "measure.h"
#include "OS.h"
#include "packet.h"
#include "Flash.h"
#include "RTC.h"
#include "selftest.h"
#include <math.h>
#include "types.h"

static const uint8_t CLEAR_TARIFF = 0xFFu;


OS_ECB *MeasureSemaphore;
Tpower Power_Buffer;
Mvalues Measured_Values;
Ttemp temp_values;


//addresses for the tariffs
uint8_t *TariffIndexAddress;

float const TariffArray[] = {22.235, 1.713, 4.100, 4.400, 2.109};
float periodEnergy;

/*! @brief If the chosen option for tariff is 0, returns which index of tariff should be used for a particular time
 *
 *  @return int
 */
int tariffTimeOpt();

/*! @brief Calculates the average power
 *
 *  @return void
 */
void calcAvgPower(float powersum);

/*! @brief Calculates the energy and adds to total energy consumed
 *
 *  @return void
 */
void calcEnergy(float powerSum);

/*! @brief Calculates the cost and adds to total cost
 *
 *  @return void
 */
void calcCost();

/*! @brief Calculates the RMS values for voltage and current
 *
 *  @return void
 */
void calcRMS(float voltrmssum, float amprmssum);

/*! @brief Calculates the power factor (P/VI)
 *
 *  @return void
 */
void calcPowFact();

/*! @brief Calculates the frequency of the input wave
 *
 *  @return void
 */
void calcFrequency(int highestsampleindex, int lowestsampleindex);

uint32_t convtoq(float fvalue);
float convtof(uint32_t *Tariff);

/*! @brief Initializes the measure class
 *
 *  @return bool
 */
bool Measure_Init()
{
  MeasureSemaphore = OS_SemaphoreCreate(0);
  Power_Buffer.powerNb = 0;
  Measured_Values.TotalUpTime = 0;
  Measured_Values.AvgPower = 0;
  Measured_Values.TotalCost = 0;
  Measured_Values.TotalEnergy = 0;
  Measured_Values.VoltRMS = 0;
  Measured_Values.AmpRMS = 0;
  Measured_Values.PowFact = 0;
  Measured_Values.Frequency = 0;

  return true;
}

/*! @brief Does all of the calculations per period (16 samples collected)
 *
 *  @return void
 */
void Measure_Calc(void *args)
{
  for(;;)
  {
    OS_SemaphoreWait(MeasureSemaphore, 0);
    float samplepowersum = 0;
    float voltrmssum = 0;
    float amprmssum = 0;
    float highestsample = 0;
    float lowestsample = 0;
    float highestsampleindex = 0;
    float lowestsampleindex = 0;

    //determine the values needed for calculating energy and power factor.
    for(int i = 0; i < POWER_SAMPLE_SIZE; i++)
    {
      samplepowersum = samplepowersum + Power_Buffer.power_array[i];
      voltrmssum = voltrmssum + pow(voltage_array[i], 2);
      amprmssum = amprmssum + pow(current_array[i], 2);
      if(voltage_array[i] > highestsample)
      {
        highestsample = voltage_array[i];
        highestsampleindex = i;
      }
      if(voltage_array[i] < lowestsample)
      {
        lowestsample = voltage_array[i];
        lowestsampleindex = i;
      }
    }
    calcEnergy(samplepowersum);
    calcAvgPower(samplepowersum);
    calcCost();
    calcRMS(voltrmssum, amprmssum);
    calcPowFact(highestsampleindex, lowestsampleindex);
    calcFrequency(highestsampleindex, lowestsampleindex);
//    Energy = temppower * 1.25;
//    PeakEnergy = PeakEnergy + Energy;
//    float tempvar = convtof(Tariff1);
//    Packet_Put(0xad, (uint8_t)temppower, (uint8_t)convtof(Tariff1), 0);
//    Packet_Put(0xad, (uint8_t)convtof(Tariff1), 0, 0);
  }
}

float Measure_Scale_Volt(int16_t voltsample)
{
  if(voltsample < 0)
  {
    return (((float)voltsample / 3276.8) * 100);
  }
  else
  {
    return (((float)voltsample / 3276.7) * 100);
  }
}

float Measure_Scale_Cur(int16_t ampsample)
{
  if(ampsample < 0)
  {
    return (float)ampsample / 3276.8;
  }
  else
  {
    return (float)ampsample / 3276.7;
  }
}

void Measure_UpTimeTick()
{
  if(SelftestMode)
  {
    Measured_Values.TotalUpTime =  Measured_Values.TotalUpTime + 3600;
  }
  else
  {
    Measured_Values.TotalUpTime =  Measured_Values.TotalUpTime + 1;
  }
}

void Measure_TariffInit()
{
  Flash_AllocateVar((void *) &TariffIndexAddress, 1);
  if (*TariffIndexAddress == CLEAR_TARIFF)
  {
    Flash_Write8(TariffIndexAddress, 1);
  }
}

int tariffTimeOpt()
{
  uint8_t days, hours, minutes, seconds;
  if(SelftestMode)
  {
    RTC_Format_Time(testClock, &days, &hours, &minutes, &seconds);
  }
  else
  {
    RTC_Get(&hours, &minutes, &seconds);
    hours = hours%24;
  }
  if(hours >= 14 && hours <= 19)
  {
    return 0;
  }
  else if((hours >= 7 && hours <= 13) || (hours >= 20 && hours <= 21))
  {
    return 4;
  }
  else
  {
    return 5;
  }
}

bool Measure_TariffFunction(uint8_t option) //called by main to change the option
{
   //Flash_AllocateVar((void *) &TariffIndexAddress, 1);
  uint8_t tempvar = option;
  OS_DisableInterrupts();
  Flash_Write8(TariffIndexAddress, option);
  OS_EnableInterrupts();
  return true;
}

void calcAvgPower(float powersum)
{
  Measured_Values.AvgPower =  powersum /16;
}

void calcEnergy(float powerSum)
{
  periodEnergy = (powerSum * (1.25/1000)); //watts * seconds = gives joules (watts per sec)
  periodEnergy = periodEnergy / 3600000; //gives kWh
  if(SelftestMode)
  {
    periodEnergy = periodEnergy*3600;
  }
    Measured_Values.TotalEnergy = Measured_Values.TotalEnergy + (periodEnergy);
}

void calcCost()
{
  if(*TariffIndexAddress == 1)
  {
    Measured_Values.TotalCost = Measured_Values.TotalCost + ( (TariffArray[tariffTimeOpt()]/100) * periodEnergy);
  }
  else
  {
    Measured_Values.TotalCost = Measured_Values.TotalCost + ( (TariffArray[*TariffIndexAddress-1]/100) * periodEnergy);
  }
}

void calcRMS(float voltrmssum, float amprmssum)
{
  Measured_Values.VoltRMS = sqrt(1/16 * voltrmssum);
  Measured_Values.AmpRMS = sqrt(1/16 * amprmssum);
}

void calcPowFact()
{
  Measured_Values.PowFact = Measured_Values.AvgPower / (Measured_Values.VoltRMS * Measured_Values.AmpRMS);
}

void calcFrequency(int highestsampleindex, int lowestsampleindex)
{
  float indexdifference;
  float periodval;
  if(highestsampleindex > lowestsampleindex)
  {
    indexdifference = highestsampleindex - lowestsampleindex;
  }
  else
  {
    indexdifference = lowestsampleindex - highestsampleindex;
  }

  periodval = indexdifference * 2 * 1.25; //index difference multiply by 2 to get period, multiply by 1.25 ms to get how long period is.

  Measured_Values.Frequency = 1000/periodval; // 1000ms / periodval will give frequency
}


//functions which return values
//<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<

bool Measure_UpTimeFunction(uint8_t *days, uint8_t *hours, uint8_t *minutes, uint8_t *seconds)
{
  RTC_Format_Time(Measured_Values.TotalUpTime, days, hours, minutes, seconds);
  return true;
}

bool Measure_PowerFunction(float *temppower)
{
  *temppower = Measured_Values.AvgPower;
  return true;
}

bool Measure_EnergyFunction(float *tempenergy)
{
  *tempenergy = Measured_Values.TotalEnergy;
  return true;
}

bool Measure_CostFunction(float *tempcost)
{
  *tempcost = Measured_Values.TotalCost;
  return true;
}

bool Measure_VoltRMSFunction(float *tempvrms)
{
  *tempvrms = Measured_Values.VoltRMS;
  return true;
}

bool Measure_AmpRMSFunction(float *temparms)
{
  *temparms = Measured_Values.AmpRMS;
  return true;
}

bool Measure_PowFactFunction(float *temppowfact)
{
  *temppowfact = Measured_Values.PowFact;
  return true;
}

bool Measure_FrequencyFunction(float *tempfreq)
{
  *tempfreq = Measured_Values.Frequency;
  return true;
}
//<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<


//will be used to store values into flash by converting float to fixed-point
uint32_t convtoq(float fvalue)
{
  return round(fvalue * pow(2,24) * 1.0);
}


float convtof(uint32_t *Tariff)
{
  return (float)(*Tariff / pow(2,24) * 1.0);
}
