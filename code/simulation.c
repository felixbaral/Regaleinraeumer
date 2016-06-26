#include "simulation.h"

/*
 * Typ: 
 * 1: x,y-Sensoren
 * 2: drei Z-Sensoren auf dem Turm
 * 3: Lichtschranken
 * 4: Lichttaster
 */


void Sensorverwaltung(void)
{
	printf("Start: Task - Sensorverwaltung \n");
	
	/* create message queue */
	if ((mesgQueueIdSensorCollector = msgQCreate(MSG_Q_MAX_Messages,1,MSG_Q_FIFO))	== NULL)
		printf("msgQCreate in failed\n");
	//printf("messageQ created\n");
	
	/*create Binary Semaphore*/
	semBinary_SteuerungToSimulation = semBCreate(SEM_Q_FIFO, SEM_FULL);
	//printf("Semaphore für Simulation <-> Steuerung erstellt");
	
	int i;
	for (i=0; i < 26; i++)
	{
		taskSpawn("Sensor",110,0x100,2000,(FUNCPTR)sensor,i,0,0,0,0,0,0,0,0,0);
	}
	
	taskSpawn("SensorCollector", 120, 0x100, 2000, (FUNCPTR)SensorCollector, 0,0,0,0,0,0,0,0,0,0);
	
	//printf("Sensor & Sensorcollector gespawnt \n");

	abusdata AktorBusData;
	int direction;
	while(true)
	{
		// regelmaessige Abfrage + weitersetzen
		semTake(semBinary_SteuerungToSimulation, WAIT_FOREVER); 
			/*
			 * TODO: Variable zwischenspeichern und nur kurzer Lock 
			 * oder ohne kopieren und mit langem Lock
			 * sieht vielleicht besser aus ...?
			 */
		abusdata AktorBusData = SteuerungToSimulation;
		semGive(semBinary_SteuerungToSimulation);
		//x-Achse
		direction = 0;
		if (AktorBusData.abits.axl)
			direction--;
		if (AktorBusData.abits.axr)
			direction++;
		if (AktorBusData.abits.axs)
			direction = direction * 2;
		towerPositionX += direction;
		if (towerPositionX < 0)
			towerPositionX = 0;
		if (towerPositionX > 9*sensorDistanceX)
			towerPositionX = 9*sensorDistanceX;
		
		//y-Achse
		direction = 0;
		if (AktorBusData.abits.ayu)
			direction++;
		if (AktorBusData.abits.ayo)
			direction--;
		towerPositionY += direction;
		if (towerPositionY < 0)
			towerPositionY = 0;
		if (towerPositionY > 9*sensorDistanceY)
			towerPositionY = 9*sensorDistanceY;
		
		//z-Achse
		direction = 0;
		if (AktorBusData.abits.azv)
			direction++;
		if (AktorBusData.abits.azh)
			direction--;
		towerPositionZ += direction;
		if (towerPositionZ < 0)
			towerPositionZ = 0;
		if (towerPositionZ > 2*sensorDistanceZ)
			towerPositionZ = 2*sensorDistanceZ;
		
		// Ein- & Ausgabe-Slot
		//TODO: Slave3
		
		
		taskDelay(Delay_Time_SensorVerwaltung);
	}
}

void sensor(int id)
{
	MessageQSensorresult returnValue;
	returnValue.result.id = id;
	int triggerOffset;
	printf("Start: Task - Sensor #%d\n", id);

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
		//TODO:Lichtsensoren
		//returnValue.result.value = 
	}



	while(1)
	{
		if(id < 10)// Y-Sensoren
		{
			//returnValue.result.value = triggersY(triggerOffset);
			returnValue.result.value = false; //test
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

		if((msgQSend(mesgQueueIdSensorCollector,&returnValue.charvalue, 1, WAIT_FOREVER, MSG_PRI_NORMAL)) == ERROR)
		{
			printf("msgQSend in Sensor #%d failed\n", id);			
		}
		
		taskDelay(Delay_Time_Sensor);
	}	
}

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

void SensorCollector(void){
	printf("Start: Task - SensorCollector \n");

	/* create message queue */
	if ((mesgQueueIdSensorData = msgQCreate(MSG_Q_MAX_Messages,5,MSG_Q_FIFO))	== NULL){
		printf("msgQCreate in failed\n");
	}
	else{
		//printf("messageQ-SensorData created\n");
	}
	
	sbusdata sensorBusData;
	
	//test start
	sensorBusData.l=0;
	sensorBusData.l--;
	sensorBusData.sbits.x1 = 1;
	sensorBusData.sbits.x2 = 0;
	sensorBusData.sbits.x3 = 1;
	printf("test start \n%d \n", sensorBusData.sbits.x1);
	printf("%d \n", sensorBusData.sbits.x2);
	printf("%d \n", sensorBusData.sbits.x3);
	sensorBusData.index[10] = 1;
	sensorBusData.index[11] = 1;
	sensorBusData.index[12] = 1;
	printf("\n%d \n", sensorBusData.sbits.x1);
	printf("%d \n", sensorBusData.sbits.x2);
	printf("%d \n", sensorBusData.sbits.x3);
	taskDelay(1000);
	//test end
		
	while(1){
		MessageQSensorresult ValueToBus;
		if (msgQNumMsgs(mesgQueueIdSensorCollector) > 0)
		{
			if(msgQReceive(mesgQueueIdSensorCollector, &ValueToBus.charvalue, 1, WAIT_FOREVER) == ERROR)
			{
				printf("msgQReceive in SensorCollector failed\n");
			}
			else
			{
				if (ValueToBus.result.value){
					printf("Sensor #%d ist 	++ aktiv ++\n",ValueToBus.result.id);
					
				}
				else {
					printf("Sensor #%d ist 	-- passiv --\n",ValueToBus.result.id); 	
				}
			}
		}
		
	taskDelay(Delay_Time_SensorCollector);
	}
}
