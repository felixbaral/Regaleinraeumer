#include "taskLib.h"
#include "semLib.h"
#include "stdio.h"
#include "vxWorks.h"
#include <readcommand.h>
#include "msgQLib.h"
#include "busdata.h"

//Task-Frequency
#define Delay_Time_Sensor 20
#define Delay_Time_SensorCollector 20
#define Delay_Time_SensorVerwaltung 20


// 3D- Tower Navi
#define sensorDistanceX 10
#define sensorDistanceY 5
#define sensorDistanceZ 10
int towerPositionX;
int towerPositionY;
int towerPositionZ;

// MessageQueue
#define MSG_Q_MAX_Messages 200
MSG_Q_ID mesgQueueId;

// Struct for Bus-Communication
typedef struct {
	UINT id :7;
	bool value : 1;
} Sensorresult;

typedef union {
	char charvalue; // Für MessageQueue
	Sensorresult result;
} MessageQSensorresult;
