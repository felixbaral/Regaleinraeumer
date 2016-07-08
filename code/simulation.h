#ifndef DOUBLE_INCLUDE_CHECK_SIMULATION
#define DOUBLE_INCLUDE_CHECK_SIMULATION

#include "taskLib.h"		// Task-Spawn
#include "semLib.h"			// Semaphore
#include "msgQLib.h"		// msgQ
#include "busdata.h"		// Busdaten-Datentyps
#include "bool_types.h"		// Bool-Datentyp
#include "config.h"			// Userconfig

// Selbsberechnug der Sensorenentfernungen auf einen ungeraden Wert fuer die X-Achse
#define sensorDistanceX (SensorDistanceX + (1 - (SensorDistanceX %2) ) )


//Aktordaten - Steuerung zu Simulation (global)
SEM_ID semBinary_SteuerungToSimulation;
abusdata SteuerungToSimulation; //used global

// MessageQueue
MSG_Q_ID mesgQueueIdSensorData;
// Matrix mit der Regalbelegung
bool belegungsMatrix[10][5];

//------------------------------------------
int Simulation_init(void);

#endif /* DOUBLE_INCLUDE_CHECK_SIMULATION */
