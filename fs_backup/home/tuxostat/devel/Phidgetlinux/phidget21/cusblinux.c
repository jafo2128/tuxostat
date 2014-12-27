/*
 *  cusblinux.cpp
 *
 *  Filled in by Daniel Risacher on 6/21/05
 *  Filled in more by Patrick McNeil on 6/27/05
 *  Copyright 2005 Phidgets Inc. All rights reserved.
 *
 */

#include "stdafx.h"
#include "cusb.h"

int CUSBCloseHandle(CPhidgetHandle phid) {
	int ret = 0;
	
	if (!phid)
		return EPHIDGET_INVALIDARG;

	CPhidget_clearStatusFlag(phid, PHIDGET_ATTACHED_FLAG);
	
	if (phid->deviceHandle == NULL)
		return EPHIDGET_NOTATTACHED;

	CThread_join(&phid->readThread);
		
	if((ret = usb_release_interface((usb_dev_handle *) phid->deviceHandle, Phid_Device_Def[phid->deviceIDSpec].pdd_iid)) < 0)
	{
		switch(ret)
		{
		case -ENODEV:
			//usb_release_interface called after the device was unplugged
			LOG(PHIDGET_LOG_WARNING, "usb_release_interface called on unplugged device.");
			break;
		default:
			LOG(PHIDGET_LOG_ERROR, "usb_release_interface failed with error code: %d \"%s\"", ret, strerror(-ret));
		}
	}
	
	if((ret = usb_close((usb_dev_handle *) phid->deviceHandle)) < 0)
	{
		LOG(PHIDGET_LOG_ERROR, "usb_close failed with error code: %d \"%s\"", ret, strerror(-ret));
		phid->deviceHandle = NULL;
		return EPHIDGET_UNEXPECTED;
	}
	
	phid->deviceHandle = NULL;
	
	return EPHIDGET_OK;
}

int CUSBSendPacket(CPhidgetHandle phid, unsigned char *buffer) {
	int BytesWritten = 0;

	if (!phid)
		return EPHIDGET_INVALIDARG;
		
	if (!CPhidget_statusFlagIsSet(phid, PHIDGET_ATTACHED_FLAG)
		&& !CPhidget_statusFlagIsSet(phid, PHIDGET_ATTACHING_FLAG))
		return EPHIDGET_NOTATTACHED;
		
	if (phid->deviceHandle == NULL)
	{
		LOG(PHIDGET_LOG_WARNING,"Handle for writing is not valid");
		return EPHIDGET_UNEXPECTED;
	}
		
	BytesWritten = usb_control_msg((usb_dev_handle *)phid->deviceHandle, 
					USB_ENDPOINT_OUT | USB_TYPE_CLASS | USB_RECIP_INTERFACE,
					USB_REQ_SET_CONFIGURATION,
					0x0200, /* value */
					Phid_Device_Def[phid->deviceIDSpec].pdd_iid, /* index*/
					(char *)buffer, 
					phid->outputReportByteLength, /* size */
					500); /* FIXME? timeout */
					
	if(BytesWritten < 0)
	{
		switch(BytesWritten)
		{
		case -ETIMEDOUT: //important case?
			return EPHIDGET_TIMEOUT;
		default:
			LOG(PHIDGET_LOG_ERROR, "usb_control_msg failed with error code: %d \"%s\"", BytesWritten, strerror(-BytesWritten));
			return EPHIDGET_UNEXPECTED;
		}
	}

	if (BytesWritten != phid->outputReportByteLength)
	{
		LOG(PHIDGET_LOG_WARNING,"Failure in CUSBSendPacket - Report Length"
			": %d, bytes written: %d",
		    (int)phid->outputReportByteLength, (int)BytesWritten);
		return EPHIDGET_UNEXPECTED;
	}
	
	return EPHIDGET_OK;
}

