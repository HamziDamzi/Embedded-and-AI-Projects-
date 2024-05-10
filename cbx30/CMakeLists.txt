cmake_minimum_required(VERSION 3.8)
set(CMAKE_CXX_STANDARD 11)
set(CMAKE_BUILD_TYPE Default)   # prevent DEBUG/NDEBUG being passed to gcc

###
### NOTE: YOU NEED TO EDIT ALL THE LINES WITH A SINGLE "#"
###

### project name
project(CROSSBOX_BOOTLOADER C)

### System settings - not important for MCUs.
set(CMAKE_SYSTEM_NAME Generic)  # Linux/Windows/Darwin - compiler is forced by project, so this is set to generic.
set(CMAKE_SYSTEM_VERSION 1)     # target 'uname -r' output - meaningless for embedded

### Toolchain settings
set(CMAKE_C_COMPILER arm-none-eabi-gcc)
set(CMAKE_CXX_COMPILER arm-none-eabi-g++)
set(CMAKE_OBJCOPY arm-none-eabi-objcopy)

### Compiler settings
set(COMMON_FLAGS "-Wall -Werror -mcpu=cortex-m4 -mfloat-abi=hard -mfpu=fpv4-sp-d16 -lc -specs=nosys.specs -specs=nano.specs -mthumb -mthumb-interwork -ffunction-sections -fdata-sections -Og -g -fno-common -fmessage-length=0")
set(CMAKE_C_FLAGS "${COMMON_FLAGS} -std=gnu99")
set(CMAKE_CXX_FLAGS "${COMMON_FLAGS} -std=c++11")

### linker flags
set(LINKER_SCRIPT ${PROJECT_SOURCE_DIR}/bsp/mcu/STM32L476RGTx_FLASH.ld)
set(CMAKE_EXE_LINKER_FLAGS "-Wl,-gc-sections -T ${LINKER_SCRIPT} -L ${PROJECT_SOURCE_DIR}/bsp/")

### project defines passed to compiler
add_definitions(-DSTM32L476xG)
add_definitions(-DSTM32L476xx)
add_definitions(-DBURNTO=STM32L476RG)
add_definitions(-DUSE_HAL_DRIVER)
add_definitions(-DUSE_FULL_ASSERT)
add_definitions(-D__BLNOOS)

