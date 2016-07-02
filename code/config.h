#ifndef DOUBLE_INCLUDE_CHECK_CONFIG
#define DOUBLE_INCLUDE_CHECK_CONFIG

#define towerHeight 5 	// Y-F�cher
#define towerWidth 10	// X-F�cher
#define towerDepth  3	// Z-Ebenen

//Task-Priority
#define Priority_Main 200
#define Priority_Visualisierung 150
#define Priority_Simulation 50
#define Priority_HRL_Steuerung 80

//TODO: Steurung adden

//----- HRL_Steuerung ----------------------
#define MSG_Q_CMD_MAX_Messages 200 //Wie viele Befehle d�rfen in der Warteschlange stehen?



//----- SIMULATION -------------------------

//Task-Frequency
#define Delay_Time_Simulation 20

// EA-Slots (Eingabe links)
#define PositionXinput 5
#define PositionYinput 5
#define PositionXOutput 9
#define PositionYOutput 8

// 3D- Tower Navi
#define sensorDistanceX 10
#define sensorDistanceY 5
#define sensorDistanceZ 10


#endif /* Config */