int CUSBSetLabel(CPhidgetHandle phid, char *buffer) {
	int BytesWritten = 0;
	int size = buffer[0];
	
	if(size>22) return EPHIDGET_INVALID;
	
	if (!phid)
		return EPHIDGET_INVALIDARG;
		
	if (!CPhidget_statusFlagIsSet(phid, PHIDGET_ATTACHED_FLAG)
		&& !CPhidget_statusFlagIsSet(phid, PHIDGET_ATTACHING_FLAG))
		return EPHIDGET_NOTATTACHED;
		
	if (phid->deviceHandle == NULL)
	{
		LOG(PHIDGET_LOG_WARNING,"Handle for writing is not valid");
		return EPHIDGET_UNEXPECTED;
	}
		
	BytesWritten = usb_control_msg(phid->deviceHandle, 
					USB_ENDPOINT_OUT | USB_TYPE_STANDARD | USB_RECIP_DEVICE,
					USB_REQ_SET_DESCRIPTOR,
					0x0304, /* value */
					0x0409, /* index*/
					(char *)buffer, 
					size, /* size */
					500); /* FIXME? timeout */
					
	if(BytesWritten < 0)
	{
		switch(BytesWritten)
		{
		case -ETIMEDOUT: //important case?
		default:
			LOG(PHIDGET_LOG_ERROR, "usb_control_msg failed with error code: %d \"%s\"", BytesWritten, strerror(-BytesWritten));
			return EPHIDGET_UNEXPECTED;
		}
	}

	if (BytesWritten != size)
	{
		LOG(PHIDGET_LOG_WARNING,"Failure in CUSBSetLabel - Report Length"
			": %d, bytes written: %d",
		    size, (int)BytesWritten);
		return EPHIDGET_UNEXPECTED;
	}
	
	return EPHIDGET_OK;
}

/* Buffer should be at least 8 bytes long */
int CUSBReadPacket(CPhidgetHandle phid, unsigned char *buffer) {
	int BytesRead = 0;
	
	if (!phid)
		return EPHIDGET_INVALIDARG;
		
	if (!CPhidget_statusFlagIsSet(phid, PHIDGET_ATTACHED_FLAG)
		&& !CPhidget_statusFlagIsSet(phid, PHIDGET_ATTACHING_FLAG))
		return EPHIDGET_NOTATTACHED;
		
	if (phid->deviceHandle == NULL)
	{
		LOG(PHIDGET_LOG_WARNING,"Handle for writing is not valid");
		return EPHIDGET_UNEXPECTED;
	}
	
	BytesRead = usb_interrupt_read((usb_dev_handle *)phid->deviceHandle, 
				Phid_Device_Def[phid->deviceIDSpec].pdd_iid+1, 
				(char *)buffer, 
				phid->inputReportByteLength, 
				500);
				
	if (BytesRead < 0)
	{
		switch(BytesRead)
		{
			// A timeout occured, but we'll just try again
			case -ETIMEDOUT:
				LOG(PHIDGET_LOG_VERBOSE, "usb_interrupt_read timeout: %d \"%s\"", BytesRead, strerror(-BytesRead));
				return EPHIDGET_TIMEOUT;
			case -EBUSY:
				//This happens when someone else calls claim_interface on this interface - basically just wait until they release it.
				//This will happen if an open occurs in another app which (for some reason) can steal the interface from this one.
				LOG(PHIDGET_LOG_INFO, "Device is busy on Read - try again.");
				return EPHIDGET_TIMEOUT;
			default:
				LOG(PHIDGET_LOG_ERROR, "usb_interrupt_read returned: %d \"%s\"", BytesRead, strerror(-BytesRead));
				return EPHIDGET_UNEXPECTED;
		}
	}

	if (BytesRead != phid->inputReportByteLength)
	{
		//Generally means the device was unplugged
		LOG(PHIDGET_LOG_WARNING,"Failure in CUSBReadPacket - Report Length"
			": %d, bytes read: %d",
		    (int)phid->inputReportByteLength, (int)BytesRead);
		return EPHIDGET_UNEXPECTED;
	}
	
	return EPHIDGET_OK;
}

