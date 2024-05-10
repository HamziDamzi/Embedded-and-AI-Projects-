/** @file bsp_rtc.c
*
* @brief A description of the module's purpose.
*
* @par
* COPYRIGHT NOTICE: (c) 2017 Byte Lab Grupa d.o.o.
* All rights reserved.
*/

//------------------------------ INCLUDES -------------------------------------
#include <inc/bsp/rtc.h>
#include <string.h>
#include <error_handler.h>
#include <inc/bsp/bsp.h>
#include <RTT.h>
#include <time.h>
#include <helpers.h>
#include <stm32l4xx_hal.h>
#include <blog.h>

//-------------------------------- MACROS -------------------------------------
/* Internal RTC defines */
#define RTC_TEST_MIN_VALUE          (950u)
#define RTC_TEST_MAX_VALUE          (1050u)
#define RTC_TEST_GET_TIMEOUT        (2000u)

// xtal [Hz] = (RTC_PREDIV_S +1) * (RTC_PREDIV_A +1)
#define RTC_PREDIV_S                (1023u)
#define RTC_PREDIV_A                (31u)

//----------------------------- DATA TYPES ------------------------------------

//--------------------- PRIVATE FUNCTION PROTOTYPES ---------------------------
/*!
* @brief Get RTC date and time
* @param[out] p_data - pointer to structure where to store date/time data/
* @return 0 -> Get Ok
 *        1 -> Argument is NULL
 *        2 -> Error reading RTC
*/
static int rtc_data_get_exec(rtc_data_t *p_data);

/**
 * @brief Bypass shadow register.
 *      Note, if bypass is enabled RTC set time is decreased (~250 to ~50us).
 *      but read is slower due to multiple read required - up ~21us (3 reads)
 *      in a worst case, ~14us (2 reads) typical.
 * @param enable - true to enable bypass, false disable bypass.
 */
static void rtc_bypass_shadow(bool enable);

/**
 * @brief Control RTC second interrupt.
 * @param enable - true to enable interrupt, false disable.
 */
static void rtc_second_interrupt(bool enable);

/**
 * @brief Convert RTC data to unix time.
 * @param p_data - pointer to struct containing RTC data.
 * @return unix time.
 */
static int32_t rtc_to_unix (const rtc_data_t *p_data);

/**
 * @brief Convert unix time to RTC data.
 * @param unixtime - unix time to convert.
 * @param p_data - pointer to struct where to store RTC data.
 */
static void unix_to_rtc(uint32_t unixtime, rtc_data_t *p_data);

//----------------------- STATIC DATA & CONSTANTS -----------------------------
static const uint32_t rtc_init_val               = 0x32F2u;
static const uint32_t rtc_set_val                = 0x32F3u;

static volatile uint32_t rtc_sec_ticks;
static volatile uint32_t rtc_start_tick;

static RTC_HandleTypeDef hrtc;
//------------------------------ GLOBAL DATA ----------------------------------

//---------------------------- PUBLIC FUNCTIONS -------------------------------

void bsp_rtc_init(void)
{
    RTC_HandleTypeDef *p_handle = (RTC_HandleTypeDef *)&hrtc;
    RTC_TimeTypeDef sTime;
    RTC_DateTypeDef sDate;

    uint32_t rtc_val;

    // Initialize RTC Only.
    hrtc.Instance = RTC;
    hrtc.Init.HourFormat = RTC_HOURFORMAT_24;
    hrtc.Init.AsynchPrediv = RTC_PREDIV_A;
    hrtc.Init.SynchPrediv = RTC_PREDIV_S;
    hrtc.Init.OutPut = RTC_OUTPUT_DISABLE;
    hrtc.Init.OutPutRemap = RTC_OUTPUT_REMAP_NONE;
    hrtc.Init.OutPutPolarity = RTC_OUTPUT_POLARITY_HIGH;
    hrtc.Init.OutPutType = RTC_OUTPUT_TYPE_OPENDRAIN;

    if (HAL_RTC_Init(p_handle) != HAL_OK)
    {
        BL_RAISE_ERROR("RTC init failed", EHALRTC);
        EPRINT("RTC init failed\n");
    }

    /**Initialize RTC and set the Time and Date
    */
    rtc_val = HAL_RTCEx_BKUPRead(p_handle, RTC_BKP_DR31);
    rtc_val = ((rtc_init_val == rtc_val) || (rtc_set_val == rtc_val));
    if (0 == rtc_val)
    {
        sTime.Hours = 0;
        sTime.Minutes = 0;
        sTime.Seconds = 0;
        sTime.DayLightSaving = RTC_DAYLIGHTSAVING_NONE;
        sTime.StoreOperation = RTC_STOREOPERATION_RESET;
        if (HAL_RTC_SetTime(p_handle, &sTime, RTC_FORMAT_BIN) != HAL_OK)
        {
            BL_RAISE_ERROR("RTC time setting failed", EHALRTC);
            EPRINT("RTC time setting failed\n");
        }

        sDate.WeekDay = RTC_WEEKDAY_MONDAY;
        sDate.Month = RTC_MONTH_JANUARY;
        sDate.Date = 1;
        sDate.Year = 0;

        if (HAL_RTC_SetDate(p_handle, &sDate, RTC_FORMAT_BIN) != HAL_OK)
        {
            BL_RAISE_ERROR("RTC date setting failed", EHALRTC);
            EPRINT("RTC date settig failed\n");
        }

        HAL_RTCEx_BKUPWrite(p_handle,RTC_BKP_DR31, rtc_init_val);
    }

    rtc_bypass_shadow(true);
    rtc_second_interrupt(true);
}


