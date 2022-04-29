/*
uF sequencer and controller
Features:
1-Bpm set with menu
2-Bpm display
3-Sequence start-stop
4-discretized CV out (1V/Oct)

display layout:

  Status: play
  BPM:    120
  Steps:  8
  TRG:    int
  Timbre: SAW

*/
#include <Wire.h>
#include <Adafruit_MCP4725.h>
Adafruit_MCP4725 CV1_dac;
#define CV1DacAddress 0x60
uint32_t CV1Value;
Adafruit_MCP4725 CV2_dac;
#define CV2DacAddress 0x61
uint32_t CV2Value;

#include <Timer.h>
Timer seq;
Timer heartbeat;
Timer pressTimer;

#define dr digitalRead
#define dw digitalWrite
#define pressed 0


int seqBcdPins[3]={7,8,9};
int timbreSelPins[3]={10,11,12};
#define BusReadPin 14 //A0
#define busMax 1024 //to calibrate

int bpm, bpmTime;
int step=0;
bool busy=false;
int clockSource=0;// 0 INT 1 EXT
int scaleMode=0;//



////////////////////////////////LCD////////////////////////////////////
#include <U8glib.h>
U8GLIB_SSD1306_128X64 u8g(U8G_I2C_OPT_NONE|U8G_I2C_OPT_DEV_0);  // I2C / TWI 
#define oled_Y_Spacing 12
#define oled_Value_X_Spacing 60
#define oled_X_Spacing 5

///////////////////////////////HMI/////////////////////////////////////
int playStatus= -1; //1 play, 0 pause, -1 reset(temp)
#define EncAPin 2
#define EncBPin 3
#define EncPPin 4 
#define BA1Pin  5
#define BA2Pin  6
#define BPMLedPin 13
#define shortPressTime 800 //ms
///////////////////////////////MENU/////////////////////////////////////

int parameterSelection=0;
int changeValue=0;
bool inMenu=false;
bool isParameterSelected= false;

void setup(){
	CV1_dac.begin(CV1DacAddress);
	CV2_dac.begin(CV2DacAddress);

  	seq.init(1000);
  	seq.ckInit(100,50);
	
	heartbeat.init(1000);
	heartbeat.ckInit(100,50);
	pressTimer.init(1000);
	for(int i=0; i<3; i++) pinMode(seqBcdPins[i],OUTPUT); 	//set output sequencer bcd pins
	for(int i=0; i<3; i++) pinMode(timbreSelPins[i],OUTPUT);	//set output mixer bcd pins
	for(int i=2; i<7; i++) pinMode(i, INPUT_PULLUP);			//set input HMI pins
	pinMode(BPMLedPin,OUTPUT);
	u8g.setColorIndex(1);
	u8g.firstPage();
	 do {
	  	 u8g.setFont(u8g_font_unifont);
	  	 u8g.drawStr(oled_X_Spacing,oled_Y_Spacing,"BPM:");
	  	 if(clockSource==0) u8g.drawStr(oled_X_Spacing, 2* oled_Y_Spacing,"Clock: INT");
	  	 if(clockSource==1) u8g.drawStr(oled_X_Spacing, 2* oled_Y_Spacing,"Clock: EXT");
	  	 u8g.drawStr(oled_X_Spacing, 3* oled_Y_Spacing,"Scale: Minor");
	  	 u8g.drawStr(oled_X_Spacing, 4* oled_Y_Spacing,"LFO: SYNC");
	  	 u8g.drawStr(oled_X_Spacing, 5* oled_Y_Spacing,"Timbre: SAW");
	
  	} while( u8g.nextPage() );
  	oneVoctGenerator();
 
  	Serial.begin(9600);
}

