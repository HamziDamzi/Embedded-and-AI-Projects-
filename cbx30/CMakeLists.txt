cmake_minimum_required(VERSION 3.5)
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_BUILD_TYPE Default)   # prevent DEBUG/NDEBUG being passed to gcc

###
### NOTE: YOU NEED TO EDIT ALL THE LINES WITH A SINGLE "#"
###

### project name
set(CMAKE_C_COMPILER_WORKS 1)
set(CMAKE_CXX_COMPILER_WORKS 1)

project(CrossBox)

### Project firmware version
set(FW_VER "01-04-00")

### Compatible hardware
set(COMP_HW '{['3000-287'],['3000-313']}')

### System settings - not important for MCUs.
set(CMAKE_SYSTEM_NAME Generic)  # Linux/Windows/Darwin - compiler is forced by project, so this is set to generic.
set(CMAKE_SYSTEM_VERSION 1)     # target 'uname -r' output - meaningless for embedded

### Toolchain settings
set(CMAKE_C_COMPILER arm-none-eabi-gcc)
set(CMAKE_CXX_COMPILER arm-none-eabi-g++)
set(CMAKE_OBJCOPY arm-none-eabi-objcopy)

### Compiler settings,
### use: -Wno-maybe-uninitialized for -o3 optimisation.
set(COMMON_FLAGS "-Wall  -mcpu=cortex-m4 \
-mfloat-abi=hard \
-mfpu=fpv4-sp-d16 -lc -specs=nosys.specs -specs=nano.specs -mthumb \
-mthumb-interwork -ffunction-sections -fdata-sections -O0 -g -fno-common \
-fmessage-length=0 -u _scanf_float")

set(CMAKE_C_FLAGS "${COMMON_FLAGS} -std=gnu99")
set(CMAKE_CXX_FLAGS "${COMMON_FLAGS} -std=c++17 -fno-rtti")

### linker flags
set(LINKER_SCRIPT ${PROJECT_SOURCE_DIR}/bsp/mcu/STM32L476QGIx_FLASH.ld)
### linker includes, TODO: decide if we need this
set(CMAKE_EXE_LINKER_FLAGS "-Wl,--gc-sections -mthumb -mabi=aapcs -T ${LINKER_SCRIPT} \
    -L ${PROJECT_SOURCE_DIR}/bsp/")

### project defines passed to compiler
add_definitions(-DSTM32L476xG)
add_definitions(-DSTM32L476xx)
add_definitions(-DBURNTO=STM32L476RG)
add_definitions(-DARM_MATH_CM4)
add_definitions(-D__FPU_PRESENT)
add_definitions(-DUSE_HAL_DRIVER)
add_definitions(-DUSE_STM32L4XX_NUCLEO)
add_definitions(-D__FREERTOS)

#add_definitions(-DDBG_VERSION)
#add_definitions(-DSYSVIEW_EN)

# Definition for nrf serialization device.
add_definitions(-DNRF52832_XXAA)
add_definitions(-DNRF_SD_BLE_API_VERSION=6)
add_definitions(-DNRFX_UARTE_ENABLED=0)
add_definitions(-DNRFX_UART_ENABLED=1)
add_definitions(-DS132)
add_definitions(-DSVCALL_AS_NORMAL_FUNCTION)
add_definitions(-DBLE_STACK_SUPPORT_REQD)
add_definitions(-D_STM32)
add_definitions(-DBLE_NUS_ENABLED)
add_definitions(-DBLE_NUS)
add_definitions(-DBLE_DB_DISCOVERY_ENABLED=1)
add_definitions(-DNRF_BLE_SCAN_ENABLED=1)
add_definitions(-DNRF_BLE_SCAN_FILTER_ENABLE=1)
add_definitions(-DNRF_BLE_SCAN_BUFFER=255)
add_definitions(-DFW_VER_DEF="${FW_VER}")
add_definitions(-DHW_VER="${COMP_HW}")
add_definitions(-DUSE_ACC_VECTOR=1)

set_property(SOURCE bsp/mcu/startup_stm32l476xx.s PROPERTY LANGUAGE C)
set(SYSVIEW_EN 0)

#set_source_files_properties(lib/printf/printf.c PROPERTIES COMPILE_FLAGS -O3)

