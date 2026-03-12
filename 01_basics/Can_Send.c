/**
 * can_send.c
 * CAN Frame Transmitter using SocketCAN
 *
 * Sends UDS DiagnosticSessionControl request
 * CAN ID  : 0x7DF (UDS functional address)
 * Service : 0x10  (DiagnosticSessionControl)
 * Session : 0x01  (Default session)
 *
 * AUTOSAR Equivalent: DCM sends session request via CanIf_Transmit()
 */

#include <stdio.h>          // printf, perror
#include <string.h>         // memset
#include <unistd.h>         // write, close
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
    printf("Step 1: Socket created! sock=%d\n", sock);

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
    printf("Step 2: Bound to vcan0 successfully!\n");

    /*
     * STEP 3 - Build CAN Frame
     * AUTOSAR Equivalent: PduInfo construction in DCM
     *
     * UDS DiagnosticSessionControl Request:
     * data[0] = 0x02  - length (2 bytes follow)
     * data[1] = 0x10  - service ID
     * data[2] = 0x01  - default session
     * data[3-7]       - padding
     */
    struct can_frame frame;
    memset(&frame, 0, sizeof(frame));
    frame.can_id  = 0x7DF;  // UDS functional address
    frame.can_dlc = 8;      // data length
    frame.data[0] = 0x02;   // length
    frame.data[1] = 0x10;   // DiagnosticSessionControl
    frame.data[2] = 0x01;   // default session
    frame.data[3] = 0x00;   // padding
    frame.data[4] = 0x00;
    frame.data[5] = 0x00;
    frame.data[6] = 0x00;
    frame.data[7] = 0x00;
    printf("Step 3: Frame built | ID: 0x%X  Service: 0x%X  Session: 0x%X\n", frame.can_id, frame.data[1], frame.data[2]);

    /*
     * STEP 4 - Transmit CAN Frame
     * AUTOSAR Equivalent: CanIf_Transmit()
     */
    int bytes = write(sock, &frame, sizeof(frame));
    if(bytes > 0)
        printf("Step 4: Frame sent successfully! (%d bytes)\n", bytes);
    else {
        perror("Send failed");
        return 1;
    }

    /*
     * STEP 5 - Cleanup
     * AUTOSAR Equivalent: CanIf_DeInit()
     */
    close(sock);
    printf("Step 5: Socket closed!\n");

    return 0;
}