void bsp_rtc_data_set (rtc_data_t *p_data)
{
    RTC_HandleTypeDef *p_handle = (RTC_HandleTypeDef *)&hrtc;
    char result = (NULL != p_data);

    if (result)
    {
        RTC_TimeTypeDef sTime;
        memset(&sTime, 0, sizeof(sTime));
        sTime.Hours = p_data->hours;
        sTime.Minutes = p_data->minutes;
        sTime.Seconds = p_data->seconds;
        result = (HAL_OK == HAL_RTC_SetTime(p_handle, &sTime, RTC_FORMAT_BIN));
    }

    if (result)
    {
        RTC_DateTypeDef sDate;
        memset(&sDate, 0, sizeof(sDate));
        sDate.Year = p_data->year;
        sDate.Month = p_data->month;
        sDate.Date = p_data->date;
        sDate.WeekDay = RTC_WEEKDAY_MONDAY; // arbitrary, not used
        result = (HAL_OK == HAL_RTC_SetDate(p_handle, &sDate, RTC_FORMAT_BIN));
    }

    if (result)
    {
        HAL_RTCEx_BKUPWrite(p_handle,RTC_BKP_DR31, rtc_set_val);
    }

    if (!result)
    {
        BL_RAISE_ERROR("RTC date and time failed", EHALRTC);
        EPRINT("RTC date and time failed\n");
    }
}


int bsp_rtc_data_get (rtc_data_t *p_data)
{
    int result;

    if (hrtc.Instance->CR & RTC_CR_BYPSHAD)
    {
        // In a case of bypass shadow register feature enabled (sync bypassed)
        // one should read RTC data at least twice. If data of both reads match
        // data are valid. If not, read until matches.

        rtc_data_t data[2];
        uint8_t sample = 0;

        // Start with not valid data, ensures at least two reads.
        data[1].seconds = 99;

        do {
            result = rtc_data_get_exec(&data[sample]);
            sample = sample ^ 0x1;
        } while ((0 == result) &&
                 (0 != memcmp(&data[0], &data[1], sizeof(rtc_data_t))));

        if (0 == result)
        {
            memcpy(p_data, &data[0], sizeof(rtc_data_t));
        }
    }
    else
    {
        result = rtc_data_get_exec(p_data);
    }

    return result;
}


int32_t bsp_get_unix_timestamp (rtc_data_t *p_data)
{
    int32_t result;

    if (NULL == p_data)
    {
        rtc_data_t rtc_data;
        bsp_rtc_data_get(&rtc_data);
        result = rtc_to_unix(&rtc_data);
    }
    else
    {
        result = rtc_to_unix(p_data);
    }

    return result;
}

/**
 * @brief Extracts milliseconds from current RTC data or supplied RTC data
 * @param p_data - RTC data to extract from
 * @return rtc subseconds
 */
int16_t bsp_get_milliseconds()
{
    int16_t result;

    rtc_data_t rtc_data;
    bsp_rtc_data_get(&rtc_data);

    return rtc_data.milisecs;
}


char bsp_rtc_is_valid(void)
{
    RTC_HandleTypeDef *p_handle = (RTC_HandleTypeDef *)&hrtc;
    return (rtc_set_val == HAL_RTCEx_BKUPRead(p_handle, RTC_BKP_DR31));
}