set(SOURCE_FILES
    src/tasks/ble_ser_task.c
    bsp/mcu/startup_stm32l476xx.s
    lib/HAL/system_stm32l4xx.c
    lib/HAL/STM32L4xx_HAL_Driver/Src/stm32l4xx_hal.c
    lib/HAL/STM32L4xx_HAL_Driver/Src/stm32l4xx_hal_cortex.c
    lib/HAL/STM32L4xx_HAL_Driver/Src/stm32l4xx_hal_dma.c
    lib/HAL/STM32L4xx_HAL_Driver/Src/stm32l4xx_hal_dma_ex.c
    lib/HAL/STM32L4xx_HAL_Driver/Src/stm32l4xx_hal_flash.c
    lib/HAL/STM32L4xx_HAL_Driver/Src/stm32l4xx_hal_flash_ex.c
    lib/HAL/STM32L4xx_HAL_Driver/Src/stm32l4xx_hal_gpio.c
    lib/HAL/STM32L4xx_HAL_Driver/Src/stm32l4xx_hal_iwdg.c
    lib/HAL/STM32L4xx_HAL_Driver/Src/stm32l4xx_hal_pcd.c
    lib/HAL/STM32L4xx_HAL_Driver/Src/stm32l4xx_hal_pcd_ex.c
    lib/HAL/STM32L4xx_HAL_Driver/Src/stm32l4xx_hal_pwr.c
    lib/HAL/STM32L4xx_HAL_Driver/Src/stm32l4xx_hal_pwr_ex.c
    lib/HAL/STM32L4xx_HAL_Driver/Src/stm32l4xx_hal_rcc.c
    lib/HAL/STM32L4xx_HAL_Driver/Src/stm32l4xx_hal_rcc_ex.c
    lib/HAL/STM32L4xx_HAL_Driver/Src/stm32l4xx_hal_uart.c
    lib/HAL/STM32L4xx_HAL_Driver/Src/stm32l4xx_hal_uart_ex.c
    lib/HAL/STM32L4xx_HAL_Driver/Src/stm32l4xx_hal_usart.c
    lib/HAL/STM32L4xx_HAL_Driver/Src/stm32l4xx_hal_usart_ex.c
    lib/HAL/STM32L4xx_HAL_Driver/Src/stm32l4xx_hal_tim.c
    lib/HAL/STM32L4xx_HAL_Driver/Src/stm32l4xx_hal_tim_ex.c
    lib/HAL/STM32L4xx_HAL_Driver/Src/stm32l4xx_hal_spi.c
    lib/HAL/STM32L4xx_HAL_Driver/Src/stm32l4xx_hal_spi_ex.c
    lib/HAL/STM32L4xx_HAL_Driver/Src/stm32l4xx_hal_i2c.c
    lib/HAL/STM32L4xx_HAL_Driver/Src/stm32l4xx_hal_i2c_ex.c
    lib/HAL/STM32L4xx_HAL_Driver/Src/stm32l4xx_hal_emmc.c
    lib/HAL/STM32L4xx_HAL_Driver/Src/stm32l4xx_hal_emmc_ex.c
    lib/HAL/STM32L4xx_HAL_Driver/Src/stm32l4xx_ll_sdmmc.c
    lib/HAL/STM32L4xx_HAL_Driver/Src/stm32l4xx_hal_rtc.c
    lib/HAL/STM32L4xx_HAL_Driver/Src/stm32l4xx_hal_rtc_ex.c
    lib/HAL/STM32L4xx_HAL_Driver/Src/stm32l4xx_hal_adc.c
    lib/HAL/STM32L4xx_HAL_Driver/Src/stm32l4xx_hal_adc_ex.c

    lib/FreeRTOS/CMSIS_RTOS/cmsis_os.c
    lib/FreeRTOS/portable/GCC/ARM_CM4F/port.c
    lib/FreeRTOS/portable/MemMang/heap_4.c
    lib/FreeRTOS/croutine.c
    lib/FreeRTOS/event_groups.c
    lib/FreeRTOS/list.c
    lib/FreeRTOS/queue.c
    lib/FreeRTOS/tasks.c
    lib/FreeRTOS/timers.c

    lib/blibI2C/bli2c-gpio.c
    lib/blibI2C/bli2c-gpio.h
    lib/blibI2C/bli2c-master.h
    lib/blibI2C/bli2c-mux.c
    lib/blibI2C/bli2c-mux.h
    lib/blibI2C/bli2c.c
    lib/blibI2C/bli2c.h

    lib/IMU/lsm6dsl/LSM6DSL_ACC_GYRO_driver.c
    lib/IMU/lsm6dsl/LSM6DSL_ACC_GYRO_driver_HL.c
    lib/IMU/lsm303agr/LSM303AGR_ACC_driver.c
    lib/IMU/lsm303agr/LSM303AGR_ACC_driver_HL.c
    lib/IMU/lsm303agr/LSM303AGR_MAG_driver.c
    lib/IMU/lsm303agr/LSM303AGR_MAG_driver_HL.c
    lib/IMU/X_NUCLEO_IKS01A2/x_nucleo_iks01a2.c
    lib/IMU/X_NUCLEO_IKS01A2/x_nucleo_iks01a2_accelero.c
    lib/IMU/X_NUCLEO_IKS01A2/x_nucleo_iks01a2_gyro.c
    lib/IMU/X_NUCLEO_IKS01A2/x_nucleo_iks01a2_magneto.c
    lib/IMU/imu.c

    lib/error_handler/blerrno.c
    lib/error_handler/error_handler.c
    lib/rtttl/rtttl.c

    lib/fatfs_ff13a/source/ff.c
    lib/fatfs_ff13a/source/ffsystem.c
    lib/fatfs_ff13a/source/ffunicode.c

    lib/emmc/emmc.c

    lib/zoe-m8/ubx/ubx.c
    lib/zoe-m8/nmea/nmea_parser.c
    lib/zoe-m8/zoe_m8/zoe_m8.c
    lib/zoe-m8/zoe_parser.c

    lib/error_log/errlog.c

    lib/TPS65721/tps65721.c

    lib/ws2812/ws2812.c
    lib/ws2812/ws2812_port_stm32l4.c

    lib/nanopb/pb_encode.c
    lib/nanopb/pb_decode.c
    lib/nanopb/pb_common.c

    lib/blibButton/blbutton.c
    lib/blibButton/blbutton-key.c
    lib/blibButton/blbutton-algo-debounce.c
    lib/blibButton/blbutton-algo-click.c
    lib/blibButton/blbutton-algo-dblclick.c

    lib/blibDelay/bldelay-freertos-hal.c
    lib/blibDelay/bldelay-loop.c

    lib/blibESPAT/esp8266.c

    lib/blibAT/atcmd.c
    lib/blibAT/atcmd_param_parser.c

    lib/blsocket/blsocket.c

    lib/blibFSM/blibFSM.cpp
    lib/blibFSM/blibFSM-Debug.cpp
    lib/blibFSM/blibFSM-ControlDataBase.cpp

    lib/blibGPIO/blgpio-stm32-hal.c
    lib/blibGPIO/blgpio-stm32-hal-auto.c
    lib/blibGPIO/blgpio-stm32-hal-pinsel-algo.c
    lib/blibGPIO/blgpio.c

    lib/blibOSWrap/bloswrap-freertos.c

    lib/blibUart/bluart.c
    lib/blibUart/bluart-buf.c
    lib/blibUart/bluart-stm32-hal.c
    lib/blibUart/bluart-stm32-hal-auto.c

    lib/blibspi/blspi-gpio.c
    lib/blibspi/blspi-stm32-hal.c
    lib/blibspi/blspi.c
    lib/blibspi/blspi-stm32-hal-auto.c
    lib/blibspi/blspi-suspend-gpio.c

    lib/wdtm/wdtm.c
    lib/wdtm/wdtm_freertos.c
    lib/wdtm/wdt_stm32l4.c

    lib/blog/blog.c
    lib/blog/blog_example.c
    lib/blog/blog_ticks_freertos.c

    bsp/src/bsp.c
    bsp/src/i2c.c
    bsp/src/rtc.c
    bsp/src/gps.c
    bsp/src/buzzer.c
    bsp/src/button.c
    bsp/src/imu.c
    bsp/mcu/burnto.c
    bsp/src/ble.c
    bsp/src/adc.c
    bsp/src/wifi.c
    bsp/src/dma.c

    src/stm32l4xx_hal_msp.c
    src/stm32l4xx_it.c
    src/fatfs_diskio.c
    src/protobuf/crossbox.pb.c
    src/protobuf/pbft.pb.c
    src/protobuf/proto_helper.c
    src/bldbg.c
    src/crossbox_fsm.cpp
    src/crossboxFSMAuto.cpp
    src/helpers.c
    src/notification.c

    # NRF serialization
    lib/nrf/components/serialization/common/struct_ser/ble/ble_gap_struct_serialization.c
    lib/nrf/components/serialization/common/struct_ser/ble/ble_gatt_struct_serialization.c
    lib/nrf/components/serialization/common/struct_ser/ble/ble_gattc_struct_serialization.c
    lib/nrf/components/serialization/common/struct_ser/ble/ble_gatts_struct_serialization.c
    lib/nrf/components/serialization/common/struct_ser/ble/ble_l2cap_struct_serialization.c
    lib/nrf/components/serialization/common/struct_ser/ble/ble_struct_serialization.c

    lib/nrf/components/serialization/common/ble_serialization.c
    lib/nrf/components/serialization/common/cond_field_serialization.c

    lib/nrf/components/serialization/application/codecs/ble/serializers/app_ble_user_mem.c
    lib/nrf/components/serialization/application/codecs/ble/serializers/ble_app.c
    lib/nrf/components/serialization/application/codecs/ble/serializers/ble_event.c
    lib/nrf/components/serialization/application/codecs/ble/serializers/ble_evt_app.c
    lib/nrf/components/serialization/application/codecs/ble/serializers/ble_gap_app.c
    lib/nrf/components/serialization/application/codecs/ble/serializers/ble_gap_evt_app.c
    lib/nrf/components/serialization/application/codecs/ble/serializers/ble_gattc_app.c
    lib/nrf/components/serialization/application/codecs/ble/serializers/ble_gattc_evt_app.c
    lib/nrf/components/serialization/application/codecs/ble/serializers/ble_gatts_app.c
    lib/nrf/components/serialization/application/codecs/ble/serializers/ble_gatts_evt_app.c
    lib/nrf/components/serialization/application/codecs/ble/serializers/ble_l2cap_app.c
    lib/nrf/components/serialization/application/codecs/ble/serializers/ble_l2cap_evt_app.c

    lib/nrf/components/serialization/application/codecs/ble/serializers/app_ble_gap_sec_keys.c

    lib/nrf/components/serialization/application/codecs/ble/middleware/app_mw_ble.c
    lib/nrf/components/serialization/application/codecs/ble/middleware/app_mw_ble_gap.c
    lib/nrf/components/serialization/application/codecs/ble/middleware/app_mw_ble_gatts.c
    lib/nrf/components/serialization/application/codecs/ble/middleware/app_mw_ble_gattc.c
    lib/nrf/components/serialization/application/codecs/ble/middleware/app_mw_ble_l2cap.c
    lib/nrf/components/serialization/application/codecs/ble/middleware/app_mw_nrf_soc.c

    lib/nrf/components/serialization/application/transport/ser_sd_transport.c
    lib/nrf/components/serialization/common/transport/ser_hal_transport.c
    lib/nrf/components/serialization/common/transport/ser_phy/ser_phy.c
    lib/nrf/components/ble/nrf_ble_gatt/nrf_ble_gatt.c

    lib/nrf/components/serialization/application/transport/ser_softdevice_handler.c
    lib/nrf/components/libraries/queue/nrf_queue.c
    lib/nrf/components/libraries/util/app_util_platform.c
    lib/nrf/components/libraries/scheduler/app_scheduler.c
    lib/nrf/components/ble/ble_services/ble_hrs/ble_hrs.c
    lib/nrf/components/ble/common/ble_srv_common.c
    lib/nrf/components/ble/common/ble_conn_params.c

    lib/nrf/components/libraries/timer/app_timer_freertos.c

    lib/nrf/components/serialization/application/hal/ser_app_hal_nrf5x.c
    lib/nrf/sdk_config.h


    lib/nrf/components/softdevice/common/nrf_sdh_freertos.c
    lib/nrf/components/softdevice/common/nrf_sdh.c
    lib/nrf/components/softdevice/common/nrf_sdh_ble.c
    lib/nrf/components/libraries/experimental_section_vars/nrf_section_iter.c
    lib/nrf/components/ble/ble_services/ble_nus/ble_nus.c
    lib/nrf/components/ble/ble_link_ctx_manager/ble_link_ctx_manager.c
    lib/nrf/components/ble/ble_advertising/ble_advertising.c
    lib/nrf/components/libraries/atomic_flags/nrf_atflags.c
    lib/nrf/components/libraries/atomic/nrf_atomic.c
    lib/nrf/components/ble/common/ble_conn_state.c
    lib/nrf/components/ble/common/ble_advdata.c

    lib/nrf/components/serialization/application/codecs/common/conn_systemreset.c
    lib/nrf/components/ble/nrf_ble_qwr/nrf_ble_qwr.c

    lib/nrf/components/libraries/util/app_error.c
    lib/nrf/components/libraries/strerror/nrf_strerror.c
    lib/nrf/components/ble/ble_db_discovery/ble_db_discovery.c
    lib/nrf/components/ble/ble_services/ble_hrs_c/ble_hrs_c.c
    lib/nrf/components/ble/ble_services/ble_hrs/ble_hrs.c
    lib/nrf/components/ble/nrf_ble_scan/nrf_ble_scan.c

    lib/pbft/pbft.c
    lib/pbft/pbft_fs.c

    lib/blog/blog.c
    lib/blog/blog_ticks_freertos.c

    src/ble_service.c
    src/ble_ser_phy_uart_stm32l4.c

    src/bloader_dfu.c

    src/tmr/bms_tmr.cpp
    src/tmr/led_task.c
    src/tmr/lp_tmr.c

    src/tasks/ui_task.c
    src/tasks/imu_task.c
    src/tasks/logger_task.cpp
    src/tasks/gnss_task.c
    src/tasks/emmc_task.c
    src/tasks/ble_task.c
    src/tasks/wifi_task.c
    src/tasks/dma_task.c
    src/tasks/proto_task.c
    src/emmc_helper.c
    src/led_gestures.c
    src/fsm_helper.cpp
    src/hrm.c
    src/circular_buffer.c
    )

