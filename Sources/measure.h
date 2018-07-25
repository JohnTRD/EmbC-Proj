/*
 * measure.h
 *
 *  Created on: 25 Oct 2017
 *      Author: 99141145
 */

#ifndef SOURCES_MEASURE_H_
#define SOURCES_MEASURE_H_

#include "OS.h"
#include "types.h"

#define POWER_SAMPLE_SIZE 16

typedef struct
{
  uint16_t volatile powerNb;
  float power_array[POWER_SAMPLE_SIZE]; // array for p=vi (already been scaled, absolute)
} Tpower;

typedef struct
{
  float TotalEnergy;
  float AvgPower;
  double TotalUpTime;
  float TotalCost;
  float VoltRMS;
  float AmpRMS;
  float PowFact;
  float Frequency;
} Mvalues;

float voltage_array[POWER_SAMPLE_SIZE]; //array for voltage samples (scaled, absolute)
float current_array[POWER_SAMPLE_SIZE]; //array for current samples (scaled, absolute)


typedef struct
{
  uint16_t volatile tempNb;
  float temporary_array[128]; // array for p=vi (already been scaled, absolute)
} Ttemp;



extern float const TariffArray[5];
extern Tpower Power_Buffer;
extern OS_ECB *MeasureSemaphore;
extern Ttemp temp_values;

//<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<
/*! @brief Initializes the measure class
 *
 *  @return bool
 */
bool Measure_Init();

/*! @brief Does all of the calculations per period (16 samples collected)
 *
 *  @return void
 */
void Measure_Calc(void *args);

/*! @brief Scales the input voltage sample to become larger and simulate real values. Also converts samples to actual amplitude
 *
 *  @return float
 */
float Measure_Scale_Volt(int16_t voltsample);

/*! @brief Scales the input current sample to become larger and simulate real values. Also converts samples to actual amplitude.
 *
 *  @return float
 */
float Measure_Scale_Cur(int16_t ampsample);

/*! @brief Increments the meter's uptime every second
 *
 *  @return void
 */
void Measure_UpTimeTick();

/*! @brief Initializes tariff by writing tariff option into flash
 *
 *  @return void
 */
void Measure_TariffInit();

/*! @brief Changes the tariff option based on user input
 *
 *  @return bool
 */
bool Measure_TariffFunction(uint8_t option);

/*! @brief Returns the uptime of the energy meter
 *
 *  @return bool
 */
bool Measure_UpTimeFunction(uint8_t *days, uint8_t *hours, uint8_t *minutes, uint8_t *seconds);

/*! @brief Returns the average power
 *
 *  @return bool
 */
bool Measure_PowerFunction(float *temppower);

/*! @brief Returns the total energy consumed
 *
 *  @return bool
 */
bool Measure_EnergyFunction(float *tempenergy);

/*! @brief Returns the total cost
 *
 *  @return bool
 */
bool Measure_CostFunction(float *tempcost);

/*! @brief Returns the RMS of the voltage
 *
 *  @return bool
 */
bool Measure_VoltRMSFunction(float *tempvrms);

/*! @brief Returns the RMS of the current
 *
 *  @return bool
 */
bool Measure_AmpRMSFunction(float *temparms);

/*! @brief Returns the power factor
 *
 *  @return bool
 */
bool Measure_PowFactFunction(float *temppowfact);

/*! @brief Returns the frequency of the input wave
 *
 *  @return bool
 */
bool Measure_FrequencyFunction(float *tempfreq);

//<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<
//FALLBACK FUNCTION DEFINITIONS
//functions
//bool Measure_Init();
//void Measure_Calc(void *args);
//float Measure_Scale_Volt(int16_t voltsample);
//float Measure_Scale_Cur(int16_t ampsample);
//void Measure_UpTimeTick();
////void Measure_CheckTime();
//void Measure_TariffInit();
//
//bool Measure_TariffFunction(uint8_t option);
////bool Measure_Time1Function();
////bool Measure_Time2Function();
//bool Measure_UpTimeFunction(uint8_t *days, uint8_t *hours, uint8_t *minutes, uint8_t *seconds);
//bool Measure_PowerFunction();
//bool Measure_EnergyFunction();
//bool Measure_CostFunction();
//bool Measure_VoltRMSFunction(float *tempvrms);
//bool Measure_AmpRMSFunction(float *temparms);
//bool Measure_PowFactFunction(float *temppowfact);
//bool Measure_FrequencyFunction(float *tempfreq);
//
//
//void calcAvgPower(float powersum);
//void calcEnergy(float powerSum);
//void calcCost();
//void calcRMS(float voltrmssum, float amprmssum);
//void calcPowFact();
//void calcFrequency(int highestsampleindex, int lowestsampleindex);
//int tariffTimeOpt();
//
//uint32_t convtoq(float fvalue);
//float convtof(uint32_t *Tariff);
//void writeTariffFlash();


#endif /* SOURCES_MEASURE_H_ */