bool bsp_rtc_subseconds_offset_set(int16_t offset)
{
    const uint32_t top = (RTC_PREDIV_S + 1);
    uint32_t ofs = abs16(offset);
    bool result = ((top > ofs) && (0 != offset));

    if (result)
    {
        uint32_t add1s;

        if (0 > offset)
        {
            // Pomicanje vremena u nazad je izvedeno tako da se doda offset na
            // postojeci counter. Efektivno se odgadja prijelaz u novu sekundu.
            add1s = RTC_SHIFTADD1S_RESET;
        }
        else
        {
            // Pomicanje vremena u naprijed je napravljeno tako da se doda
            // citava sekunda na postojeci counter a zatim se oduzme offset.
            // Npr. RTC sadrzi 00:00:30,178 i dogadja se pozitivni pomak za 50ms
            // RTC sada sadrzi 00:00:31,384
            // 256 (citava nova sekunda, PREDIV_S+1) + 178 (vrijednost
            // predhodne sekunde kada je zapocen time shift) - 50 (pozitivni
            // pomak) = 384.
            // Efektivno se ubrzava prijelaz u novu sekundu.
            add1s = RTC_SHIFTADD1S_SET;
            ofs = top - ofs;
        }

        result = (HAL_OK == HAL_RTCEx_SetSynchroShift(&hrtc, add1s, ofs));
    }

    return result;
}


uint8_t bsp_rtc_osc_test (void)
{
    static uint8_t b_ret = 1;
    static rtc_data_t rtc_val;

    uint32_t ms_cnt1;
    uint32_t ms_cnt2;

    bsp_rtc_data_get(&rtc_val);
    uint8_t seconds = rtc_val.seconds;

    /* Wait until seconds change. */
    ms_cnt1 = HAL_GetTick();
    do
    {
        ms_cnt2 = HAL_GetTick();
        bsp_rtc_data_get(&rtc_val);
    } while((rtc_val.seconds == seconds) &&
            ((ms_cnt2 - ms_cnt1) < RTC_TEST_GET_TIMEOUT));

    /* Check if seconds changed in RTC_TEST_GET_TIMEOUT ticks. */
    if (RTC_TEST_GET_TIMEOUT <= (ms_cnt2 - ms_cnt1))
    {
        b_ret = 1;
    }
    else
    {
        uint32_t tick1 = HAL_GetTick();
        seconds = rtc_val.seconds;

        /* When seconds changed start to count ticks till the next change. */
        ms_cnt1 = HAL_GetTick();
        do
        {
            ms_cnt2 = HAL_GetTick();
            bsp_rtc_data_get(&rtc_val);
        } while ((rtc_val.seconds == seconds) &&
                 ((ms_cnt2 - ms_cnt1) < RTC_TEST_GET_TIMEOUT));

        /* Check if seconds changed in RTC_TEST_GET_TIMEOUT ticks. */
        if (RTC_TEST_GET_TIMEOUT <= (ms_cnt2 - ms_cnt1))
        {
            b_ret = 1;
        }
        else
        {
            uint32_t tick2 = HAL_GetTick();

            dprintf("Delta ticks: %d\n", tick2 - tick1);

            /* Check if ticks for 1 second is between min and max. */
            if ((RTC_TEST_MIN_VALUE < (tick2 - tick1)) &&
                (RTC_TEST_MAX_VALUE > (tick2 - tick1)))
            {
                b_ret = 0;
            }

            else
            {
                b_ret = 1;
            }
        }
    }

    return b_ret;
}


static inline uint32_t rtc_current_tick_calc (void)
{
    uint32_t sec1;
    uint16_t subs1;
    uint16_t subs2;

    do {
        subs1 = (uint16_t)(hrtc.Instance->SSR);
        sec1 = rtc_sec_ticks;
        subs2 = (uint16_t)(hrtc.Instance->SSR);
    } while ((subs1 != subs2) || (sec1 != rtc_sec_ticks));

    uint64_t ticks = rtc_sec_ticks + (RTC_PREDIV_S - subs1);

    // Convert to miliseconds.
    ticks = (1000 * ticks) / (RTC_PREDIV_S +1);
    return (uint32_t)ticks;
}


void bsp_rtc_tick_reset (void)
{
    rtc_start_tick = rtc_current_tick_calc();
}


uint32_t bsp_rtc_tick_get (void)
{
    return (rtc_current_tick_calc() - rtc_start_tick);
}


void bsp_rtc_compability_mode (void)
{
    // In a case when function is called prior RTC init, enable RTC clock
    // and assign an instance to the handle.
    __HAL_RCC_RTC_ENABLE();
    hrtc.Instance = RTC;

    rtc_bypass_shadow(false);
}

//--------------------------- PRIVATE FUNCTIONS -------------------------------

