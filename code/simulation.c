#include "simulation.h"

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


/*
 * Typ: 
 * 1: x,y-Sensoren
 * 2: drei Z-Sensoren auf dem Turm
 * 3: Lichtschranken
 * 4: Lichttaster
 */
void sensor(int id)
{
	printf("Sensor started %d \n", id);
	MessageQSensorresult returnValue;
	Sensorresult result;
	returnValue.result.id = id;
	
	//TODO: die positionen zu beginn nur einmal ausrechnen
	
	printf("bla 1\n");
	while(1)
	{
		
		// Y-Sensoren
		if(id < 10)
		{
			// unten
			if(id < 5)
			{
				returnValue.result.value = triggersY((id*2+1)*sensorDistanceY);
			}
			// oben
			else
			{
				returnValue.result.value = triggersY((id-5)*2*sensorDistanceY);
			}
		}
		// X-Achse
		else if(id < 20)
		{
			returnValue.result.value = triggersX((id-10)*sensorDistanceX);
		}
		// Z-Achse
		else if(id < 23)
		{
			//Z=0: Arm im Regal drinne
			returnValue.result.value = triggersZ((id-20)*sensorDistanceZ);
		}
		// Lichtkram
		else
		{
			//TODO:bla
			//returnValue.result.value = 
		}
		printf("bla 2\n");

		if((msgQSend(mesgQueueId,&returnValue.charvalue,1, WAIT_FOREVER, MSG_PRI_NORMAL)) == ERROR)
				printf("msgQSend in taskTwo failed\n");
		
		printf("I am %d\n", id);
		taskDelay(20);
	}
	
	printf("i'm dead");
	
}

void SensorCollector(void){
	printf("SensorCollector started \n");
	
	while(1){
		MessageQSensorresult ValueToBus;
		if(msgQReceive(mesgQueueId,&ValueToBus.charvalue,1, WAIT_FOREVER) == ERROR)
			printf("msgQReceive in taskTwo failed\n");	
		else
			printf("Message from %d \n",ValueToBus.result.id); 
		//msgQDelete(mesgQueueId); /* delete message queue */
	
	taskDelay(20);
	}
}

void Sensorverwaltung(void)
{
	printf("start: sensortasks\n");
	
	
	/* create message queue */
	if ((mesgQueueId = msgQCreate(maxMsgs,1,MSG_Q_FIFO))
		== NULL)
		printf("msgQCreate in failed\n");
		
	printf("messageQ created\n");

	taskDelay(1000);
	
	int i;
	for (i=0; i < 2; i++)
	{
		taskSpawn("Sensor",110,0x100,2000,(FUNCPTR)sensor,i,0,0,0,0,0,0,0,0,0);
	}
	
	taskSpawn("SensorCollector", 120, 0x100, 2000, (FUNCPTR)SensorCollector, 0,0,0,0,0,0,0,0,0,0);
	
	while(true)
	{
		
	taskDelay(20);
	}
}

