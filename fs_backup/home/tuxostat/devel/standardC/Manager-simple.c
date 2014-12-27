// - Manager simple -
// This is a simple example showing how to setup a phidget manager and display a list of the currently connected
// Phidgets devices to the PC
//
// Copyright 2007 Phidgets Inc.  All rights reserved.

#include <stdio.h>
#include <phidget21.h>

int AttachHandler(CPhidgetHandle phid, void *userPtr)
{
	int serialNo;
	const char *name;

	CPhidget_getDeviceName (phid, &name);
	CPhidget_getSerialNumber(phid, &serialNo);
	printf("%s %10d attached!\n", name, serialNo);

	return 0;
}

int DetachHandler(CPhidgetHandle phid, void *userPtr)
{
	int serialNo;
	const char *name;

	CPhidget_getDeviceName (phid, &name);
	CPhidget_getSerialNumber(phid, &serialNo);
	printf("%s %10d detached!\n", name, serialNo);

	return 0;
}

int ErrorHandler(CPhidgetManagerHandle MAN, void *usrptr, int Code, const char *Description)
{
	printf("Error handled. %i - %s\n", Code, Description);
	return 0;
}

//Display the properties of the attached phidget(s) to the screen.  We will be displaying the name, serial number and version of the attached device(s).
int display_devices(CPhidgetManagerHandle MAN)
{
	int serialNo, version, numDevices, i;
	const char* ptr;
	CPhidgetHandle *devices;

	CPhidgetManager_getAttachedDevices (MAN, &devices, &numDevices);

	printf("|-   # -|-              Type              -|- Serial No. -|-  Version -|\n");
	printf("|-------|----------------------------------|--------------|------------|\n");


	for(i = 0; i < numDevices; i++)
	{
		CPhidget_getDeviceType(devices[i], &ptr);
		CPhidget_getSerialNumber(devices[i], &serialNo);
		CPhidget_getDeviceVersion(devices[i], &version);

		printf("|- %3d -|- %30s -|- %10d -|- %8d -|\n", i, ptr, serialNo, version);
		printf("|-------|----------------------------------|--------------|------------|\n");
	}

	return 0;
}

int manager_simple()
{
	//Declare an Manager handle
	CPhidgetManagerHandle man = 0;

	//create the Manager object
	CPhidgetManager_create(&man);

	//Set the handlers to be run when the device is plugged in or opened from software, unplugged or closed from software, or generates an error.
	CPhidgetManager_set_OnAttach_Handler(man, AttachHandler, NULL);
	CPhidgetManager_set_OnDetach_Handler(man, DetachHandler, NULL);
	CPhidgetManager_set_OnError_Handler(man, ErrorHandler, NULL);

	//open the Manager for device connections
	CPhidgetManager_open(man);

	//Display the properties of the attached devices
	printf("Press any key to display list of devices\n");
	getchar();

	display_devices(man);

	//end simulation
	printf("Press any key to end\n");
	getchar();

	//since user input has been read, this is a signal to terminate the program so we will close the phidget and delete the object we created
	printf("Closing...\n");
	CPhidgetManager_close(man);
	CPhidgetManager_delete(man);

	//all done, exit
	return 0;
}

int main(int argc, char* argv[])
{
	manager_simple();
	return 0;
}