static int32_t rtc_to_unix (const rtc_data_t *p_data)
{
    int32_t utc_tim;
    static struct tm time;
    memset(&time, 0, sizeof(time));

    time.tm_year = (2000 + p_data->year) - 1900;
    time.tm_mon = p_data->month-1;
    time.tm_mday = p_data->date;
    time.tm_hour = p_data->hours;
    time.tm_min = p_data->minutes;
    time.tm_sec = p_data->seconds;
    utc_tim = (int32_t)mktime(&time);

    return utc_tim;
}


static void unix_to_rtc (uint32_t unixtime, rtc_data_t *p_data)
{
    struct tm *time;

    time_t timestamp = (time_t)unixtime;
    time = gmtime(&timestamp);

    p_data->year = time->tm_year + 1900 - 2000;
    p_data->month = time->tm_mon+1;
    p_data->date = time->tm_mday;
    p_data->hours = time->tm_hour;
    p_data->minutes = time->tm_min;
    p_data->seconds = time->tm_sec;
}

static int rtc_data_get_exec (rtc_data_t *p_data)
{
    int return_value = 0;

    // Check if pointer is valid.
    if (NULL != p_data)
    {
#if 0
        __HAL_RTC_WRITEPROTECTION_DISABLE(&hrtc);
        HAL_RTC_WaitForSynchro(&hrtc);
        __HAL_RTC_WRITEPROTECTION_ENABLE(&hrtc);
#endif
        // Get RTC time.
        RTC_TimeTypeDef sTime;
        if (HAL_OK == HAL_RTC_GetTime(&hrtc, &sTime, RTC_FORMAT_BIN))
        {
            p_data->hours = sTime.Hours;
            p_data->minutes = sTime.Minutes;
            p_data->seconds = sTime.Seconds;
            uint32_t tmp = (1000 * (RTC_PREDIV_S - sTime.SubSeconds)) /
                (RTC_PREDIV_S +1);
            p_data->milisecs = (uint16_t)tmp;
        }

        else
        {
            return_value = 2;
            BL_RAISE_ERROR("RTC time get failed", EHALRTC);
            EPRINT("RTC time get failed\n");
        }

        // Get RTC Date.
        RTC_DateTypeDef sDate;
        if (HAL_OK == HAL_RTC_GetDate(&hrtc, &sDate, RTC_FORMAT_BIN))
        {
            p_data->year = sDate.Year;
            p_data->month = sDate.Month;
            p_data->date = sDate.Date;
        }
        else
        {
            return_value = 2;
            BL_RAISE_ERROR("RTC date get failed", EHALRTC);
            EPRINT("RTC date get failed\n");
        }

    }
    else
    {
        return_value = 1;
    }

    return return_value;
}


void rtc_bypass_shadow (bool enable)
{
    __HAL_RTC_WRITEPROTECTION_DISABLE(&hrtc);

    // Configure new state of bypass the shadow registers
    if (enable) {
        hrtc.Instance->CR |=  RTC_CR_BYPSHAD;
    } else {
        hrtc.Instance->CR &= ~RTC_CR_BYPSHAD;
    }

    __HAL_RTC_WRITEPROTECTION_ENABLE(&hrtc);
}


static void rtc_second_interrupt (bool enable)
{
    if (enable)
    {
        const RTC_AlarmTypeDef alarm = {
            .Alarm = RTC_ALARM_A,
            .AlarmMask = RTC_ALARMMASK_ALL,
            .AlarmSubSecondMask = RTC_ALARMSUBSECONDMASK_NONE,
            .AlarmDateWeekDay = RTC_WEEKDAY_MONDAY,
            .AlarmTime = {
                .SubSeconds = RTC_PREDIV_S,
            },
        };

        HAL_RTC_SetAlarm_IT(&hrtc, (RTC_AlarmTypeDef *)&alarm, RTC_FORMAT_BIN);
    }
    else
    {
        HAL_RTC_DeactivateAlarm(&hrtc, RTC_ALARM_A);
    }
}


// Suppress warnings, DO NOT CALL !
void bsp_rtc_dummy (void)
{
    (void)unix_to_rtc;
}

//--------------------------- INTERRUPT HANDLERS ------------------------------

void HAL_RTC_AlarmAEventCallback(RTC_HandleTypeDef *hrtc)
{
    rtc_sec_ticks = rtc_sec_ticks + (RTC_PREDIV_S + 1);
}

void RTC_Alarm_IRQHandler(void)
{
    HAL_RTC_AlarmIRQHandler(&hrtc);
}