if (SYSVIEW_EN)
    set(SOURCE_FILES
            ${SOURCE_FILES}
            lib/sysview/Config/SEGGER_SYSVIEW_Config_FreeRTOS.c
            lib/sysview/SEGGER_SYSVIEW_FreeRTOS.c
            lib/sysview/SEGGER/SEGGER_RTT.c
            lib/sysview/SEGGER/SEGGER_SYSVIEW.c
            lib/sysview/SEGGER_RTT_printf.c)

    include_directories(lib/sysview)
    include_directories(lib/sysview/Config)
    include_directories(lib/sysview/SEGGER)
else (SYSVIEW_EN)
    set(SOURCE_FILES
            ${SOURCE_FILES}
            lib/rtt/RTT.h
            lib/rtt/rtt_freertos.c
            lib/rtt/SEGGER_RTT.c
            lib/rtt/SEGGER_RTT.h
            lib/rtt/SEGGER_RTT_printf.c)

    include_directories(lib/rtt)
endif (SYSVIEW_EN)

include_directories(lib/nrf/components/libraries/timer)
include_directories(lib/nrf/components/ble/common)
include_directories(lib/nrf/components/ble/ble_services/ble_hrs)
include_directories(lib/nrf/components/ble/nrf_ble_gatt/)
include_directories(lib/nrf/components/serialization/common/struct_ser/ble)
include_directories(lib/nrf/components/serialization/common)
include_directories(lib/nrf/components/libraries/util)
include_directories(lib/nrf/components/serialization/application/codecs/ble/serializers)
include_directories(lib/nrf/components/serialization/application/codecs/common)

