# nRF(NCS) with MCUboot OTA

This project implements a secure over-the-air (OTA) update system for Nordic nRF52840 using MCUboot as the secure bootloader within the Nordic Connect SDK (NCS) environment. It provides a reliable and secure way to update firmware on Nordic devices.

## Project Structure

```
├── pm_static.yml        # Memory partition configuration for nRF52840
├── CMakeLists.txt      # NCS build system configuration
└── [Other project files]
```

## Memory Layout

The nRF52840 system memory is organized into several partitions:

- **MCUboot** (0x0 - 0xc000): Secure bootloader
- **Primary Slot** (0xc200 - 0x86000): Active firmware
- **Secondary Slot** (0x86000 - 0x100000): OTA update storage

## Features

- Secure bootloader with MCUboot
- Over-the-air (OTA) firmware updates
- Secure firmware verification
- Rollback protection
- Power-fail safe updates

## Requirements

- Compatible MCU with sufficient flash memory
- Development environment setup for embedded development
- Proper signing keys for secure boot
- Hardware: nrf52840dk
- NCS SDK: v2.8.0

## Building

```bash
west build --pristine always --board nrf52840dk/nrf52840  --sysbuild
```
## OTA Update Process

1. New firmware is downloaded to the secondary partition
2. MCUboot verifies the firmware integrity and signature
3. On next boot, MCUboot swaps the primary and secondary partitions
4. New firmware becomes active
5. Automatic rollback on boot failure

## NRF52840-specific OTA Information

### Debugging

Use the nRF Connect for Desktop tools for debugging:
- Programmer app for flashing
- Serial Terminal for logs
- Bluetooth Low Energy app for OTA testing

### Hardware Setup

1. **nRF52840 DK Connections**:
   - USB connection for programming and debugging
   - Optional: External antenna for better BLE range
   - LED indicators:
     * LED1: Connection status
     * LED2: OTA progress
     * LED3: Update status
     * LED4: Error indicator

2. **J-Link Debug Probe** (integrated in nRF52840 DK):
   - Provides programming and debugging capabilities
   - Real-time debugging support
   - SWD interface

### Testing OTA Updates

1. **Using nRF Connect Mobile App**:
   - Install nRF Connect on your smartphone
   - Scan for your device
   - Connect to the device
   - Select DFU option
   - Choose the new firmware file (.bin)
   - Monitor update progress

2. **Using Custom Mobile App**:
   - Implement Nordic DFU Library
   - Follow Nordic's DFU protocol
   - Handle BLE connections
   - Manage firmware transfer

## Security

- Firmware images must be properly signed
- Version checking prevents rollback attacks
- Secure boot process ensures system integrity

## Contributing

Contributions are welcome! Please feel free to submit a Pull Request.

## License

This project is licensed under the MIT License - see the [LICENSE](LICENSE) file for details.

## Author

KUNYI CHEN <kunyi.chen@gmail.com>
