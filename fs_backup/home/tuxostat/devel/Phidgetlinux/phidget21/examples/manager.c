/*
* manager.cpp
*
* Patrick McNeil - July 19, 2005
*
* Simple program to list all Phidgets that are added and removed from the system
*/

#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include "phidget21.h"
#include <signal.h>
		
int something;
static int exit = 0;

void sighandler (int n)
{
    printf ("got a signal. exiting.\n");
    exit = 1;
}

int gotAttach(CPhidgetHandle phid, void *ptr) {
	char *id;
	int serial, version;
	
	//print out some info  
	CPhidget_getSerialNumber(phid, &serial);
  	CPhidget_getDeviceVersion(phid, &version);
	CPhidget_getDeviceType((CPhidgetHandle)phid,(const char **)&id);
	printf("Device Added: %s, Serial: %d, Version: %d\n",id,serial,version);
	
	return 0;
}

int gotDetach(CPhidgetHandle phid, void *ptr) {
	char *id;
	int serial, version;
	
	//print out some info  
	CPhidget_getSerialNumber(phid, &serial);
  	CPhidget_getDeviceVersion(phid, &version);
	CPhidget_getDeviceType((CPhidgetHandle)phid,(const char **)&id);
	printf("Device Removed: %s, Serial: %d, Version: %d\n",id,serial,version);
	
	return 0;
}


int main() 
{
  CPhidgetManagerHandle phidm;
  CPhidgetManager_create(&phidm);
  CPhidgetManager_set_OnAttach_Handler(phidm, gotAttach, NULL);
  CPhidgetManager_set_OnDetach_Handler(phidm, gotDetach, NULL);
  CPhidgetManager_open(phidm);

  signal (SIGTERM, sighandler);
  signal (SIGINT, sighandler);

while(!exit)
  sleep(5);

  printf ("Removing resources.\n");
	 CPhidgetManager_close(phidm);
	 CPhidgetManager_delete(phidm);
  return 0;
}
