# 04_gateway - CAN Gateway

## Overview
Simulates a real automotive gateway ECU that forwards
CAN frames between two separate CAN buses.

Receives frames on vcan0 (Powertrain bus) and
forwards them to vcan1 (Body bus).

## AUTOSAR Mapping
| Code | AUTOSAR Equivalent |
|------|--------------------|
| socket1 bound to vcan0 | CanIf RX channel 0 |
| socket2 bound to vcan1 | CanIf TX channel 1 |
| read() | CanIf_RxIndication() |
| Tx_frame = Rx_frame | PduR frame copy |
| write() | CanIf_Transmit() |
| while(1) | OS cyclic task |

## Network Topology
```
vcan0                    can_gateway.c                   vcan1
(Powertrain bus)         (Gateway ECU)                (Body bus)

Engine ECU    -->    Receives on vcan0    -->    Dashboard ECU
Sensor ECU    -->    Forwards to vcan1    -->    Display ECU
```

## Real Vehicle Equivalent
```
CAN Bus 1                Gateway ECU              CAN Bus 2
(Powertrain 500kbps) --> forwards frames --> (Body 125kbps)

Engine ECU                                    Dashboard ECU
Transmission ECU                              Door ECU
ABS ECU                                       Light ECU
```

## How It Works
```
Step 1: Gateway waits on vcan0 (blocking read)
Step 2: Frame arrives on vcan0
Step 3: Gateway prints received frame
Step 4: Gateway copies frame (Tx = Rx)
Step 5: Gateway forwards to vcan1
Step 6: Go back to Step 1
```

## Why Two Sockets?
```
socket1 -> bound to vcan0 -> RX only
socket2 -> bound to vcan1 -> TX only

One socket per CAN bus
Same concept as two CAN controllers in real ECU
```

## Why No memset on Frames?
```
Rx_frame -> filled completely by read()
            no garbage possible!

Tx_frame -> copied from Rx_frame (Tx = Rx)
            entire struct copied!
            no garbage possible!

memset only needed when building frame manually!
```

## How to Compile
```bash
gcc can_gateway.c -o can_gateway
```

## How to Run
```bash
# Terminal 1 - start gateway
./can_gateway

# Terminal 2 - listen on vcan1 (destination)
candump vcan1

# Terminal 3 - send frames on vcan0 (source)
cansend vcan0 123#AABBCCDD
cansend vcan0 7DF#0210010000000000
cansend vcan0 456#DEADBEEF
```

## Expected Output

Terminal 1 (gateway):
```
Gateway ready: vcan0 -> vcan1
Waiting for frames on vcan0...
vcan0 RX: ID: 0x123  DLC: 4  Data: AA BB CC DD
vcan1 TX: ID: 0x123  forwarded!
vcan0 RX: ID: 0x7DF  DLC: 8  Data: 02 10 01 00 00 00 00 00
vcan1 TX: ID: 0x7DF  forwarded!
```

Terminal 2 (candump vcan1):
```
vcan1  123  [4]  AA BB CC DD
vcan1  7DF  [8]  02 10 01 00 00 00 00 00
vcan1  456  [4]  DE AD BE EF
```

Note: candump vcan0 shows only sent frames
      candump vcan1 shows only forwarded frames

## Advanced - Selective Gateway

Current gateway forwards ALL frames.
Real automotive gateway forwards ONLY specific IDs.

Example selective routing:
```c
// forward only specific IDs
if(Rx_frame.can_id == 0x100 ||
   Rx_frame.can_id == 0x101 ||
   Rx_frame.can_id == 0x7DF)
{
    Tx_frame = Rx_frame;
    write(socket2, &Tx_frame, sizeof(Tx_frame));
}
```

## Future Improvements
| Feature | Description | Status |
|---------|-------------|--------|
| Selective routing | Forward only specific IDs | Planned |
| Bidirectional | Forward both ways | Planned |
| ID translation | Change ID while forwarding | Planned |
| Physical hardware | Pi to STM32 via MCP2515 | Week 6 |

## Requirements
- Linux Ubuntu 22/24
- can-utils installed
- vcan0 and vcan1 interfaces running

## Setup
```bash
sudo modprobe vcan
sudo ip link add dev vcan0 type vcan
sudo ip link set up vcan0
sudo ip link add dev vcan1 type vcan
sudo ip link set up vcan1
```