include_directories(lib/nrf/components/serialization/common/transport)
include_directories(lib/nrf/components/serialization/application/transport)
include_directories(lib/nrf/components/serialization/common/transport/ser_phy)
include_directories(lib/nrf/components/softdevice/s132)
include_directories(lib/nrf/components/softdevice/s132/headers)
include_directories(lib/nrf/modules/nrfx/mdk)
include_directories(lib/nrf/components/libraries/experimental_section_vars)
include_directories(lib/nrf)
include_directories(lib/nrf/components/softdevice/common)
include_directories(lib/nrf/components/serialization/common/transport)
include_directories(lib/nrf/components/libraries/queue/)
include_directories(lib/nrf/components/libraries/scheduler/)
include_directories(lib/nrf/components/serialization/application/hal)
include_directories(lib/nrf/components/libraries/log)
include_directories(lib/nrf/components/libraries/util)
include_directories(lib/nrf/components/ble/ble_db_discovery)
include_directories(lib/nrf/components/ble/ble_services/ble_hrs_c)
include_directories(lib/nrf/components/ble/ble_services/ble_hrs)
include_directories(lib/nrf/components/ble/nrf_ble_scan)

include_directories(inc)
include_directories(inc/config)
include_directories(bsp)
include_directories(bsp/inc)
include_directories(bsp/src)
include_directories(bsp/mcu)
include_directories(lib/blibButton)
include_directories(lib/IMU)
include_directories(lib/IMU/X_NUCLEO_IKS01A2)
include_directories(lib/IMU/lsm303agr)
include_directories(lib/IMU/lsm6dsl)
include_directories(lib/IMU/Common)
include_directories(lib/HAL/CMSIS/Device/ST/STM32L4xx/Include)
include_directories(lib/HAL/CMSIS/Include)
include_directories(lib/HAL/STM32L4xx_HAL_Driver/Inc)
include_directories(lib/HAL/STM32L4xx_HAL_Driver/Inc/Legacy)
include_directories(lib/FreeRTOS/include)
include_directories(lib/FreeRTOS/CMSIS_RTOS)
include_directories(lib/FreeRTOS/portable/GCC/ARM_CM4F)
include_directories(lib/blibGPIO)
include_directories(lib/blibI2C)
include_directories(lib/blibDelay)
include_directories(lib/blibESPAT)
include_directories(lib/blibAT)
include_directories(lib/blibAT/include)
include_directories(lib/blibAT/include/atcmd)
include_directories(lib/blsocket)
include_directories(lib/blibFSM)
include_directories(lib/blibFSM/inc)
include_directories(lib/blibFSM/inc/blib)
include_directories(lib/blibFSM/inc/blib/FSM)
include_directories(lib/blibpp)
include_directories(lib/blibpp/inc)
include_directories(lib/blibpp/std-inc)
include_directories(lib/blibpp/inc/blib)
include_directories(lib/blibpp/inc/blib/bits)
include_directories(lib/blibpp/inc/blib/bits/FreeRTOS)
include_directories(lib/blibpp/inc/blib/bits/stdcpp)
include_directories(lib/blibpp/inc/FreeRTOS)
include_directories(lib/blibpp/inc/FreeRTOS/bits)
include_directories(lib/blibOSWrap)
include_directories(lib/blibUart)
include_directories(lib/fatfs_ff13a/source)
include_directories(lib/emmc)
include_directories(lib/wdtm)
include_directories(lib/zoe-m8/ubx)
include_directories(lib/zoe-m8/nmea)
include_directories(lib/zoe-m8/zoe_m8)
include_directories(lib/zoe-m8)
include_directories(lib/error_handler)
include_directories(lib/error_log)
include_directories(lib/TPS65721)
include_directories(src/tasks)
include_directories(src/tmr)
include_directories(lib/nanopb)
include_directories(src/protobuf)
include_directories(lib/rtttl)
include_directories(lib/blog)
include_directories(lib/blibspi)
include_directories(lib/ws2812)
include_directories(lib/blog)
include_directories(lib/fts)
include_directories(lib/BLOADER/bootloader)
#include_directories(lib/fts)
include_directories(lib/pbft)


