#ifndef DOUBLE_INCLUDE_CHECK_CONFIG
#define DOUBLE_INCLUDE_CHECK_CONFIG

#define towerHeight 5 	// Y-Fächer
#define towerWidth 10	// X-Fächer
#define towerDepth  3	// Z-Ebenen

//----- HRL_Steuerung ----------------------
#define MSG_Q_CMD_MAX_Messages 200 //Wie viele Befehle dürfen in der Warteschlange stehen?



//----- SIMULATION -------------------------

//Task-Frequency
#define Delay_Time_Simulation 20

//Task-Priority
#define Priority_Simulation 100

// EA-Slots (Eingabe links)
#define PositionXinput 0
#define PositionYinput 8
#define PositionXOutput 9
#define PositionYOutput 8

// 3D- Tower Navi
#define sensorDistanceX 10
#define sensorDistanceY 5
#define sensorDistanceZ 10


#endif /* Config */