int oldBPM=119,newBPM=120;
int nSteps=8;
/////////////////////////////////////////////loop/////////////////////////////////
void loop(){

  dw(BPMLedPin,!heartbeat.Clock());
    
  playStatus = playPauseReader(BA2Pin, BA1Pin, playStatus);
  //Serial.println(playStatus); //DEBUG
  if(playStatus== -1){
      step=0;
      bcdWrite(step);
      playStatus=0;
  }
  sequencer(nSteps, playStatus);
    
  menu();
}
////////////////////////////////////////
int menuSelection=0;
bool parameterSetted=false;
void menu(){

	if(seq.Clock())	Serial.println("in Menu");

	if(!isParameterSelected){
		menuSelection= constrain(getRotaryEncoder(oldBPM),0,5);

	}
	if(seq.Clock()){
		Serial.print("selection: "); //debug
		Serial.print(menuSelection);//debug
		Serial.println("");//debug
	}	
	if(isParameterSelected){

 		switch (menuSelection){
	 		case 1: //BPM section
	 			if(seq.Clock())	Serial.println("bpm Setting");
	 			//entering value changing stage
	 			if(enterShortPress() && isParameterSelected==false){
	 				isParameterSelected=true;
	 			}
	 			//value setting stage
	 			if (isParameterSelected && !parameterSetted){
					if(dr(EncPPin) == pressed){
				   	newBPM = constrain(getRotaryEncoderByTen(oldBPM),1,300);
					}else{
				  		newBPM = constrain(getRotaryEncoder(oldBPM),1,300);
					}
				}
				//updating value
				if(isParameterSelected && !parameterSetted &&enterShortPress()){
					oldBPM= newBPM;
					seq.ckSet((30000/oldBPM),50);
					heartbeat.ckSet((60000/oldBPM),50);
					drawMainPage();
					Serial.print("bpm update to: ");//DEBUG
					Serial.print(oldBPM);//DEBUG
					Serial.println("");//DEBUG
				}
	 				 	
	 		
	 		break;
	 		case 2: //clock source
	 			if(seq.Clock())	Serial.println("clock source Setting");
	 			//entering value changing stage
	 			if(enterShortPress() && isParameterSelected==false){
	 				isParameterSelected=true;
	 			}
	 			//value setting stage
	 			if (isParameterSelected && !parameterSetted){
					if(dr(EncPPin) == pressed){
				   	newBPM = constrain(getRotaryEncoderByTen(oldBPM),1,300);
					}else{
				  		newBPM = constrain(getRotaryEncoder(oldBPM),1,300);
					}
				}
				//updating value
				if(isParameterSelected && !parameterSetted &&enterShortPress()){
					oldBPM= newBPM;
					seq.ckSet((30000/oldBPM),50);
					heartbeat.ckSet((60000/oldBPM),50);
					drawMainPage();
					Serial.print("bpm update to: ");//DEBUG
					Serial.print(oldBPM);//DEBUG
					Serial.println("");//DEBUG
				}

	 		break;
	 		case 3:
	 		break;
	 		case 4:
	 		break;
	 		case 5:
	 		break;

 		default: break;
 		}
	}
}

bool latch=false;
bool enterShortPress(){
	if(!latch && dr(EncPPin)== pressed){
		latch=true;
		pressTimer.Trg(shortPressTime);
	}
	if(latch && dr(EncPPin)== !pressed && !pressTimer.Out()){
		pressTimer.Rst();
		latch =false;
		return true;
	}
	if(latch && dr(EncPPin)== !pressed && pressTimer.Out()) {
		pressTimer.Rst();
		latch =false;
		return false;
	}

}

