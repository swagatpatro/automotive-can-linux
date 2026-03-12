/**
 * can_receive.c
 * CAN Frame Receiver using SocketCAN
 *
 * Receives and prints all CAN frames on vcan0
 * Displays frame count, CAN ID and data bytes
 *
 * AUTOSAR Equivalent: CanIf_RxIndication callback
 */

#include <stdio.h>          // printf, perror
#include <string.h>         // memset
#include <unistd.h>         // read, close
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
    printf("Socket created: %d | Bound to vcan0 successfully!\n", sock);

    /*
     * STEP 3 - Receive Loop
     * AUTOSAR Equivalent: CanIf_RxIndication callback
     * Blocks on read() until frame arrives
     */
    int frame_count = 0;
    struct can_frame frame;

    while(1)
    {
        // Block until CAN frame arrives
        int bytes = read(sock, &frame, sizeof(frame));

        if(bytes > 0)
        {
            frame_count++;

            // Print frame count, ID, DLC and data
            printf("Frame %d: ID: 0x%X  DLC: %d  Data: ", frame_count, frame.can_id, frame.can_dlc);

            // Print each data byte
            for(int i = 0; i < frame.can_dlc; i++)
                printf("%02X ", frame.data[i]);

            printf("\n");
            fflush(stdout);  // force immediate print
        }
    }

    /*
     * STEP 4 - Cleanup
     * AUTOSAR Equivalent: CanIf_DeInit()
     */
    close(sock);
    return 0;
}
```

---

**Now do can_server.c yourself in same format!**
```
Structure to follow:
- Top comment block
- Header comments
- Step comments with AUTOSAR equivalent
- Single line printf where possible
- Error checking on socket and bind