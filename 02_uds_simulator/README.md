# 02_uds_simulator - UDS ECU Simulator

## Overview
Simulates a real ECU responding to UDS diagnostic
requests over virtual CAN bus.

This demonstrates how a real automotive ECU processes
diagnostic requests — same logic as AUTOSAR DCM module.

## AUTOSAR Mapping
| Program | AUTOSAR Equivalent |
|---------|-------------------|
| can_server.c | DCM module session handling |

## UDS Protocol Flow
```
Tester  ->  7DF 02 10 01 00 00 00 00
ECU     ->  7E8 02 50 01 00 00 00 00
```

### Frame Breakdown

Request (Tester sends):
```
CAN ID  : 0x7DF        - UDS functional address
data[0] : 0x02         - length (2 bytes follow)
data[1] : 0x10         - DiagnosticSessionControl
data[2] : 0x01         - Default session
data[3-7]: 0x00        - padding
```

Response (ECU sends):
```
CAN ID  : 0x7E8        - ECU physical address
data[0] : 0x02         - length (2 bytes follow)
data[1] : 0x50         - positive response (0x10 + 0x40)
data[2] : 0x01         - default session confirmed
data[3-7]: 0x00        - padding
```

## Positive Response Rule
```
Request service + 0x40 = Positive response

0x10 + 0x40 = 0x50  (DiagnosticSessionControl)
0x22 + 0x40 = 0x62  (ReadDataByIdentifier)
0x27 + 0x40 = 0x67  (SecurityAccess)
0x2E + 0x40 = 0x6E  (WriteDataByIdentifier)
```

## How to Compile
```bash
gcc can_server.c -o can_server
```

## How to Run
```bash
# Terminal 1 - start ECU simulator
./can_server

# Terminal 2 - send UDS request
cansend vcan0 7DF#0210010000000000

# OR use can_send from 01_basics
cd ../01_basics
./can_send
```

## Expected Output

Terminal 1 (ECU simulator):
```
Socket created: 3 | ECU Server running on vcan0
Frame 1: ID: 0x7DF  DLC: 8  Data: 02 10 01 00 00 00 00 00
UDS Response sent | ID: 0x7E8  Service: 0x50  Session: 0x01
```

Terminal 2 (candump to verify):
```
vcan0  7DF  [8]  02 10 01 00 00 00 00 00
vcan0  7E8  [8]  02 50 01 00 00 00 00 00
```

## AUTOSAR DCM Connection

| C Code | AUTOSAR Equivalent |
|--------|-------------------|
| `Rx_frame.can_id == 0x7DF` | DCM receives on functional address |
| `Rx_frame.data[1] == 0x10` | DCM identifies service ID |
| `Tx_frame.data[1] = 0x50` | DCM sends positive response |
| `Tx_frame.can_id = 0x7E8` | DCM uses ECU physical address |
| `read()` | CanIf_RxIndication() callback |
| `write()` | CanIf_Transmit() |

## Supported UDS Services
| Service | ID | Response | Status |
|---------|----|----------|--------|
| DiagnosticSessionControl | 0x10 | 0x50 | Done |
| ReadDataByIdentifier | 0x22 | 0x62 | Planned |
| SecurityAccess | 0x27 | 0x67 | Planned |
| WriteDataByIdentifier | 0x2E | 0x6E | Planned |
| ECU Reset | 0x11 | 0x51 | Planned |

## Requirements
- Linux Ubuntu 22/24
- can-utils installed
- vcan0 interface running

## Setup
```bash
sudo modprobe vcan
sudo ip link add dev vcan0 type vcan
sudo ip link set up vcan0
```