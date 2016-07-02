#ifndef DOUBLE_INCLUDE_CHECK_SIMULATION
#define DOUBLE_INCLUDE_CHECK_SIMULATION

#include "taskLib.h"
#include "semLib.h"
#include "msgQLib.h"
#include "busdata.h"
#include "bool_types.h"

#include "config.h"

//Aktordaten - Steuerung zu Simulation (global)
SEM_ID semBinary_SteuerungToSimulation;
abusdata SteuerungToSimulation; //used global

// MessageQueue
MSG_Q_ID mesgQueueIdSensorData;

//------------------------------------------
int Simulation_init(void);

#endif /* DOUBLE_INCLUDE_CHECK_SIMULATION */
