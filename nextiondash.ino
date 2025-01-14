/*
	Clone library from: https://github.com/DFRobot/DFRobot_MCP2515.git
	
	Serial: 0 (RX) 1 (TX) (DEBUG)
	Serial1: 19 (RX) 18 (TX) (NEXTION)
*/

//#include "DFRobot_MCP2515.h"

const int SPI_CS_PIN=10;
//DFRobot_MCP2515 CAN(SPI_CS_PIN);

//CAN Related Variables
unsigned char flagRecv = 0;
unsigned char len = 0;
unsigned char buf[8];

//Vehicle Telemetry Variables
unsigned int rpm = 0;
unsigned int tps = 0;
double spd = 0.0;
double afr = 0.0;
double iat = 0.0;
double clt = 0.0;
unsigned int gear = 0;
double battery = 0.0;

//CAN to Nextion Methods
int rpm2ProgBar(int rpm){
	return ((rpm+1000)/100);
}

//ISR Method
void MCP_2515_ISR(){flagRecv=1;}

void setup(){
	
	//Start Serial Debug Console
	Serial.begin(115200); 
	Serial.println("NEXTION CANBUS ENABLED DASH");
	
	//Start Nextion Serial Port
	Serial1.begin(115200);
	
	//Attempt CAN BUS Initialization at 1MBit per second
	while(CAN.begin(CAN_1000KBPS)){
		Serial.println("CANBUS INIT FAILURE \nPlease initialize again.");
		delay(1000);
	}
	Serial.println("CANBUS INIT SUCCESS")
	
	//Setting mask to accept all IDs with 29 bits
	CAN.initMask(MCP2515_RXM0, 0, 0x1FFFFFFF);
	CAN.initMask(MCP2515_RXM1, 0, 0x1FFFFFFF);
	
	/*
		The CAN IDs that we need to filter out are all 8 bits:
		
		0x00001000, 0x00001001, 0x00001003
	*/
	CAN.initFilter(MCP2515_RXF0, 0, 0x00001000);
	CAN.initFilter(MCP2515_RXF1, 0, 0x00001001);
	CAN.initFilter(MCP2515_RXF2, 0, 0x00001003);
	
	//On receiving a message, perform ISR below.
	attachInterrupt(0, MCP2515_ISR, FALLING);
}

// Function to send a command to Nextion
void sendToNextion(String cmd) {
  Serial1.print(cmd);      // Send the command string
  Serial1.write(0xFF);    // End of command
  Serial1.write(0xFF);    // End of command
  Serial1.write(0xFF);    // End of command
}

//CAN to MCU

//0x00001000 processing method. Calculate RPM and TPS.
void thousand(unsigned char buf[8]){
	
	//Obtaining Data from 0x00001000
	rpm = (buf[0] << 8) | buf[1];
	tps = buf[6];
	
	//Printing Data to Nextion
	sendToNextion("tacho.val="+String(rpm2ProgBar(rpm)));
	//Add TPS Method
}

//0x00001001 processing method. Calculate speed in kmph and AFR.
void thousand-one(unsigned char buf[8]){
	
	//Obtaining Data from 0x00001001
	double constant = (2.25/256)*1.609344;
	double spdraw = (buf[2] << 8) | buf[3];
	
	spd = spdraw*constant;
	
	afr = ((buf[6] << 8) | buf[7])/10;
	
	//Printing Data to Nextion
	sendToNextion("speedo.val="+String(spd));
	//Add AFR Method
}

//0x00001003 processing method. Calculate IAT, CLT, Gear and Battery.
void thousand-three(unsigned char buf[8]){
	iat = buf[0] - 40;
	clt = buf[1] - 40;
	gear = buf[5]++;
	battery = buf[7]/11;
	
	//Add Methods to Print Data to Nextion
}


void loop(){
	
	//First, check if any data received
	if(flagRecv){
		flagRecv = 0; //Reset the Flag
		
		CAN.readMsgBuf(&len, buf); //Update Length and Buffer Variables from CAN Message Buffer
		
		if(len == 8){
			unsigned long canID = CAN.getCanId(); //get CANID and store in variable for switch
			
			//Process depending on CANID
			switch(canID){
					
				case(0x00001000):
					thousand(buf);
				break;
				
				case(0x00001001):
					thousand-one(buf);
				break;
				
				case(0x00001003):
					thousand-three(buf);
				break;
				
				
			}
		}else{
			Serial.println("INVALID CAN MSG LENGTH");
		}
		
	}
}
 