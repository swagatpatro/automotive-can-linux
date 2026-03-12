#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <net/if.h>
#include <sys/socket.h>
#include <linux/can.h>
#include <linux/can/raw.h>

int main() {
    
    // ========================
    // STEP 1 — Create socket
    // ========================
    int sock = socket(AF_CAN, SOCK_RAW, CAN_RAW);
    if (sock < 0) {
        perror("Socket creation failed");
        return 1;
    }
    printf("✅ Step 1: Socket created!\n");

    // ========================
    // STEP 2 — Bind to vcan0
    // ========================
    struct sockaddr_can addr;
    memset(&addr, 0, sizeof(addr));
    addr.can_family  = AF_CAN;
    addr.can_ifindex = if_nametoindex("vcan0");
    
    if (bind(sock, (struct sockaddr*)&addr, sizeof(addr)) < 0) {
        perror("Bind failed");
        return 1;
    }
    printf("✅ Step 2: Bound to vcan0!\n");

    // ========================
    // STEP 3 — Build CAN frame
    // ========================
    struct can_frame frame;
    memset(&frame, 0, sizeof(frame));
    
    frame.can_id  = 0x7DF;   // UDS functional address
    frame.can_dlc = 8;       // 8 bytes data
    
    // UDS DiagnosticSessionControl request
    frame.data[0] = 0x02;    // Length — 2 bytes follow
    frame.data[1] = 0x10;    // Service ID — 0x10 DiagnosticSessionControl
    frame.data[2] = 0x01;    // Sub function — Default session
    frame.data[3] = 0x00;    // Padding
    frame.data[4] = 0x00;
    frame.data[5] = 0x00;
    frame.data[6] = 0x00;
    frame.data[7] = 0x00;
    
    printf("✅ Step 3: CAN frame built!\n");
    printf("   CAN ID  : 0x%X\n", frame.can_id);
    printf("   Service : 0x%X (DiagnosticSessionControl)\n", frame.data[1]);
    printf("   Session : 0x%X (Default)\n", frame.data[2]);

    // ========================
    // STEP 4 — Send frame
    // ========================
    int bytes = write(sock, &frame, sizeof(frame));
    if (bytes > 0)
        printf("✅ Step 4: Frame sent successfully! (%d bytes)\n", bytes);
    else {
        perror("Send failed");
        return 1;
    }

    // ========================
    // STEP 5 — Close socket
    // ========================
    close(sock);
    printf("✅ Step 5: Socket closed!\n");
    printf("\n🎉 Your first CAN program ran successfully!\n");
    
    return 0;
}