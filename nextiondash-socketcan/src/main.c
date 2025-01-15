//Install socketcan library with sudo apt-get install libsocketcan

//Dependencies
#include "main.h"

//CAN Related Variables
unsigned char flagRecv, len = 0;
unsigned char buf[8];

//Vehicle Telemetry Variables
unsigned int rpm, tps, gear = 0;
double spd, afr, iat, clt, battery = 0.0;

//Method to send command to Nextion
int next_cmd(struct sp_port *serial, const char *cmd) {
    sp_blocking_write(serial, cmd, strlen(cmd), 1000); // Send the command string
    sp_blocking_write(serial, "\xFF\xFF\xFF", 3, 1000); // Send the command terminator
    return 0;
}

//SocketCAN initialization Method
int socketcan_init(const char *iface){
    
    /*
        This chunk of code below creates a socket and checks if the socket creation failed.
        If Socket creation fails, error is thrown and program terminates.
    */
    int s = socket(PF_CAN, SOCK_RAW, CAN_RAW);
    if(s < 0){
        perror("ERROR: Socket Creation Failure");
        exit(1);
    }

    /*
        This code first creates an ifreq struct named ifr to handle network interface requests including manipulation of the interface properties.
        strncpy is used to set the ifr_name property of ifr from iface, while making sure that it will not overflow the buffer. 
        Finally, ioctl is used to get the index of the network name, s for socket opened earlier,  (SIOCGIFINDEX) interface index, all stored in ifr. 
    */

    struct ifreq ifr;
    strncpy(ifr.ifr_name, iface, sizeof(ifr.ifr_name)-1);
    ioctl(s, SIOCGIFINDEX, &ifr);

    struct sockaddr_can addr;
    memset(&addr, 0, sizeof(addr));
    addr.can_family = AF_CAN;
    addr.can_ifindex = ifr.ifr_ifindex;

    if(bind(s, (struct sockaddr * )&addr, sizeof(addr)) < 0){
        perror("ERROR: Socket Bindings Failure");
        close(s);
        exit(1);
    }

    return s;
}

// CAN Data Processing
int thousand(unsigned char buf[8], struct sp_port *serial) {
    rpm = (buf[0] << 8) | buf[1];
    tps = buf[6];

    char rpm_cmd[50];
    char tps_cmd[50];

    unsigned int rpm_proc = ((rpm + 500) / 100);
    snprintf(rpm_cmd, sizeof(rpm_cmd), "tacho.val=%d", rpm_proc);
    next_cmd(serial, rpm_cmd);

    snprintf(tps_cmd, sizeof(tps_cmd), "tps.val=%d", tps);
    next_cmd(serial, tps_cmd);

    printf("RPM: %d, TPS: %d\n", rpm, tps); // Print data to serial console

    return 0;
}

int thousand1(unsigned char buf[8], struct sp_port *serial) {
    double spdraw = (buf[2] << 8) | buf[3];
    spd = spdraw * SPD_CONSTANT;

    afr = ((buf[6] << 8) | buf[7]) / 10;

    char spd_cmd[50];
    char afr_cmd[50];

    snprintf(spd_cmd, sizeof(spd_cmd), "speedo.val=%d", (int)spd);
    next_cmd(serial, spd_cmd);

    printf("Speed: %.2f km/h, AFR: %.1f\n", spd, afr); // Print data to serial console

    return 0;
}

int thousand3(unsigned char buf[8], struct sp_port *serial) {
    iat = buf[0] - 40;
    clt = buf[1] - 40;
    gear = buf[5];
    battery = buf[7] / 11.0;

    printf("IAT: %.1f, CLT: %.1f, Gear: %d, Battery: %.2f\n", iat, clt, gear, battery);

    return 0;
}

int main(){

    //Initialize SocketCAN
    int can_s = socketcan_init(IF_CANBUS);

   // Initialize Serial Port using libserialport
    struct sp_port *serial;
    if (sp_get_port_by_name(IF_NEXTION, &serial) != SP_OK) {
        fprintf(stderr, "ERROR: Unable to find serial port\n");
        close(can_s);
        exit(1);
    }

    if (sp_open(serial, SP_MODE_READ_WRITE) != SP_OK) {
        fprintf(stderr, "ERROR: Unable to open serial port\n");
        sp_free_port(serial);
        close(can_s);
        exit(1);
    }

    sp_set_baudrate(serial, 115200);
    sp_set_flowcontrol(serial, SP_FLOWCONTROL_NONE);

    printf("NEXTION CANBUS ENABLED DASH INIT\n");

    while (1) {
        struct can_frame frame;
        int nbytes = read(can_s, &frame, sizeof(struct can_frame));

        if (nbytes < 0) {
            perror("ERROR: CANBUS read error");
            break;
        }

        if (nbytes < sizeof(struct can_frame)) {
            fprintf(stderr, "ERROR: CAN frame incomplete\n");
            continue;
        }

        // Process CAN Frame based on ID
        unsigned long canID = frame.can_id;
        switch (canID) {
            case 0x00001000:
                thousand(frame.data, serial);
                break;

            case 0x00001001:
                thousand1(frame.data, serial);
                break;

            case 0x00001003:
                thousand3(frame.data, serial);
                break;

            default:
                printf("UNHANDLED CANID: 0x%lX\n", canID);
                break;
        }
    }

    sp_close(serial);
    sp_free_port(serial);
    close(can_s);

    return 0;
}