### source and includes - you should probably generate this via CLion GUI
set_property(SOURCE bsp/mcu/startup_stm32l476xx.s PROPERTY LANGUAGE C)
set(SOURCE_FILES

    lib/blibGPIO/blgpio-stm32-hal-auto.c
    lib/blibGPIO/blgpio-stm32-hal-auto.h
    lib/blibGPIO/blgpio-stm32-hal-pinsel-algo.c
    lib/blibGPIO/blgpio-stm32-hal.c
    lib/blibGPIO/blgpio-stm32-hal.h
    lib/blibGPIO/blgpio.h

    lib/blibOSWrap/blcc_cross.h
    lib/blibOSWrap/blmmgr-core.h
    lib/blibOSWrap/blmmgr-heap1.c
    lib/blibOSWrap/blmmgr.c
    lib/blibOSWrap/blmmgr.h
    lib/blibOSWrap/bloswrap-cpu.h
    lib/blibOSWrap/bloswrap-delay_loop.c
    lib/blibOSWrap/bloswrap-delay_loop.h
    lib/blibOSWrap/bloswrap-inflate.h
    lib/blibOSWrap/bloswrap-interrupts.h
    lib/blibOSWrap/bloswrap-time.h
    lib/blibOSWrap/bloswrap.h

    lib/BLOADER/bl_md5/bl_md5.c
    lib/BLOADER/bl_md5/bl_md5.h
    lib/BLOADER/bootloader/bl_core-stm32l4.c
    lib/BLOADER/bootloader/bl_core-stm32l4.h
    lib/BLOADER/bootloader/bl_core.c
    lib/BLOADER/bootloader/bl_core.h
    lib/BLOADER/bootloader/blcc_cross.h
    lib/BLOADER/bootloader/bloader.h
    inc/config/bloader_conf.h

    lib/emmc/emmc.c
    lib/emmc/emmc.h

    lib/RTT/RTT.h
    lib/RTT/SEGGER_RTT.c
    lib/RTT/SEGGER_RTT.h
    lib/RTT/SEGGER_RTT_printf.c

    lib/error_handler/blerrno.c
    lib/error_handler/blerrno.h
    lib/error_handler/error_handler.c
    lib/error_handler/error_handler.h

    lib/STM32Cube/Inc/Legacy/stm32_hal_legacy.h
    lib/STM32Cube/Inc/stm32l4xx_hal.h
    lib/STM32Cube/Inc/stm32l4xx_hal_adc.h
    lib/STM32Cube/Inc/stm32l4xx_hal_adc_ex.h
    lib/STM32Cube/Inc/stm32l4xx_hal_can.h
    lib/STM32Cube/Inc/stm32l4xx_hal_comp.h
    lib/STM32Cube/Inc/stm32l4xx_hal_cortex.h
    lib/STM32Cube/Inc/stm32l4xx_hal_flash.h
    lib/STM32Cube/Inc/stm32l4xx_hal_flash_ex.h
    lib/STM32Cube/Inc/stm32l4xx_hal_gpio.h
    lib/STM32Cube/Inc/stm32l4xx_hal_gpio_ex.h
    lib/STM32Cube/Inc/stm32l4xx_hal_nand.h
    lib/STM32Cube/Inc/stm32l4xx_hal_nor.h
    lib/STM32Cube/Inc/stm32l4xx_hal_pwr.h
    lib/STM32Cube/Inc/stm32l4xx_hal_pwr_ex.h
    lib/STM32Cube/Inc/stm32l4xx_hal_rcc.h
    lib/STM32Cube/Inc/stm32l4xx_hal_rcc_ex.h
    lib/STM32Cube/Inc/stm32l4xx_hal_spi.h
    lib/STM32Cube/Inc/stm32l4xx_hal_spi_ex.h
    lib/STM32Cube/Inc/stm32l4xx_hal_sram.h
    lib/STM32Cube/Inc/stm32l4xx_hal_swpmi.h
    lib/STM32Cube/Inc/stm32l4xx_ll_cortex.h
    lib/STM32Cube/Inc/stm32l4xx_ll_crc.h
    lib/STM32Cube/Inc/stm32l4xx_ll_crs.h
    lib/STM32Cube/Inc/stm32l4xx_ll_dac.h
    lib/STM32Cube/Inc/stm32l4xx_ll_exti.h
    lib/STM32Cube/Inc/stm32l4xx_ll_fmc.h
    lib/STM32Cube/Inc/stm32l4xx_ll_gpio.h
    lib/STM32Cube/Inc/stm32l4xx_ll_i2c.h
    lib/STM32Cube/Inc/stm32l4xx_ll_iwdg.h
    lib/STM32Cube/Inc/stm32l4xx_ll_lptim.h
    lib/STM32Cube/Inc/stm32l4xx_ll_lpuart.h
    lib/STM32Cube/Inc/stm32l4xx_ll_opamp.h
    lib/STM32Cube/Inc/stm32l4xx_ll_pwr.h
    lib/STM32Cube/Inc/stm32l4xx_ll_rcc.h
    lib/STM32Cube/Inc/stm32l4xx_ll_rng.h
    lib/STM32Cube/Inc/stm32l4xx_ll_rtc.h
    lib/STM32Cube/Inc/stm32l4xx_ll_sdmmc.h
    lib/STM32Cube/Inc/stm32l4xx_ll_spi.h
    lib/STM32Cube/Inc/stm32l4xx_ll_swpmi.h
    lib/STM32Cube/Inc/stm32l4xx_ll_system.h
    lib/STM32Cube/Inc/stm32l4xx_ll_tim.h
    lib/STM32Cube/Inc/stm32l4xx_ll_usart.h
    lib/STM32Cube/Inc/stm32l4xx_ll_usb.h
    lib/STM32Cube/Inc/stm32l4xx_ll_utils.h
    lib/STM32Cube/Inc/stm32l4xx_ll_wwdg.h
    lib/STM32Cube/Src/stm32l4xx_hal.c
    lib/STM32Cube/Src/stm32l4xx_hal_adc.c
    lib/STM32Cube/Src/stm32l4xx_hal_adc_ex.c
    lib/STM32Cube/Src/stm32l4xx_hal_can.c
    lib/STM32Cube/Src/stm32l4xx_hal_comp.c
    lib/STM32Cube/Src/stm32l4xx_hal_cortex.c
    lib/STM32Cube/Src/stm32l4xx_hal_crc.c
    lib/STM32Cube/Src/stm32l4xx_hal_crc_ex.c
    lib/STM32Cube/Src/stm32l4xx_hal_cryp.c
    lib/STM32Cube/Src/stm32l4xx_hal_cryp_ex.c
    lib/STM32Cube/Src/stm32l4xx_hal_dac.c
    lib/STM32Cube/Src/stm32l4xx_hal_dac_ex.c
    lib/STM32Cube/Src/stm32l4xx_hal_dcmi.c
    lib/STM32Cube/Src/stm32l4xx_hal_dfsdm.c
    lib/STM32Cube/Src/stm32l4xx_hal_dfsdm_ex.c
    lib/STM32Cube/Src/stm32l4xx_hal_dma.c
    lib/STM32Cube/Src/stm32l4xx_hal_dma2d.c
    lib/STM32Cube/Src/stm32l4xx_hal_dma_ex.c
    lib/STM32Cube/Src/stm32l4xx_hal_dsi.c
    lib/STM32Cube/Src/stm32l4xx_hal_firewall.c
    lib/STM32Cube/Src/stm32l4xx_hal_flash.c
    lib/STM32Cube/Src/stm32l4xx_hal_flash_ex.c
    lib/STM32Cube/Src/stm32l4xx_hal_flash_ramfunc.c
    lib/STM32Cube/Src/stm32l4xx_hal_gfxmmu.c
    lib/STM32Cube/Src/stm32l4xx_hal_gpio.c
    lib/STM32Cube/Src/stm32l4xx_hal_hash.c
    lib/STM32Cube/Src/stm32l4xx_hal_hash_ex.c
    lib/STM32Cube/Src/stm32l4xx_hal_hcd.c
    lib/STM32Cube/Src/stm32l4xx_hal_i2c.c
    lib/STM32Cube/Src/stm32l4xx_hal_i2c_ex.c
    lib/STM32Cube/Src/stm32l4xx_hal_irda.c
    lib/STM32Cube/Src/stm32l4xx_hal_iwdg.c
    lib/STM32Cube/Src/stm32l4xx_hal_lcd.c
    lib/STM32Cube/Src/stm32l4xx_hal_lptim.c
    lib/STM32Cube/Src/stm32l4xx_hal_ltdc.c
    lib/STM32Cube/Src/stm32l4xx_hal_ltdc_ex.c
    lib/STM32Cube/Src/stm32l4xx_hal_nand.c
    lib/STM32Cube/Src/stm32l4xx_hal_nor.c
    lib/STM32Cube/Src/stm32l4xx_hal_opamp.c
    lib/STM32Cube/Src/stm32l4xx_hal_opamp_ex.c
    lib/STM32Cube/Src/stm32l4xx_hal_ospi.c
    lib/STM32Cube/Src/stm32l4xx_hal_pcd.c
    lib/STM32Cube/Src/stm32l4xx_hal_pcd_ex.c
    lib/STM32Cube/Src/stm32l4xx_hal_pwr.c
    lib/STM32Cube/Src/stm32l4xx_hal_pwr_ex.c
    lib/STM32Cube/Src/stm32l4xx_hal_qspi.c
    lib/STM32Cube/Src/stm32l4xx_hal_rcc.c
    lib/STM32Cube/Src/stm32l4xx_hal_rcc_ex.c
    lib/STM32Cube/Src/stm32l4xx_hal_rng.c
    lib/STM32Cube/Src/stm32l4xx_hal_rtc.c
    lib/STM32Cube/Src/stm32l4xx_hal_rtc_ex.c
    lib/STM32Cube/Src/stm32l4xx_hal_sai.c
    lib/STM32Cube/Src/stm32l4xx_hal_sai_ex.c
    lib/STM32Cube/Src/stm32l4xx_hal_sd.c
    lib/STM32Cube/Src/stm32l4xx_hal_sd_ex.c
    lib/STM32Cube/Src/stm32l4xx_hal_emmc.c
    lib/STM32Cube/Src/stm32l4xx_ll_exti.c
    lib/STM32Cube/Src/stm32l4xx_ll_gpio.c
    lib/STM32Cube/Src/stm32l4xx_ll_pwr.c
    lib/STM32Cube/Src/stm32l4xx_ll_rcc.c
    lib/STM32Cube/Src/stm32l4xx_ll_sdmmc.c
    lib/STM32Cube/Src/stm32l4xx_ll_utils.c

    bsp/mcu/startup_stm32l476xx.s
    bsp/mcu/burnto.c

    arm/src/system_stm32l4xx.c

    src/main.c
    src/bldbg.c
    src/stm32l4xx_it.c
    src/stm32l4xx_hal_msp.c


    # VCS: don't move the parentheses
    )