include_directories(lib/nrf/components/ble/ble_services/ble_nus)
include_directories(lib/nrf/components/ble/ble_link_ctx_manager)
include_directories(lib/nrf/components/ble/ble_advertising)
include_directories(lib/nrf/components/ble/common)
include_directories(lib/nrf/components/libraries/atomic_flags)
include_directories(lib/nrf/components/libraries/atomic)
include_directories(lib/nrf/components/ble/nrf_ble_qwr)
# Nrf serializer includes

########################################################
### project targets/executables
########################################################
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY build)

### List of all targets, default are release and debug
set(TARGET_LIST release debug debug_no_bloader test_fw)


add_custom_target(deploy-dbg-no-bloader
                  COMMAND bljflash ${PROJECT_SOURCE_DIR}/build/debug_no_bloader/debug_no_bloader.hex)

add_custom_target(deploy-debug
                  COMMAND bash ${PROJECT_SOURCE_DIR}/tools/fuse.sh debug
                  COMMAND bljflash ${PROJECT_SOURCE_DIR}/build/debug/fuse/debug.hex)

#add_custom_target(deploy-release
#                  COMMAND bash ${PROJECT_SOURCE_DIR}/tools/fuse.sh release
#                  COMMAND bljflash ${PROJECT_SOURCE_DIR}/build/release/fuse/release.hex)

