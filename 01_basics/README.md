# 01_basics - CAN Socket Programs

## Overview
Basic CAN socket programs using Linux SocketCAN API.
These programs demonstrate the fundamental concepts
of CAN communication in Linux.

## AUTOSAR Mapping
| Program | AUTOSAR Equivalent |
|---------|-------------------|
| can_send.c | CanIf_Transmit() |
| can_receive.c | CanIf_RxIndication() |
| can_filter.c | CanIf HW Filter config |

## Programs

### can_send.c
Sends UDS DiagnosticSessionControl request

- CAN ID  : 0x7DF (UDS functional address)
- Service : 0x10  (DiagnosticSessionControl)
- Session : 0x01  (Default session)

### can_receive.c
Receives and prints all CAN frames on vcan0

- Displays frame count
- Displays CAN ID
- Displays DLC
- Displays data bytes

### can_filter.c
Filtered CAN receiver - accepts only specific IDs

- Filter 1: 0x123 (exact match)
- Filter 2: 0x7DF (UDS functional address)
- Formula: (received_id AND mask) == (filter_id AND mask)
- Mask used: CAN_SFF_MASK = 0x7FF

## How to Compile
```bash
gcc can_send.c    -o can_send
gcc can_receive.c -o can_receive
gcc can_filter.c  -o can_filter
```

## How to Run

### Test can_send and can_receive
```bash
# Terminal 1 - start receiver
./can_receive

# Terminal 2 - send frame
./can_send
```

Expected output in Terminal 1:
```
Frame 1: ID: 0x7DF  DLC: 8  Data: 02 10 01 00 00 00 00 00
```

### Test can_filter
```bash
# Terminal 1 - start filter
./can_filter

# Terminal 2 - send accepted ID
cansend vcan0 123#AABBCCDD

# Terminal 2 - send rejected ID
cansend vcan0 456#DEADBEEF
```

Expected output in Terminal 1:
```
Filter active: accepting 0x123 and 0x7DF only
ID: 0x123  DLC: 4  Data: AA BB CC DD
```
Note: 0x456 will never appear - filtered out!

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