////////////////////////////////////////////////////////////////////////////
void sequencer(int n, int stat){

  if(seq.Clock() && !busy && stat){
    step= (++step)%n;
    bcdWrite(step);
    busy=true;
  }
  if(!seq.Clock() && busy) busy= false;
}
void bcdWrite(int val){
  uint8_t out =val;
  digitalWrite(seqBcdPins[0], out&1);//lsb 0000 0011 & 0000 0001 = 0000 0001 >0 ==> true
  digitalWrite(seqBcdPins[1], out&2);//    0000 0011 & 0000 0010 = 0000 0010 >0 ==> true
  digitalWrite(seqBcdPins[2], out&4);//msb 0000 0011 & 0000 0100 = 0000 0000 =0 ==> false
  digitalWrite(10, out&1);//lsb 0000 0011 & 0000 0001 = 0000 0001 >0 ==> true
  digitalWrite(11, out&2);//    0000 0011 & 0000 0010 = 0000 0010 >0 ==> true
  digitalWrite(12, out&4);//msb 0000 0011 & 0000 0100 = 0000 0000 =0 ==> false

}

int getRotaryEncoder(int n)
{
  static int oldA = HIGH; //set the oldA as HIGH
  static int oldB = HIGH; //set the oldB as HIGH
  int result = 0;
  int newA = !digitalRead(EncAPin); //read the value of APin to newA
  int newB = !digitalRead(EncBPin); //read the value of BPin to newB
  if (newA != oldA || newB != oldB)//if the value of APin or the BPin has changed
  {  
    if (oldA == HIGH && newA == LOW)// something has changed
    {
      result = (oldB * 2 - 1);
    }
  }
  oldA = newA;
  oldB = newB;
  return n+result;
}
int getRotaryEncoderByTen(int n)
{
  static int oldA = HIGH; //set the oldA as HIGH
  static int oldB = HIGH; //set the oldB as HIGH
  int result = 0;
  int newA = !digitalRead(EncAPin); //read the value of APin to newA
  int newB = !digitalRead(EncBPin); //read the value of BPin to newB
  if (newA != oldA || newB != oldB)//if the value of APin or the BPin has changed
  {  
    if (oldA == HIGH && newA == LOW)// something has changed
    {
      result = (oldB * 2 - 1);
    }
  }
  oldA = newA;
  oldB = newB;
  return n+(10*result);
}

void drawMainPage(){
  u8g.firstPage();
    do {
        u8g.setFont(u8g_font_unifont);
        u8g.drawStr(oled_X_Spacing,oled_Y_Spacing,"BPM:");
        u8g.setPrintPos(oled_Value_X_Spacing ,oled_Y_Spacing); 
        u8g.print(oldBPM);
        u8g.drawStr(oled_X_Spacing, 2* oled_Y_Spacing,"Clock: INT");
        u8g.drawStr(oled_X_Spacing, 3* oled_Y_Spacing,"Scale: Minor");
        u8g.drawStr(oled_X_Spacing, 4* oled_Y_Spacing,"LFO: SYNC");
        u8g.drawStr(oled_X_Spacing, 5* oled_Y_Spacing,"Timbre: SAW");

  } while( u8g.nextPage() );
}

bool playFlag=false;
bool stopFlag=false;
#define pressed false
#define unpressed true

int playPauseReader(int PPpin,int Spin, int oldState){
  if(stopFlag ==false && dr(Spin)==pressed && oldState != -1 && step != 0) stopFlag=true;
  if(stopFlag && dr(Spin)==unpressed){
    stopFlag=false;
    return -1;
  }

    if(playFlag == false && dr(PPpin) == pressed){
      playFlag=true;
    }
    if(playFlag==true && dr(PPpin)== unpressed){
      playFlag= false;
      return !oldState;
    }
    else{
      return oldState;
    }
}


float oneVoct[60];
void oneVoctGenerator(){
	for(int i =0; i<60; i++){
		oneVoct[i]= i/12;
	}
}
int Oct=0;
int octSpan=1;
int oldNote=0;
void CV1Write(){
	int busRead =analogRead(BusReadPin);
	int qNote= constrain(map(busRead,0,busMax,0,(12*octSpan)+1), 0,12*octSpan);
	if(qNote != oldNote){
		CV1_dac.setVoltage(oneVoct[(Oct*12)+ qNote], false);
		oldNote = qNote;
	}
}