foreach(targ ${TARGET_LIST})
    add_executable(${targ} ${SOURCE_FILES} src/main.c)

    ### set output/build directory for targets
    set_target_properties(${targ} PROPERTIES RUNTIME_OUTPUT_DIRECTORY ${PROJECT_SOURCE_DIR}/build/${targ})
    set_target_properties(${targ} PROPERTIES OUTPUT_NAME ${targ}.elf)

    ### add .ld file to make dependencies
    set_target_properties(${targ} PROPERTIES LINK_DEPENDS ${LINKER_SCRIPT})

    # TODO: creates build dir as expected but also creates garbage in project root
    # file(MAKE_DIRECTORY $<TARGET_FILE_DIR:${targ}>/)

    add_custom_command(
            TARGET ${targ} PRE_BUILD
            COMMAND ${CMAKE_COMMAND} -E make_directory $<TARGET_FILE_DIR:${targ}>/
    )

    ### create .bin & .hex for targets
    set(HEX_FILE_${targ} $<TARGET_FILE_DIR:${targ}>/${targ}.hex)
    set(BIN_FILE_${targ} $<TARGET_FILE_DIR:${targ}>/${targ}.bin)
    add_custom_command(
            TARGET ${targ} POST_BUILD
            COMMAND ${CMAKE_OBJCOPY} -Oihex $<TARGET_FILE:${targ}> ${HEX_FILE_${targ}}
            COMMAND ${CMAKE_OBJCOPY} -Obinary $<TARGET_FILE:${targ}> ${BIN_FILE_${targ}}
            COMMAND arm-none-eabi-size $<TARGET_FILE:${targ}>
    )
