/**
 * can_server.c
 * UDS ECU Simulator using SocketCAN
 *
 * Simulates a real ECU responding to UDS diagnostic requests
 *
 * UDS Flow:
 * Tester  -> 7DF 02 10 01 00 00 00 00  (DiagnosticSessionControl)
 * ECU     -> 7E8 02 50 01 00 00 00 00  (Positive response)
 *
 * Response rule: service + 0x40 = positive response
 * 0x10 + 0x40 = 0x50
 *
 * AUTOSAR Equivalent: DCM module session handling
 */

#include <stdio.h>          // printf, perror
#include <string.h>         // memset
#include <unistd.h>         // read, write, close
#include <net/if.h>         // if_nametoindex
#include <sys/socket.h>     // socket, bind
#include <linux/can.h>      // can_frame, sockaddr_can
#include <linux/can/raw.h>  // CAN_RAW

int main()
{
    /*
     * STEP 1 - Create CAN Socket
     * AUTOSAR Equivalent: CanIf_Init()
     */
    int sock = socket(AF_CAN, SOCK_RAW, CAN_RAW);
    if(sock < 0) {
        perror("Socket creation failed");
        return 1;
    }

    /*
     * STEP 2 - Bind to vcan0 interface
     * AUTOSAR Equivalent: CanIf controller binding
     */
    struct sockaddr_can addr;
    memset(&addr, 0, sizeof(addr));
    addr.can_family  = AF_CAN;
    addr.can_ifindex = if_nametoindex("vcan0");
    if(bind(sock, (struct sockaddr*)&addr, sizeof(addr)) < 0) {
        perror("Bind failed");
        return 1;
    }
    printf("Socket created: %d | ECU Server running on vcan0\n", sock);

    /*
     * STEP 3 - Declare frames
     * Rx_frame - incoming UDS request
     * Tx_frame - outgoing UDS response
     *
     * AUTOSAR Equivalent:
     * Rx_frame = RxPduInfo in CanIf_RxIndication()
     * Tx_frame = PduInfo in CanIf_Transmit()
     */
    int frame_count = 0;
    struct can_frame Rx_frame;
    struct can_frame Tx_frame;

    /*
     * STEP 4 - Receive and Respond Loop
     * AUTOSAR Equivalent: DCM request processing
     */
    while(1)
    {
        // Block until CAN frame arrives
        int bytes = read(sock, &Rx_frame, sizeof(Rx_frame));

        if(bytes > 0)
        {
            frame_count++;

            // Print received frame
            printf("Frame %d: ID: 0x%X  DLC: %d  Data: ", frame_count, Rx_frame.can_id, Rx_frame.can_dlc);
            for(int i = 0; i < Rx_frame.can_dlc; i++)
                printf("%02X ", Rx_frame.data[i]);
            printf("\n");
            fflush(stdout);
        }

        /*
         * STEP 5 - Check and Respond to UDS Request
         * Check: CAN ID = 0x7DF (functional address)
         * Check: Service = 0x10 (DiagnosticSessionControl)
         *
         * AUTOSAR Equivalent: Dcm_DiagnosticSessionControl()
         *
         * Response:
         * data[0] = 0x02  - length
         * data[1] = 0x50  - positive response (0x10 + 0x40)
         * data[2] = 0x01  - default session confirmed
         */
        if(Rx_frame.can_id == 0x7DF && Rx_frame.data[1] == 0x10)
        {
            // Build response frame
            memset(&Tx_frame, 0, sizeof(Tx_frame));
            Tx_frame.can_id  = 0x7E8;   // ECU physical address
            Tx_frame.can_dlc = 8;       // data length
            Tx_frame.data[0] = 0x02;    // length
            Tx_frame.data[1] = 0x50;    // positive response
            Tx_frame.data[2] = 0x01;    // default session

            // Transmit response
            write(sock, &Tx_frame, sizeof(Tx_frame));
            printf("UDS Response sent | ID: 0x%X  Service: 0x%X  Session: 0x%X\n", Tx_frame.can_id, Tx_frame.data[1], Tx_frame.data[2]);
            fflush(stdout);
        }
    }

    /*
     * STEP 6 - Cleanup
     * AUTOSAR Equivalent: CanIf_DeInit()
     */
    close(sock);
    return 0;
}
```

---

**Now you have all 4 files in professional format:**
```
can_send.c    - done
can_receive.c - done
can_server.c  - done
can_filter.c  - done