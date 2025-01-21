//Dependencies
#include "main.h"

//Vehicle Telemetry Variables
float rpm, tps, iacv, spd, afr, iat, clt, battery = 0.0f;

//Method to send command to Nextion
int next_cmd(struct sp_port *serial, const char *cmd){
    sp_blocking_write(serial, cmd, strlen(cmd), 1000); //Send the command string
    sp_blocking_write(serial, "\xFF\xFF\xFF", 3, 1000); //Send the command terminator
    return 0;
}

//Main function
int main(){

    //Initialize Nextion Serial Port using libserialport
    struct sp_port *serial_nextion;
    if(sp_get_port_by_name(IF_NEXTION, &serial_nextion) != SP_OK){
        fprintf(stderr, "Error: Unable to find serial port\n");
        //Close MEMS Serial Port Socket
        exit(1);
    }

    //Attempting to open Nextion Serial Port
    if(sp_open(serial_nextion, SP_MODE_READ_WRITE) != SP_OK){
        fprintf(stderr, "Error: Unable to open serial port\n");    
        sp_free_port(serial_nextion);
        //Close MEMS Serial Port Socket
        exit(1);
    }

    sp_set_baudrate(serial_nextion, 9600);
    sp_set_flowcontrol(serial_nextion, SP_FLOWCONTROL_NONE);

    print("NEXTION MEMS1.9 DASH");

    //Main Loop
    while(1){

    }

    sp_close(nextion_serial);
    sp_free_port(nextion_serial);
    //Close MEMS1.9 socket

    return 0;
}