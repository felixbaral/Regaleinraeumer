#include "taskLib.h"
#include "semLib.h"
#include "stdio.h"
#include "vxWorks.h"

int towerPositionX;
int towerPositionY;

void sensor(int x, int y, int typ)
{
	int id = taskIdSelf();
	while(1)
	{
		printf("I am %d\n", id);
		taskDelay(20);
	}
	printf("i'm dead");
	
}

void main(void)
{
	for (int i=0; i < 2; i++)
	{
		taskSpawn((char)i,90,0x100,2000,(FUNCPTR)sensor,i,i,i,0,0,0,0,0,0,0);
	}
	while(1)
	{}
	return 1;
}

