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