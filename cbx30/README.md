# About connectivity hex

* target MCU: `NRF52832`
* UART Baud: 1000000 bps
* UART flow control enabled
* Even parity

# How to flash hex to the target using nrfjprog

```
nrfjprog -f nrf52 --program ble_connectivity_s132_uart_pca10040.hex --sectorerase
nrfjprog -f nrf52 --reset

nrfjprog -f nrf52 --program s132_nrf52_6.1.1_softdevice.hex --sectorerase
nrfjprog -f nrf52 --reset
```

* Alternativel you can use `JFlash` or `JFlash Lite` to burn `ble_connectivity_s132_uart_pca10040.hex `

# How to rebuild from source
To rebuild connectivity app from the source, download SDK version `nRF5_SDK_15.3.0` navigate to `examples/connectivity/`