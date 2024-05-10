CrossBox README
===============

## Index
* [UI Notifications](doc/ui_notifications.md)
* [Heart rate](doc/heart-rate.md)
* [Protobuf file generation](./src/protobuf/README.md)

## Versioning
02.06.2020 - From now on, we enforce following versioning scheme for firmwares.

`01-xx-yy` - where,
  -  01 is fixed until further notice
  - `xx` is increased on every `official release` and `yy` is reset to `00`
  - `yy` is increased on every `feature preview` release which is sent to client for testing feature before official release

## FSM generation
In root:

```bash
$ lib/blibFSM/tool/blfsm.py -g fsm_graph.png -gE -gC inc/crossbox_fsm.hpp
```

This will generate two files, `crossboxFSMAuto.hpp` and `crossboxFSMAuto.cpp`.
You need to move `crossboxFSMAuto.hpp` to `/inc` and `crossboxFSMAuto.cpp` to
`/src`.

```bash
$ mv crossboxFSMAuto.hpp inc/
$ mv crossboxFSMAuto.cpp src/
```

## Protobuf file generation

Use protobuf-compiler (protoc) from [docker image](https://git.byte-lab.com/internal/devops/docker/protobuf) for file generation.  
If needed pull docker image and change it's tag:
```bash
$ docker pull git.byte-lab.com:4567/internal/devops/docker/protobuf/protobuf:3.12.3
$ docker tag git.byte-lab.com:4567/internal/devops/docker/protobuf/protobuf:3.12.3 protobuf:latest
```

Generate `crossbox.pb.h` and `crossbox.pb.c` using docker:

```bash
$ cd src/protobuf/
$ docker run --rm -v $PWD:$PWD -w $PWD -i protobuf:latest bash $PWD/pb_gen.sh c crossbox.proto
```

Generate `crossbox_pb2.py` using docker and move it to toolbox:

```bash
$ cd src/protobuf/
$ docker run --rm -v $PWD:$PWD -w $PWD -i protobuf:latest bash $PWD/pb_gen.sh py crossbox.proto
$ mv crossbox_pb2.py ../../tools/pc_interfacer
```

---
## Tools
---

### PC Interfacer

`tools/pc_interfacer/` contains a python script that can be used to send and
receive protobuf messages to & from the CrossBox. 

#### How to use:

 - First, you need to make sure that the script is using an up to date version
of the Protocol Buffers file. You can regenerate them by following section [Protobuf file generation](#protobuf-file-generation).
Make sure that the up to date `crossbox_pb2.py`
and `nano_pb2.py` are in the same directory as the `main.py` file of the
pc_interfacer.

 - Connect device to USB port and find available serial ports:
```bash
ls /dev/tty* # example: look for /dev/ttyACM0
```

 - Execute script using docker: 

If needed pull docker image and change it's tag:
```bash
$ docker pull git.byte-lab.com:4567/internal/devops/docker/nrf-pc-ble-driver/nrf_pc_ble_driver:1.0
$ docker tag git.byte-lab.com:4567/internal/devops/docker/nrf-pc-ble-driver/nrf_pc_ble_driver:1.0 nrf_pc_ble_driver:latest
```
To show live session messages set second argument when running **crossbox_nus.py** to 1.  
Live session messages will be shown when run command **LiveTX**.

Executing script with set serial port:

```bash
$ cd tools/pc_interfacer/
$ docker run --rm -it -v $PWD/../../:$PWD/../../ -w $PWD --device=<Serial port> nrf-pc-ble-driver:latest python3 crossbox_nus.py <Serial port> <Enable live session message show>

# example:

$ docker run --rm -it -v $PWD/../../:$PWD/../../ -w $PWD --device=/dev/ttyACM0 nrf-pc-ble-driver:latest python3 crossbox_nus.py /dev/ttyACM0 1
```

This will give you a prompt. Type "help" for a list of commands and their short
description. Alternatively, you can double-tap `TAB` to see just the names of 
the available commands.

| Available commands | Description |
| ---                | ------      |
|list_sessions | Get a list of the session files on crossbox|
|session_info  | Ask for session info (Session ID prompted)|
|session_chunk | Ask for a chunk of session (session id, entry_num & entry_len will be asked)|
|session_chunk_repeat | Ask for a chunk of session (session id, entry_num, entry_len and repeat will be asked) |
|delete_session | Send a request to delete a session (Session ID prompted)|
|wifi_ctrl_enable | Enables wifi as AP and starts TCP IP Server on 192.168.4.1:8080|
|dev_info | Get device info|
|wifi_ctrl_disable | Disables wifi and powers it down|
|dev_ctrl_start | Send CMD_START|
|dev_ctrl_stop | Send CMD_STOP|
|dev_ctrl_pause | Send CMD_PAUSE|
|dev_ctrl_restart | Send CMD_RESTART|
|dev_ctrl_power_off | Send CMD_POWER_OFF|
|dev_ctrl_factory_reset | Send CMD_FACTORY_RESET|
|liveTX | Receive live flow of msgs from crossbox|
|hrm_pair | Initiate pairing with external heart rate monitor|
|hrm_pair_del | Delete paired external heart rate monitor|
|scan | Scans for devices for scan duration (prompted)|
|connect | Connects to device (ID prompted)|
|disconnect | Terminates current connection|
|enable | Enables notification (live stream)|
|disable | Disables notification |
|help | Print this help|
|exit | Disconnect and then exit program|
|device_status | Get device status|
---

### BT_FTS - Bluetooth file transfer tool

`tools/bt_fts` contains a python script that implements PBFT messages support
over bluetooth for the crossbox project.

#### How to use:

 - Get NRF52 dongle
 - Follow [nux-x86's directions](https://git.byte-lab.com/1000-109-crossbox/3300-144-crossbox/tree/master/lib/nus-x86-client)
 - You may need `sudo` priviledge to run the script.
 - Make sure script has up to date versions of crossbox and pbft protobuf generated files
 - Usage:

``` bash
$ sudo python3 tools/bt_fts/bt_fts.py --port <Serial Port> --help
```

| Available commands | Description |
| ---                | ------      |
| --help             | Print the help |
| --scan `<scan duration>` `<advertized name>` | Start a scan to see available bluetooth devices and their address |
| --proto-info `<address>` | Get PBFT protocol information: version, buff size, and checksum algorith |
| --write `<address>` `<file path>`            | Send the file at `<file path>` to the device |
| --read `<address>` `<file name>`             | Get the file from the device |
| --list-all `<address>` `<(optional) mask`    | List all files on the device (Mask is not implemented yet |
| --delete `<address>` `<file name>`           | Delete this file on the device |
| --meta `<address>` `<file name>`             | Get META information about the specified file. |
| --fs-meta `<address>` | Get filesystem information (Total size and available space |
| --enable-checksum     | Enable checksum calculation/verification for commands that support it (read/write/meta)
| --verbose             | Print incoming/outgoing protobuf messages |

#### About PBFT protocol:
 - Version: Indicates the version of the protofile on device
 - Buff size: The size of buffers used to read and write data. Note that bt_fts
 automatically asks for protocol information before all operations that need
 to know about buffer size or checksum algorithm.
 - For now, only MD5 is supported as a checksum algorithm


### DUT
DUT firmware is special located in /dut folder. It uses some of libraries and drivers
from this project. 
It has its own cmake and build output folder. 
