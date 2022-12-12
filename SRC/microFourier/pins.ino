int seqBcdPins[3]={7,8,9};
int timbreSelPins[3]={10,11,12};
#define BusReadPin 14 //A0
#define busMax 1024 //to calibrate

///////////////////////////////HMI/////////////////////////////////////
int playStatus= -1; //1 play, 0 pause, -1 reset(temp)
#define EncAPin 2
#define EncBPin 3
#define EncPPin 4 
#define BA1Pin  5
#define BA2Pin  6
#define BPMLedPin 13
#define shortPressTime 800 //ms