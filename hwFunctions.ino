



//////////////////////////  CV-DAC Write funct

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

//////////////////////////// 1 volt-per-octave array generator
double *oneVoctGenerator(){
	
	double *oneVoct = (double)malloc(sizeof(double) * 60);
	for(int i =0; i<60; i++){
		oneVoct[i]= i/12;
	}
	return oneVoct;
}

///////// writes int val in binary over 3 pins

void bcdWrite(int val){

	uint8_t out =val;
  	digitalWrite(seqBcdPins[0], out&1);//lsb 0000 0011 & 0000 0001 = 0000 0001 >0 ==> true
  	digitalWrite(seqBcdPins[1], out&2);//    0000 0011 & 0000 0010 = 0000 0010 >0 ==> true
  	digitalWrite(seqBcdPins[2], out&4);//msb 0000 0011 & 0000 0100 = 0000 0000 =0 ==> false
  	digitalWrite(10, out&1);//lsb 0000 0011 & 0000 0001 = 0000 0001 >0 ==> true
  	digitalWrite(11, out&2);//    0000 0011 & 0000 0010 = 0000 0010 >0 ==> true
  	digitalWrite(12, out&4);//msb 0000 0011 & 0000 0100 = 0000 0000 =0 ==> false

}

