/*
 * selftest.h
 *
 *  Created on: 4 Nov 2017
 *      Author: 99141145
 */

#ifndef SOURCES_SELFTEST_H_
#define SOURCES_SELFTEST_H_

extern bool SelftestMode;
extern double testClock;

/*! @brief Initializes Selftest class
 *
 *  @return bool
 */
bool Selftest_Init();

/*! @brief Starts selftest by getting current RTC time
 *
 *  @return void
 */
void Selftest_Start();

/*! @brief Output the selftest wave
 *
 *  @return void
 */
void Selftest_Wave();

/*! @brief Increment the selftest time (like RTC but increases by 1 hr every second)
 *
 *  @return void
 */
void SelfTest_ClockTick();

/*! @brief Changes the voltage multiplier for the voltage selftest wave
 *
 *  @return bool
 */
bool SelfTest_ChangeVoltMult(uint16_t voltagestep);

/*! @brief Changes the current multiplier for the current selftest wave
 *
 *  @return bool
 */
bool SelfTest_ChangeAmpMult(uint16_t currentstep);




#endif /* SOURCES_SELFTEST_H_ */
