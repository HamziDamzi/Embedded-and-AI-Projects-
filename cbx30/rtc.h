/** @file bsp_rtc.h
*
* @brief See source file.
*
* @par
* COPYRIGHT NOTICE: (c) 2017 Byte Lab Grupa d.o.o.
* All rights reserved.
*/

#ifndef __BSP_RTC_H__
#define __BSP_RTC_H__

#ifdef __cplusplus
extern "C" {
#endif

//------------------------------ INCLUDES -------------------------------------
#include <stdint.h>
#include <stdbool.h>

//-------------------------- CONSTANTS & MACROS -------------------------------

//----------------------------- DATA TYPES ------------------------------------
typedef struct {
    uint16_t milisecs;
    uint8_t year;
    uint8_t month;
    uint8_t date;
    uint8_t hours;
    uint8_t minutes;
    uint8_t seconds;
} rtc_data_t;

//---------------------- PUBLIC FUNCTION PROTOTYPES ---------------------------
/**
 * @brief RTC init function
 */
void bsp_rtc_init(void);

/*!
* @brief Set RTC date and time
* @param[in] p_data - pointer to struct with date/time data
* @return void
*/
void bsp_rtc_data_set(rtc_data_t *p_data);

/**
 * @brief Safe read RTC data regardless of bypass shadow register feature.
 * @param[out] p_data - pointer to structure where to store date/time data/
 * @return 0 -> Get Ok
 *        1 -> Argument is NULL
 *        2 -> Error reading RTC
 */
int  bsp_rtc_data_get(rtc_data_t *p_data);

/*!
* @brief Get if RTC date/time is set.
* @return 1 if RTC is set, 0 otherwise.
*/
char bsp_rtc_is_valid(void);

/**
 * @brief Convert current RTC data or supplied RTC data to unix time.
 * @param p_data - RTC data to convert. If NULL function first read RTC and
 *                  then convert current time to unix time.
 * @return unix time
 */
int32_t bsp_get_unix_timestamp(rtc_data_t *p_data);

/**
 * @brief Extracts milliseconds from current RTC data
 * @return rtc subseconds
 */
int16_t bsp_get_milliseconds(void);

/**
 * @brief Run test on RTC to check if it is running.
 * @return 0 -> Test Ok
 *        1 -> Test Failed
 */
uint8_t bsp_rtc_osc_test (void);

/**
 * @brief Apply sub-second offset to RTC.
 * @param offset offset to apply - sub-seconds in range +-(RTC_PREDIV_S).
 *      Default +-255.
 * @return true on success. Note if bypass shadow register is enabled function
 *      will return fast but changes will not be immediately visible due to
 *      skipped synchronization. It takes ~21ms for changes to be visible.
 *      On the other side if bypass is disabled function will block that time.
 */
bool bsp_rtc_subseconds_offset_set(int16_t offset);

/**
 * @brief Reset millisecond tick.
 */
void bsp_rtc_tick_reset(void);

/**
 * @brief Get tick in milliseconds from last tick reset.
 * @return
 */
uint32_t bsp_rtc_tick_get(void);

/**
 * @brief Disable rtc features what cause bootloop when downgrading to older
 *      firmware versions.
 */
void bsp_rtc_compability_mode(void);

#ifdef __cplusplus
}
#endif

#endif //__BSP_RTC_H__

