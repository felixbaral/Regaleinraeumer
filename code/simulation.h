#ifndef DOUBLE_INCLUDE_CHECK_SIMULATION
#define DOUBLE_INCLUDE_CHECK_SIMULATION

#include "taskLib.h"
#include "semLib.h"
#include "msgQLib.h"
#include "busdata.h"
#include "readcommand.h"

//Task-Frequency
#define Delay_Time_Simulation_Sensor 20
#define Delay_Time_Simulation_SensorCollector 20 //sollte 1/26 von Delay_Time_Sensor sein
#define Delay_Time_Simulation_SensorVerwaltung 20

//Task-Priority
#define Priority_Simulation_Sensor 100
#define Priority_Simulation_SensorCollector 100
#define Priority_Simulation_SensorVerwaltung 100

// 3D- Tower Navi
#define sensorDistanceX 10
#define sensorDistanceY 5
#define sensorDistanceZ 10


// EA-Slots
#define PositionXinput 0
#define PositionYinput 8
#define PositionXOutput 9
#define PositionYOutput 8

// Belegungsmatrix der Simulation
// Wird noch nicht ausgefŸllt
bool belegungsMatrix[10][5];



int towerPositionX;
int towerPositionY;
int towerPositionZ;

//Aktordaten - Steuerung zu Simulation (global)
SEM_ID semBinary_SteuerungToSimulation;
abusdata SteuerungToSimulation; //used global

// MessageQueue
#define MSG_Q_MAX_Messages 200
MSG_Q_ID mesgQueueIdSensorCollector;
MSG_Q_ID mesgQueueIdSensorData;


// Struct for Bus-Communication
typedef struct {
	UINT id :7;
	bool value : 1;
} Sensorresult;

typedef union {
	char charvalue; // Für MessageQueue
	Sensorresult result;
} MessageQSensorresult;


//------------------------------------------
int Simulation_init(void);

#endif /* DOUBLE_INCLUDE_CHECK_SIMULATION */