int CUSBGetDeviceCapabilities(CPhidgetHandle phid, struct usb_device *dev, struct usb_dev_handle *udev) {
	unsigned char buf[255];
	int len = 0, i = 0;
	
	memset(buf, 0, sizeof(buf));
	
	len = usb_control_msg(udev, USB_ENDPOINT_IN+1, 
						USB_REQ_GET_DESCRIPTOR,
						(USB_DT_REPORT << 8) + 0, Phid_Device_Def[phid->deviceIDSpec].pdd_iid, (char*)buf, 
						sizeof(buf), 500 /* ms timeout */);
						
	if(len < 0)
	{
		switch(len)
		{
		case -ETIMEDOUT: //important case?
		default:
			LOG(PHIDGET_LOG_ERROR, "usb_control_msg failed in CUSBGetDeviceCapabilities with error code: %d \"%s\"", len, strerror(-len));
			return EPHIDGET_UNEXPECTED;
		}
	}
	
	if(len >= 10)
	{	
		for(i=10;i<len;i++) {
			if(buf[i]==0x81 && buf[i-2]==0x95)
				phid->inputReportByteLength=buf[i-1];
			if(buf[i]==0x91 && buf[i-2]==0x95)
				phid->outputReportByteLength=buf[i-1];
		}
	}
	else
	{
		LOG(PHIDGET_LOG_ERROR, "Couldn't get report lengths in CUSBGetDeviceCapabilities");
		return EPHIDGET_UNEXPECTED;
	}
	
	len = usb_get_string_simple(udev, 4, (char *)phid->label, 11);
		
	if(len < 0)
	{
		switch(len)
		{
		case -ETIMEDOUT: //important case?
		default:
			LOG(PHIDGET_LOG_ERROR, "usb_get_string_simple failed in CUSBGetDeviceCapabilities with error code: %d \"%s\" while reading label - this probably just means the device doesn't support labels, so this is fine.", len, strerror(-len));
		}
	}
	
	return EPHIDGET_OK;
}

/*
   This needs to maintain a list of devices - it's used by the phidget manager for
   keeping track of attach and detach events. (On Mac this is handled with 
   notifications, so this is not needed.) - PM
*/
int CUSBBuildList(CPhidgetList **curList) {
	int MemberIndex, i, found, ret;
	unsigned long Length;
	CPhidgetList *traverse;
	Length = 0;
	MemberIndex = 0;
	struct usb_bus *bus;
	struct usb_device *dev;
	usb_dev_handle *udev;
	CPhidgetHandle phid;

	TESTPTR(curList)
	
	usb_init();
	if((ret = usb_find_busses()) < 0)
	{
		LOG(PHIDGET_LOG_ERROR, "usb_find_busses failed with error code: %d \"%s\"", ret, strerror(-ret));
	}
	if((ret = usb_find_devices()) < 0)
	{
		LOG(PHIDGET_LOG_ERROR, "usb_find_devices failed with error code: %d \"%s\"", ret, strerror(-ret));
	}
	
	//search through all USB devices
	for (bus = usb_busses; bus; bus = bus->next) {
		for (dev = bus->devices; dev; dev = dev->next) {
		
			found = 0;
			if (AttachedDevices) {
				for (traverse = AttachedDevices; traverse;
					traverse=traverse->next) {
					if (!memcmp(traverse->phid->CPhidgetFHandle, dev->filename, 3)) {
						CList_addToList((CListHandle *)curList, traverse->phid, CPhidget_areEqual);
						found = 1;
					}
				}
			}
			if (!found) {
				for (i = 1; i<PHIDGET_DEVICE_COUNT; i++) {	 
					if ((dev->descriptor.idVendor == Phid_Device_Def[i].pdd_vid) && 
						(dev->descriptor.idProduct == Phid_Device_Def[i].pdd_pid)) {
						if (!(phid = (CPhidgetHandle)malloc(
						    sizeof (*phid))))
							return EPHIDGET_NOMEMORY;
						ZEROMEM(phid, sizeof(*phid));
						
						udev = usb_open(dev);
						if(udev) {
							if (dev->descriptor.bcdDevice < 0x100) 
								phid->deviceVersion = dev->descriptor.bcdDevice * 100;
							else
								phid->deviceVersion = ((dev->descriptor.bcdDevice >> 8) * 100) + ((dev->descriptor.bcdDevice & 0xff));
							phid->deviceType = (char *)Phid_DeviceName[Phid_Device_Def[i].pdd_did];

							CPhidget_setStatusFlag(phid, PHIDGET_ATTACHED_FLAG);
							phid->deviceIDSpec = i;
							phid->deviceID = Phid_Device_Def[i].pdd_did;
							phid->ProductID = dev->descriptor.idProduct;
							phid->VendorID = dev->descriptor.idVendor;
							
							if (dev->descriptor.iSerialNumber) {
								char string[256];
								memset(string, 0, 256);
								if((ret = usb_get_string_simple(udev, dev->descriptor.iSerialNumber, string, sizeof(string))) < 0)
								{
									LOG(PHIDGET_LOG_ERROR, "usb_get_string_simple failed with error code: %d \"%s\"", ret, strerror(-ret));
									LOG(PHIDGET_LOG_INFO, "This usually means you need to run as root");
									if((ret = usb_close(udev)) < 0)
									{
										LOG(PHIDGET_LOG_ERROR, "usb_close failed with error code: %d \"%s\"", ret, strerror(-ret));
									}
									goto next;
								}
								else
								{
									phid->serialNumber = atol(string);
								}
							}
							phid->specificDevice = TRUE;
							phid->attr = Phid_Device_Def[i].pdd_attr;
							
							phid->CPhidgetFHandle = (char *)malloc(3);
							memcpy(phid->CPhidgetFHandle, dev->filename, 3);
							
							LOG(PHIDGET_LOG_INFO, "New device in CUSBBuildList: %s", (char *)phid->CPhidgetFHandle);

							if((ret = usb_close(udev)) < 0)
							{
								LOG(PHIDGET_LOG_ERROR, "usb_close failed with error code: %d \"%s\"", ret, strerror(-ret));
							}
							CList_addToList((CListHandle *)curList, phid, CPhidget_areEqual);
						} //if(udev)
						else
						{
							if((ret = usb_close(udev)) < 0)
							{
								LOG(PHIDGET_LOG_ERROR, "usb_close failed with error code: %d \"%s\"", ret, strerror(-ret));
							}
						}
					} //vendor, product ids match
				} /* iterate over phidget device table */
      		} //!found
next: ;
    	} /* iterate over USB devices */
  	} /* iterate over USB busses */
    return EPHIDGET_OK;
}

