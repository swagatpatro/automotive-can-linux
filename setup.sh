#!/bin/bash
#
# setup.sh
# One click setup for automotive-can-linux
#
# Sets up virtual CAN interface and
# compiles all CAN socket programs
#
# Usage: chmod +x setup.sh && ./setup.sh

echo "================================"
echo " Automotive CAN Linux Setup"
echo "================================"

# STEP 1 - Install can-utils
echo ""
echo "Step 1: Installing can-utils..."
sudo apt install can-utils gcc -y
echo "can-utils installed!"

# STEP 2 - Load vcan kernel module
echo ""
echo "Step 2: Loading vcan kernel module..."
sudo modprobe vcan
echo "vcan module loaded!"

# STEP 3 - Create virtual CAN interfaces
echo ""
echo "Step 3: Creating virtual CAN interfaces..."
sudo ip link add dev vcan0 type vcan
sudo ip link set up vcan0
sudo ip link add dev vcan1 type vcan
sudo ip link set up vcan1
echo "vcan0 and vcan1 ready!"

# STEP 4 - Verify interfaces
echo ""
echo "Step 4: Verifying interfaces..."
ip link show type vcan

# STEP 5 - Compile all programs
echo ""
echo "Step 5: Compiling all programs..."

# compile 01_basics
gcc 01_basics/can_send.c    -o 01_basics/can_send
gcc 01_basics/can_receive.c -o 01_basics/can_receive
gcc 01_basics/can_filter.c  -o 01_basics/can_filter
echo "01_basics compiled!"

# compile 02_uds_simulator
gcc 02_uds_simulator/can_server.c -o 02_uds_simulator/can_server
echo "02_uds_simulator compiled!"

echo ""
echo "================================"
echo " Setup Complete! Ready to run!"
echo "================================"
echo ""
echo "Quick test:"
echo "  Terminal 1: ./01_basics/can_receive"
echo "  Terminal 2: ./01_basics/can_send"