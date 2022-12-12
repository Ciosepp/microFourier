/*
layer 0 parameters:	
	0 - BPM
	1 - Clock Source
	2 - Scale type
	3 - LFO 
	4 - Timbre
 layer 1:
 	x.0 - back

 	0.1 - value

 	1.1 - INT
 	1.2 - Ext

 	2.1	- Chromatic
 	2.2 - Major
 	2.3 - Minor
 	2.4 - Pentatonic

 	3.1 - Sine
 	3.2 - Triangle
 	3.3 - Saw
 	3.4 - Square

 	4.1 -> 4.8
*/
bool isMenuEntered = false;
int layer = 0; //number of layer in wich navigating
int selection = 0 ; //parameter selected in x-layer
int layer1Nselections[5]= {1, 2, 4, 4, 8};

void menu(){
	isMenuEntered = enterExitMenu();
}

bool enterExitMenu(bool in, bool out, int oldState){
	return ((in && out) || oldState&& (!(in^out)) ); //made via Karnaugh map
}