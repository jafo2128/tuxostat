#ifndef _ZEROCONF_H_
#define _ZEROCONF_H_

int zeroconf_advertise_ws();
int zeroconf_unadvertise_phidget(CPhidgetHandle phid);
int zeroconf_advertise_phidget(CPhidgetHandle phid);

int InitializeZeroconf();
int UninitializeZeroconf();

#endif
