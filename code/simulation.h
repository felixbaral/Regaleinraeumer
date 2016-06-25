#include "taskLib.h"
#include "semLib.h"
#include "stdio.h"
#include "vxWorks.h"
#include <readcommand.h>
#include "msgQLib.h"
#include "busdata.h"

#define sensorDistanceX 10
#define sensorDistanceY 5
#define sensorDistanceZ 10
#define maxMsgs 200
int towerPositionX;
int towerPositionY;
int towerPositionZ;

/* globals */
MSG_Q_ID mesgQueueId;


typedef struct {
	UINT id :7;
	bool value : 1;
} Sensorresult;

typedef union {
	char charvalue; // Für MessageQueue
	Sensorresult result;
} MessageQSensorresult;
