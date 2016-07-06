#ifndef DOUBLE_INCLUDE_CHECK_SIMULATION
#define DOUBLE_INCLUDE_CHECK_SIMULATION

#include "taskLib.h"
#include "semLib.h"
#include "msgQLib.h"
#include "busdata.h"
#include "bool_types.h"
#include "config.h"


#define sensorDistanceX (SensorDistanceX + (1 - (SensorDistanceX %2) ) )


//Aktordaten - Steuerung zu Simulation (global)
SEM_ID semBinary_SteuerungToSimulation;
abusdata SteuerungToSimulation; //used global

// MessageQueue
MSG_Q_ID mesgQueueIdSensorData;

//------------------------------------------
int Simulation_init(void);

bool belegungsMatrix[10][5];


#endif /* DOUBLE_INCLUDE_CHECK_SIMULATION */
