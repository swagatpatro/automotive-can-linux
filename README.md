# 🚗 Automotive CAN Linux Development

## 👨‍💻 About Me
8 years of Automotive BSW experience in AUTOSAR 
(DCM, CAN COM stack) transitioning to 
Linux CAN driver and firmware development.

## 🎯 Goal
Transitioning from AUTOSAR BSW configuration 
to Linux firmware and driver engineering.

## 🗺️ Learning Path
| Week | Topic | Status |
|------|-------|--------|
| 1-2  | SocketCAN basics | ✅ Done |
| 3-4  | Linux kernel modules | ⏳ Planned |
| 5-6  | Linux driver development | ⏳ Planned |
| 7+   | STM32 bare metal firmware | ⏳ Planned |

## 📁 Project Structure
| Folder | Description | Status |
|--------|-------------|--------|
| 01_basics | CAN send, receive, filter programs | ✅ Done |
| 02_uds_simulator | ECU diagnostic simulator | ✅ Done |
| 03_isotp | Multi frame UDS over ISOTP | ⏳ Planned |
| 04_kernel_modules | Linux kernel driver basics | ⏳ Planned |
| 05_stm32_baremetal | STM32 bare metal firmware | ⏳ Planned |

## 🛠️ Tech Stack
| Technology | Purpose |
|------------|---------|
| Linux Ubuntu 24 | Development platform |
| SocketCAN | CAN communication API |
| C | Programming language |
| can-utils | CAN testing tools |
| Virtual CAN (vcan) | Testing without hardware |
| STM32 Nucleo F446RE | Bare metal target (upcoming) |

## 🔗 AUTOSAR to Linux Mapping
| AUTOSAR | Linux Equivalent |
|---------|-----------------|
| CanIf_Transmit() | write(sock, &frame) |
| CanIf_RxIndication() | read(sock, &frame) |
| CanIf HW Filter config | setsockopt CAN_RAW_FILTER |
| CanTp ISO 15765-2 | CAN_ISOTP socket |
| COM Periodic TX | CAN_BCM socket |
| PduR routing | SocketCAN routing |
| MCAL CAN Driver | Linux CAN kernel driver |

## 🚀 Quick Start
### Requirements
- Linux Ubuntu 22/24
- gcc compiler
- can-utils
- Virtual CAN support

### Setup
```bash
