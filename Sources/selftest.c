/*
 * selftest.c
 *
 *  Created on: 4 Nov 2017
 *      Author: 99141145
 */

#include "analog.h"
#include "measure.h"
#include "RTC.h"
#include "selftest.h"

//signal array with a frequency of 50hz and amplitude of 1
float const selftestarray[128] = {-2500, -1868, -735, 507, 1685, 2591,
    3126, 3187, 2774, 1940, 827, -416, -1598, -2523,
    -3044, -3111, -2705, -1877, -742, 497, 1672, 2588,
    3124, 3186, 2771, 1941, 824, -416, -1598, -2524,
    -3049, -3115, -2707, -1880, -742, 496, 1669, 2588,
    3126, 3186, 2773, 1944, 823, -419, -1596, -2524,
    -3046, -3109, -2710, -1886, -751, 489, 1663, 2579,
    3123, 3184, 2776, 1945, 836, -402, -1587, -2518,
    -3044, -3111, -2711, -1887, -753, 487, 1660, 2582,
    3120, 3187, 2775, 1946, 837, -401, -1587, -2517,
    -3043, -3107, -2712, -1883, -751, 489, 1664, 2589,
    3124, 3184, 2779, 1958, 845, -394, -1579, -2511,
    -3042, -3112, -2718, -1891, -765, 480, 1657, 2576,
    3115, 3190, 2781, 1957, 845, -393, -1580, -2513,
    -3042, -3112, -2713, -1894, -763, 478, 1659, 2578,
    3120, 3192, 2779, 1956, 847, -394, -1579, -2512, -3040, -3112};

//default multiplier values
static const float VOLTDEFAULT = 282.8;
static const float AMPDEFAULT = 1;


bool SelftestMode;
float voltagemult;
float currentmult;
double testClock;



bool Selftest_Init()
{
  SelftestMode = false;
  voltagemult = VOLTDEFAULT;
  currentmult = AMPDEFAULT;
}

void Selftest_Start()
{
  uint8_t hours, minutes, seconds;
  RTC_Get(&hours, &minutes, &seconds);
  testClock = (hours*3600)+(minutes*60)+(seconds);
  SelftestMode = true;
}

//produce the test wave
void Selftest_Wave()
{
  Analog_Put(0, selftestarray[temp_values.tempNb]*voltagemult/100);
  Analog_Put(1, selftestarray[temp_values.tempNb]*currentmult);
  temp_values.tempNb++;
  if(temp_values.tempNb >= 128)
  {
    temp_values.tempNb = 0;
  }
}

//add 1 hours every tick
void SelfTest_ClockTick()
{
  testClock = testClock + 3600;
}


bool SelfTest_ChangeVoltMult(uint16_t voltagestep)
{
  voltagemult = VOLTDEFAULT + (voltagestep * 0.03052);
}


bool SelfTest_ChangeAmpMult(uint16_t currentstep)
{
  currentmult = AMPDEFAULT + (currentstep * 0.0003052);
}
