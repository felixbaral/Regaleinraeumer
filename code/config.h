#ifndef DOUBLE_INCLUDE_CHECK_CONFIG
#define DOUBLE_INCLUDE_CHECK_CONFIG

/*
 * für die Zusatzaufgabe bitte den define 
 * auf 1 (an) oder 0 (aus) setzen
 */
#define zusatz 0


//grundlegende Dimension des HRL
#define towerHeight 5 	// Y-Fächer
#define towerWidth 10	// X-Fächer
#define towerDepth  3	// Z-Ebenen

//Task-Priority
#define Priority_Main 200
#define Priority_Visualisierung 150
#define Priority_Simulation 50
#define Priority_HRL_Steuerung 80


//----- HRL_Steuerung ----------------------
#define MSG_Q_CMD_MAX_Messages 200 //Wie viele Befehle dürfen in der Warteschlange stehen?


//----- SIMULATION -------------------------
#define StartPositionX 9
#define StartPositionY 5
#define StartPositionZ 1


//Task-Frequency
#define Delay_Time_Simulation 20

// EA-Slots (Eingabe links)
#define PositionXinput 1
#define PositionYinput 8
#define PositionXoutput 9
#define PositionYoutput 8


// 3D- Tower Navi
#define SensorDistanceX 10 
#define sensorDistanceY 5
#define sensorDistanceZ 10
#define Delay_Time_IO_Slots 8

//------------------------------------------------

#endif /* Config */
