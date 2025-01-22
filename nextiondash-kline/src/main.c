//Dependencies
#include "main.h"

//Vehicle Telemetry Variables
float rpm, tps, iacv, spd, afr, iat, clt, battery = 0.0f;

// Function to invert the second byte of the ECU reply
int invert_second_byte(uint8_t *response, int length) {
    if (length > 1) {
        response[1] = ~response[1];  // Invert the second byte
    }
    return 0;
}

//Method to send command to Nextion
int next_cmd(struct sp_port *serial, const char *cmd){
    sp_blocking_write(serial, cmd, strlen(cmd), 1000); //Send the command string
    sp_blocking_write(serial, "\xFF\xFF\xFF", 3, 1000); //Send the command terminator
    return 0;
}

int spConfigure(struct sp_port *port, int baud_rate, int bits, int stop_bits, int parity){
    sp_set_baudrate(port, baud_rate);
    sp_set_bits(port, bits);
    sp_set_parity(port, parity);
    sp_set_stopbits(port, stop_bits);
    return 0;
}

int sendECUAddress(struct sp_port *port){
    //Start by setting serial port to 5 baud for wakeup
    sp_set_baudrate(port, 5);

    //Then, send the ECU Address Byte 0x16 one bit at a time at 5 baud rate
    uint8_t ecuAddress = ECU_ADDRESS;

    for(int i=0; i<8;i++){
        uint8_t bit = (ecuAddress >> (7-i)) & 1; //Get each bit from MSB to LSB
        uint8_t byteToSend = bit ? 0x00 : 0x01;
        sp_blocking_write(port, &byteToSend, 1);

        usleep(200000); //Sleep for 200ms (5 baud stands for 5 milliseconds per bit)
    }
    usleep(200000); //Extra delay to signal stop bit
    sp_set_baudrate(9600); //Set baudrate back to normal after transmission    
}


//Main function
int main(){

    //Starting Nextion Communications

    //Initialize Nextion Serial Port using libserialport
    struct sp_port *serial_nextion;
    if(sp_get_port_by_name(IF_NEXTION, &serial_nextion) != SP_OK){
        fprintf(stderr, "Error: Unable to find nextion serial port\n");
        exit(1);
    }

    //Attempting to open Nextion Serial Port
    if(sp_open(serial_nextion, SP_MODE_READ_WRITE) != SP_OK){
        fprintf(stderr, "Error: Unable to open nextion serial port\n");    
        sp_free_port(serial_nextion);
        //Close MEMS Serial Port Socket
        exit(1);
    }

    sp_set_baudrate(serial_nextion, 9600);
    sp_set_flowcontrol(serial_nextion, SP_FLOWCONTROL_NONE);

    //Starting MEMS1.9 Communications

    //Initialize MEMS1.9 SerialPort using libserialport
    struct sp_port *serial_MEMS;
    if(sp_get_port_by_name(IF_MEMS, &serial_MEMS) != SP_OK){
        fprintf(stderr, "Error: Unable to find MEMS1.9 serial port\n");
        exit(1);
    }

    //Attempting to open MEMS1.9 Serial Port
    if(sp_open(serial_MEMS, SP_MODE_READ_WRITE) != SP_OK){
        fprintf(stderr, "Error: Unable to open MEMS1.9 serial port\n");    
        sp_free_port(serial_MEMS);
        exit(1);
    }

    spConfigure(serial_MEMS, 9600, 8, 1, SP_PARITY_NONE);
    sendECUAddress(serial_MEMS);

    //Wait for ECU Response
    uint8_t response[ECU_REPLY_EXPECTED_LENGTH];
    int bytesRead = sp_blocking_read(serial_MEMS, response, ECU_REPLY_EXPECTED_LENGTH, 1000); //Operation to timeout after one second

    if(bytesRead > 0){
        printf("Response Received: ");
        for (int i = 0; i < bytesRead; i++) {
            printf("%02X ", response[i]);
        }
        printf("\n");
        // Invert the second byte of the response
        invert_second_byte(response, bytesRead);

        // Send the modified response back to the ECU (if needed)
        sp_blocking_write(serial_MEMS, response, bytesRead);
        printf("Sent modified response\n");
    } else {
        fprintf(stderr, "No response or error reading from ECU\n");
    }

    //Delete these accordingly
    sp_close(serial_MEMS);
    sp_free_port(serial_MEMS);    

    

    print("NEXTION MEMS1.9 DASH");

    //Main Loop
    while(1){

    }

    sp_close(nextion_serial);
    sp_free_port(nextion_serial);

    return 0;
}