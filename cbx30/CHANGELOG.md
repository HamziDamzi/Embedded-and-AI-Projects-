# Changelog
All notable changes to this project will be documented in this file.

The format is based on [Keep a Changelog](http://keepachangelog.com/en/1.0.0/)
and this project adheres to [Semantic Versioning](http://semver.org/spec/v2.0.0.html).

## [01-04-00] - 2022-09-26
### Changed
- Updated buzzer sound for starting and stopping session  
- Sending IMU and GNSS data to logger task only when active session.  

### Removed
- Logger task doesn't check for active session when saving or sending IMU and GNSS data  

### Fixed
- Fixed calling xQueueReceive function without calling xQueueSelectFromSet  
- Fixing logger queue blockage on session start due to file name format. 
  Blockage is caused by conversion of file name from LFN to SFN format when opening file.  
  Fixed by opening file with simple name and renaming file to session id on session stop.  
- Fixing accidentally continue writing to file when session isn't active
- Fixed integrity of accessing emmc by using emmc mutex for every emmc write and read

## [01-03-01] - 2022-09-16
### Fixed
- Using IMU non buffered filtered accelerometer vector values

## Added
- Compatibility for using multiple format of IMU accelerometer information
    1. Accelerometer axis values
    2. Accelerometer vector value - calculated from axis values

## [01-03-00] - 2021-02-22

### Fixed
- Fixed long reconnect times with HR sensors

### Changed
- Pause scanning when data transfer is in progress
- Send HR data continuously
- Reduce advertising interval to 250ms

## [01-02-02] - 2020-09-28

### Fixed
- fixed soft device event queue overflow

## [01-02-01] - 2020-09-04

## Changed
- Send live data on BLE only when session is active

### Fixed
- fixed bug when sending filtered IMU values

## [01-02-00] - 2020-07-30

## Changed
- Extended jump detection algorithm: values are sent only if 25 acceleration vectors in a row are under a threshold
- Reduced acceleration vector threshold value to 1020

## [01-01-01] - 2020-07-03

## Changed
- Appended BLE MAC address bytes in reverse order to advertising SSID
- Increased timeout when waiting for SEND OK to 15 seconds

## [01-01-00] - 2020-06-16

## Changed
- Removed acc_x,acc_y and acc_z from session_imu_t; replaced with vector magnitude
- session_imu are saved / sent only when below defined ACC_THRESHOLD

## [01-00-01] - 2020-06-05

## Changed
- Increased min/max connection interval on BLE
- Increased adv interval to 

## [01-00-00] - 2020-06-02

### Changed
- Switched to v01-00-00

## [00-02-11] - 2020-05-08

### Fixed
- Stop sending chuns when wifi error occurs (#103)
- Session entry incorrect indices (#104)

## [00-02-10] - 2020-04-24

### Fixed
- Check if we are charging when powering off
- Logger now works continuously (resolved hanging on session stop)
- Session list starts from beginning on CMD_FIRST request
- Race condition when sending and receiving concurrently on BLE

### Changed
- Max number of sessions raised to 1250

## [00-02-09] - 2020-04-03

### Fixed
- HRM MAC Address saving
- BLE hang issue when sending/receiving concurrently
- Hard Fault on GPS lock
- Blink issue
- Session info duration 
- Charging state transition fix

## [00-02-08] - 2020-03-13

### Changed
- IMU frequency changed to 50 Hz
- Removed gyro data from messages
- Low battery event messages are send every 5 seconds
- Changed discharge battery characteristics

### Added
- Longpress during charging will turn on the device
- Data is transmited over BLE even if session is not started

## [00-02-07] - 2020-03-03

### Fixed
- Auto HRM pairing on boot

### Changed
- LED's LP animations
- Session info returns session duration

### Added
- Wifi 30seconds timeout on inactivity
- Device power off command

## [00-02-06] - 2020-02-14

### Fixed
- GPS timestamp set to every ~100ms

### Changed
- Updated nanopb library
- Added dummy field to empty requests

### Added
- If battery is below 3.3V device is going to shutdown.
- Current session file is closed if device is going to shutdown.
- If waked up with charger turns off when charger is disconnected.

## [00-02-05] - 2020-01-20

### Added
- Wifi AP and TCP Server on default IP Address (192.168.4.1:8080)
- Wifi is enabled/disabled with proto request

## [00-02-04] - 2020-01-07
### Added
- Do not start session if the free space on emmc is less than 128MB.
- Sound and light indication of low memory on disk
- BLE command `pbft_fs_meta` can be used to get disk size over BLE

## [00-02-03] - 2019-12-20

### Fixed
- BLE task is no longer hanging when connection is lost
- Fixed hard fault when receiving session_list cmd on device with more than 200 sessions
- Device does not power off when there is an active BLE connection

## [00-02-02] - 2019-12-14

### Added
- Added external heart rate support

### Changed
- Optimized BLE parameters when using peripheral and central role

## [00-02-01] - 2019-12-10

### Changed
- IMU fetches samples at ~ 100Hz

## [00-02-00] - 2019-12-04

### Changed
- Decreased BLE MTU to 185 bytes (182 user payload)
- IMU fetches sample every 20 miliseconds (~ 50Hz rate)

### Added
- Request for restarting device

## [00-02-00] - 2019-12-02

### Changed
- Decreased BLE MTU to 185 bytes (182 user payload)
- IMU fetches sample every 20 miliseconds (~ 50Hz rate)

### Added
- Request for restarting device

## [00-01-00] - 2019-12-02

### Added
- This changelog.
- CI build system. 
- Firmware version is kept in CmakeList.txt under `FW_VER` variable.

### Fixed
- build scripts independent from which folder called

