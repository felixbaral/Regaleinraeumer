#include "simulation.h"

/*
 * Typ: 
 * 1: x,y-Sensoren
 * 2: drei Z-Sensoren auf dem Turm
 * 3: Lichtschranken
 * 4: Lichttaster
 */


bool triggersX(int x)
{
	if(towerPositionX == x) return true;
	return false;
}

bool triggersY(int y)
{
	if(towerPositionY == y) return true;
	return false;
}

bool triggersZ(int z)
{
	if(towerPositionZ == z) return true;
	return false;
}

void sensor(int id)
{
	MessageQSensorresult returnValue;
	returnValue.result.id = id;
	int triggerOffset;
	
	//TODO: die positionen zu beginn nur einmal ausrechnen
	
	printf("Sensor #%d started \n", id);

	if(id < 10)
	{
		// unten
		if(id < 5)
		{
			triggerOffset = triggersY((id*2+1)*sensorDistanceY);
		}
		// oben
		else
		{
			triggerOffset = triggersY((id-5)*2*sensorDistanceY);
		}
	}
	// X-Achse
	else if(id < 20)
	{
		triggerOffset = triggersX((id-10)*sensorDistanceX);
	}
	// Z-Achse
	else if(id < 23)
	{
		//Z=0: Arm im Regal drinne
		triggerOffset = triggersZ((id-20)*sensorDistanceZ);
	}
	// Lichtkram
	else
	{
		//TODO:bla
		//returnValue.result.value = 
	}



	while(1)
	{
		if(id < 10)// Y-Sensoren
		{
			returnValue.result.value = triggersY(triggerOffset);
		}
		else if(id < 20)// X-Achse
		{
			returnValue.result.value = triggersX(triggerOffset);
		}
		else if(id < 23)// Z-Achse
		{	//Z=0: Arm im Regal drinne
			returnValue.result.value = triggersZ(triggerOffset);
		}
		// Lichtkram
		else
		{
			//TODO:bla
			//returnValue.result.value = 
		}

		if((msgQSend(mesgQueueId,&returnValue.charvalue,1, WAIT_FOREVER, MSG_PRI_NORMAL)) == ERROR)
				printf("msgQSend in Sensor #%d failed\n", id);
		
		taskDelay(Delay_Time_Sensor);
	}
	
	printf("i'm dead");
	
}

void SensorCollector(void){
	printf("SensorCollector started \n");
	

	while(1){
		MessageQSensorresult ValueToBus;
		if(msgQReceive(mesgQueueId,&ValueToBus.charvalue,1, WAIT_FOREVER) == ERROR)
			printf("msgQReceive in SensorCollector failed\n");	
		else
			if (ValueToBus.result.bool){
				printf("Sensor #%d ist 	++ aktiv ++\n",ValueToBus.result.id); 
			}
			else {
				printf("Sensor #%d ist 	-- passiv --\n",ValueToBus.result.id); 	
			}
	
	taskDelay(Delay_Time_SensorCollector);
	}
}

void Sensorverwaltung(void)
{
	printf("start: sensortasks\n");
	
	
	/* create message queue */
	if ((mesgQueueId = msgQCreate(MSG_Q_MAX_Messages,1,MSG_Q_FIFO))
		== NULL)
		printf("msgQCreate in failed\n");
		
	//printf("messageQ created\n");
	
	int i;
	for (i=0; i < 2; i++)
	{
		taskSpawn("Sensor",110,0x100,2000,(FUNCPTR)sensor,i,0,0,0,0,0,0,0,0,0);
	}
	
	taskSpawn("SensorCollector", 120, 0x100, 2000, (FUNCPTR)SensorCollector, 0,0,0,0,0,0,0,0,0,0);
	
	//printf("Sensor & Sensorcollector gespawnt \n");

	while(true)
	{
	//TODO: regelmäßige abfrage + weitersetzen	
	taskDelay(Delay_Time_SensorVerwaltung);
	}
}