void CUSBCleanup(void)
{
	usb_cleanup();
}
/* 
	CUSBOpenHandle takes a CPhidgetInfo structure, with 
	ProductID/VendorID/SerialNumber filled in.

	Serial number is always filled in.
*/
int CUSBOpenHandle(CPhidgetHandle phid)
{
	int idVendor;
	int idProduct;
	int serial = 0;
	int i,ret;
	struct usb_bus *bus;
	struct usb_device *dev;
	usb_dev_handle *udev;
	
	usb_init();
	if((ret = usb_find_busses()) < 0)
	{
		LOG(PHIDGET_LOG_ERROR, "usb_find_busses failed with error code: %d \"%s\"", ret, strerror(-ret));
	}
	if((ret = usb_find_devices()) < 0)
	{
		LOG(PHIDGET_LOG_ERROR, "usb_find_devices failed with error code: %d \"%s\"", ret, strerror(-ret));
	}

	for (bus = usb_busses; bus; bus = bus->next) {
		for (dev = bus->devices; dev; dev = dev->next) {
			for (i = 1; i<PHIDGET_DEVICE_COUNT; i++) {
				if (Phid_Device_Def[i].pdd_did == phid->deviceID) {
					idVendor = Phid_Device_Def[i].pdd_vid;
					idProduct = Phid_Device_Def[i].pdd_pid;
					if ((dev->descriptor.idVendor == idVendor) && (dev->descriptor.idProduct == idProduct)) {
						/* the vend/prod matches! */
						udev = usb_open(dev);
						if (udev) {
							serial = -1;
							if (dev->descriptor.iSerialNumber) {
								char string[256];
								if((ret = usb_get_string_simple(udev, dev->descriptor.iSerialNumber, string, sizeof(string))) < 0)
								{
									LOG(PHIDGET_LOG_WARNING, "usb_get_string_simple failed with error code: %d \"%s\"", ret, strerror(-ret));
									LOG(PHIDGET_LOG_INFO, "This usually means you need to run as root");
									if((ret = usb_close(udev)) < 0)
									{
										LOG(PHIDGET_LOG_ERROR, "usb_close failed with error code: %d \"%s\"", ret, strerror(-ret));
									}
									goto next;
								}
								else
								{
									serial = atol(string);
								}
							}
							if (serial == phid->serialNumber) {
								/*	On Linux, the HID driver likes to claim Phidgets - we can disconnect it here.
									Maybe the user has installed the kernel drivers for the interface kit or servo - disconnect them too (does this work)
									Note this is Linux Only - PM
									Note that we don't need to claim it if usbfs has it - this is what libusb uses. */	
#ifdef LIBUSB_HAS_DETACH_KERNEL_DRIVER_NP
								char name[32];
								if((ret = usb_get_driver_np(udev, Phid_Device_Def[i].pdd_iid, name, 32)) < 0)
								{
									LOG(PHIDGET_LOG_WARNING, "usb_get_driver_np failed with error code: %d \"%s\"", ret, strerror(-ret));
								}
								else
								{
									LOG(PHIDGET_LOG_INFO, "Kernel driver name: %s", name);
									if(strncmp(name, "usbfs", 5)) //not usbfs
									{
										if((ret = usb_detach_kernel_driver_np(udev, Phid_Device_Def[i].pdd_iid)) < 0)
										{
											LOG(PHIDGET_LOG_WARNING, "usb_detach_kernel_driver_np failed with error code: %d \"%s\"", ret, strerror(-ret));
										}
										else
										{
											LOG(PHIDGET_LOG_INFO, "Successfully detached kernel driver: %s", name);
										}
									}
								}
#endif
								if((ret = usb_claim_interface(udev, Phid_Device_Def[i].pdd_iid)) < 0)
								{
									LOG(PHIDGET_LOG_WARNING, "usb_claim_interface failed with error code: %d \"%s\"", ret, strerror(-ret));
									if((ret = usb_close(udev)) < 0)
									{
										LOG(PHIDGET_LOG_ERROR, "usb_close failed with error code: %d \"%s\"", ret, strerror(-ret));
									}
								}
								else
								{
									/* the serialnum is okay */
							
									phid->deviceHandle = (HANDLE)udev;
									phid->deviceIDSpec = i;
									phid->deviceType = (char *)Phid_DeviceName[Phid_Device_Def[i].pdd_did];
			
									phid->ProductID = idProduct;
									phid->VendorID = idVendor;
									if (dev->descriptor.bcdDevice < 0x100)
										phid->deviceVersion = dev->descriptor.bcdDevice * 100;
									else
										phid->deviceVersion = ((dev->descriptor.bcdDevice >> 8) * 100) + ((dev->descriptor.bcdDevice & 0xff));			
									phid->serialNumber = serial;
			
									if((ret = CUSBGetDeviceCapabilities(phid, dev, udev)))
									{
										LOG(PHIDGET_LOG_ERROR, "CUSBGetDeviceCapabilities returned nonzero code: %d", ret);
									}
	
									phid->attr = Phid_Device_Def[i].pdd_attr;
			
									if ((Phid_Device_Def_WorkAroundA[i][0]) && (Phid_Device_Def_WorkAroundA[i][1] >= dev->descriptor.bcdDevice)) {
										unsigned char buffer[8];
										ZEROMEM(buffer,8);
										if(!(ret = CUSBSendPacket((CPhidgetHandle)phid, buffer)))
										{
											LOG(PHIDGET_LOG_ERROR, "CUSBSendPacket returned nonzero code: %d", ret);
										}
									}
									return EPHIDGET_OK;
								} /* usb_claim_interface */
	      					} /* serial matches */
	      					else
							{
								if((ret = usb_close(udev)) < 0)
								{
									LOG(PHIDGET_LOG_ERROR, "usb_close failed with error code: %d \"%s\"", ret, strerror(-ret));
								}
							}
	    				} /* udev open */
						else
						{
							if((ret = usb_close(udev)) < 0)
							{
								LOG(PHIDGET_LOG_ERROR, "usb_close failed with error code: %d \"%s\"", ret, strerror(-ret));
							}
							LOG(PHIDGET_LOG_WARNING, "usb_open failed - bad permission or what?");
						}
	  				} /* vendor/product match */
				} /* deviceID matches in table */
      		} /* iterate over phidget device table */
next: ;
    	} /* iterate over USB devices */
  	} /* iterate over USB busses */
  	return EPHIDGET_NOTFOUND;
}