endforeach()

target_compile_definitions(debug PRIVATE "-DDEBUG")
target_compile_definitions(debug PRIVATE "-DBLOADER_IMG")
target_link_libraries(debug PRIVATE "-Xlinker --defsym=__bloader__=1")

target_compile_definitions(release PRIVATE "-DDEBUG")
target_compile_definitions(release PRIVATE "-DBLOADER_IMG")
target_link_libraries(release PRIVATE "-Xlinker --defsym=__bloader__=1")

target_compile_definitions(debug_no_bloader PRIVATE "-DDEBUG")

target_compile_definitions(test_fw PRIVATE "-DDEBUG")
target_compile_definitions(test_fw PRIVATE "-DTESTFW")

# Post-build debug
# 1 - Build Bootloader
# 2 - make DFU .bin
# 3 - make fuse .hex
# 4 - copy debug files (fuse hex, DFU bin, app_only) to output folder `output/debug`
add_custom_command(
        TARGET debug POST_BUILD
        COMMAND ${PROJECT_SOURCE_DIR}/tools/metagen/metagen.py --project-dir ${PROJECT_SOURCE_DIR} --target debug
)

# Post-build debug-no-bootlaoder
# 1 - copy debug-no-bootloader hex
add_custom_command(
        TARGET debug_no_bloader POST_BUILD
        COMMAND ${PROJECT_SOURCE_DIR}/tools/metagen/metagen.py --project-dir ${PROJECT_SOURCE_DIR} --target debug_no_bloader
)

# Post-build release
# 1 - Build Bootloader
# 2 - make DFU .bin
# 3 - make fuse .hex
# 4 - Copy hex app
# 5 - Copy hex bootlaoder
# 6 - Rename files according to fw naming scheme
# 7 - Generate artifacts
# 8 - Generate metafiles
add_custom_command(
        TARGET release POST_BUILD
        COMMAND ${PROJECT_SOURCE_DIR}/tools/metagen/metagen.py --version ${FW_VER} --comp-hw ${COMP_HW} --project-dir ${PROJECT_SOURCE_DIR} --target release
)

