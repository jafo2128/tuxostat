// - Encoder simple -
// This example simply creates an Encoder handle, creates it, hooks the event handlers, and then waits for an encoder is attached.
// Once it is attached, the program will wait for user input so that we can see the event data on the screen when using the
// encoder.
// For a more detailed example, please see the Encoder-full example.
//
// Please note that this example was designed to work with only one Phidget Encoder connected. 
// For an example using multiple Phidget Encoders, please see a "multiple" example in the Encoder Examples folder.
//
// Copyright 2007 Phidgets Inc.  All rights reserved.

#include <stdio.h>
#include <phidget21.h>

int AttachHandler(CPhidgetHandle ENC, void *userptr)
{
	int serialNo;
	CPhidget_getSerialNumber(ENC, &serialNo);
	printf("Encoder %10d attached!", serialNo);

	return 0;
}

int DetachHandler(CPhidgetHandle ENC, void *userptr)
{
	int serialNo;
	CPhidget_getSerialNumber(ENC, &serialNo);
	printf("Encoder %10d detached!", serialNo);

	return 0;
}

int ErrorHandler(CPhidgetHandle ENC, void *userptr, int ErrorCode, const char *Description)
{
	printf("Error handled. %i - %s", ErrorCode, Description);

	return 0;
}

int InputChangeHandler(CPhidgetEncoderHandle ENC, void *usrptr, int Index, int State)
{
	printf("Input #%i - State: %i", Index, State);

	return 0;
}

int PositionChangeHandler(CPhidgetEncoderHandle ENC, void *usrptr, int Index, int Position, int Time)
{
	printf("Encoder #%i - Position: %d -- Elapsed Time: %d", Index, Position, Time);

	return 0;
}

//Display the properties of the attached phidget to the screen.  We will be displaying the name, serial number and version of the attached device.
//Will also display the number of inputs and encoders on this device
int display_properties(CPhidgetEncoderHandle phid)
{
	int serialNo, version, num_inputs, num_encoders;
	const char* ptr;

	CPhidget_getDeviceType((CPhidgetHandle)phid, &ptr);
	CPhidget_getSerialNumber((CPhidgetHandle)phid, &serialNo);
	CPhidget_getDeviceVersion((CPhidgetHandle)phid, &version);

	CPhidgetEncoder_getNumInputs(phid, &num_inputs);
	CPhidgetEncoder_getNumEncoders(phid, &num_encoders);

	printf("%s\n", ptr);
	printf("Serial Number: %10d\nVersion: %8d\n", serialNo, version);
	printf("Num Encoders: %d\nNum Inputs: %d\n", num_encoders, num_inputs);

	return 0;
}

int encoder_simple()
{
	int result;
	const char *err;

	//Declare an accelerometer handle
	CPhidgetEncoderHandle encoder = 0;

	//create the accelerometer object
	CPhidgetEncoder_create(&encoder);

	//Set the handlers to be run when the device is plugged in or opened from software, unplugged or closed from software, or generates an error.
	CPhidget_set_OnAttach_Handler((CPhidgetHandle)encoder, AttachHandler, NULL);
	CPhidget_set_OnDetach_Handler((CPhidgetHandle)encoder, DetachHandler, NULL);
	CPhidget_set_OnError_Handler((CPhidgetHandle)encoder, ErrorHandler, NULL);

	//Registers a callback that will run if an input changes.
	//Requires the handle for the Phidget, the function that will be called, and an arbitrary pointer that will be supplied to the callback function (may be NULL).
	CPhidgetEncoder_set_OnInputChange_Handler(encoder, InputChangeHandler, NULL);

	//Registers a callback that will run if the encoder changes.
	//Requires the handle for the Encoder, the function that will be called, and an arbitrary pointer that will be supplied to the callback function (may be NULL).
	CPhidgetEncoder_set_OnPositionChange_Handler (encoder, PositionChangeHandler, NULL);

	CPhidget_open((CPhidgetHandle)encoder, -1);

	//get the program to wait for an accelerometer device to be attached
	printf("Waiting for accelerometer to be attached....");
	if((result = CPhidget_waitForAttachment((CPhidgetHandle)encoder, 10000)))
	{
		CPhidget_getErrorDescription(result, &err);
		printf("Problem waiting for attachment: %s\n", err);
		return 0;
	}

	//Display the properties of the attached accelerometer device
	display_properties(encoder);

	//read accelerometer event data
	printf("Reading.....\n");

	//keep displaying accelerometer data until user input is read
	printf("Press any key to end\n");
	getchar();

	//since user input has been read, this is a signal to terminate the program so we will close the phidget and delete the object we created
	printf("Closing...\n");
	CPhidget_close((CPhidgetHandle)encoder);
	CPhidget_delete((CPhidgetHandle)encoder);

	//all done, exit
	return 0;
}
int main(int argc, char* argv[])
{
	encoder_simple();
	return 0;
}

