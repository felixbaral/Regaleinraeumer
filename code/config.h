#ifndef DOUBLE_INCLUDE_CHECK_CONFIG
#define DOUBLE_INCLUDE_CHECK_CONFIG

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
#define StartPositionX 0
#define StartPositionY 0
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
#define Delay_Time_IO_Slots 5

//------------------------------------------------

#endif /* Config */