include_directories(bsp)
include_directories(inc)
include_directories(inc/config)
include_directories(lib/error_handler)
include_directories(lib/blibGPIO)
include_directories(lib/blibOSWrap)
include_directories(lib/BLOADER)
include_directories(lib/BLOADER/bootloader)
include_directories(lib/BLOADER/bl_md5)
include_directories(lib/BLOADER/fw_interface)
include_directories(lib/CMSIS/Include)
include_directories(lib/CMSIS/Device/ST/STM32L4xx/Include)
include_directories(lib/emmc)
include_directories(lib/RTT)
include_directories(lib/STM32Cube/Inc)

########################################################
### project targets/executables
########################################################
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY build)

### List of all targets, default are release and debug
set(TARGET_LIST release)

foreach(targ ${TARGET_LIST})
    add_executable(${targ} ${SOURCE_FILES})

    ### set output/build directory for targets
    set_target_properties(${targ} PROPERTIES RUNTIME_OUTPUT_DIRECTORY ${PROJECT_SOURCE_DIR}/build/${targ})
    set_target_properties(${targ} PROPERTIES OUTPUT_NAME ${targ}.elf)

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

### if necessary, add custom compile definitions to targets set in TARGET_LIST
### uncomment the lines and add e.g. "debug_no_bloader" target to the TARGET_LIST
### to support the bootloader and the application relocation
### consult the BL Wiki for more information on the bootloader and linker script
target_compile_definitions(release PRIVATE "-DRELEASE")
