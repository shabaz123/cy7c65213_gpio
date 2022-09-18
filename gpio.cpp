/*************************************************************
 * gpio.cpp
 * rev 1 - Sep 2022 - shabaz
 *************************************************************/


// gpio.cpp : Defines the entry point for the console application.
// 

#include "stdafx.h"

#include <stdio.h>
#include <windows.h>
#include <dbt.h>
#include <conio.h>
#include "..\..\..\library\cyusbserial\CyUSBSerial.h"


// ************** defines ********************
// Define VID & PID
#define VID 0x04B4
#define PID 0x0003 

// ********** global variables *****************
//Variable to store cyHandle of the selected device
CY_HANDLE cyHandle; 
//Varible to capture return values from USB Serial API 
CY_RETURN_STATUS cyReturnStatus; 

//CY_DEVICE_INFO provides additional details of each device such as product Name, serial number etc..
CY_DEVICE_INFO cyDeviceInfo, cyDeviceInfoList[16]; 

//Structure to store VID & PID defined in CyUSBSerial.h
CY_VID_PID cyVidPid;

//Variables used by application
UINT8 cyNumDevices;
unsigned char deviceID[16];
unsigned char gpioNumber;
unsigned char value;
int sleepms;

// **************** functions *******************
/*
Function Name: int GPIOTasks(int deviceNumber)
Arguments:
deviceNumber - The device number identified during the enumeration process
Retrun Code: returns falure code of USB-Serial API, -1 for local failures.
*/
int GPIOTasks(int deviceNumber)
{        
    CY_DATA_BUFFER cyDatabuffer;    
    unsigned char buffer[4096];
    unsigned char readvalue;

    CY_RETURN_STATUS rStatus;    
    int interfaceNum = 0, count = 1000;
    unsigned char bytesPending = 0;

    cyDatabuffer.buffer = buffer;

    //Obtain handle for a USB-Serial device
    printf("\nOpening device %d...\n",deviceNumber);
    rStatus = CyOpen (deviceNumber, interfaceNum, &cyHandle);

    if (rStatus){
        printf ("Failed to open USB-Serial device\n");
        return -1;
        }

    // Note: GPIO must have been previously configured using Cypress USB-Serial Configuration Utility,
    // otherwise this code can't do its thing.

    printf("Setting gpio %d to %d...\n", gpioNumber, value);
    rStatus = CySetGpioValue (cyHandle, gpioNumber, value);    
    if (rStatus){
        printf ("Set GPIO failed.\n");
        return -1;
        }
    printf("Sleeping..\n");
    Sleep(sleepms);

    printf("Verification read from gpio...\n");
    rStatus = CyGetGpioValue (cyHandle, gpioNumber, &readvalue);    
    if (rStatus){
        printf ("Get GPIO value failed\n");
        return -1;
        }
    printf ("Read value is %d \n", readvalue);

    // Close the cyHandle to USB-Serial device
    return CyClose (cyHandle); 

}
/*
Function Name: int FindDeviceAtSCB0()           
Arguments: None
Return Code: returns -1, if no device is present or deviceIndex of the device.
*/
int FindDeviceAtSCB0()
{
    CY_VID_PID cyVidPid;

    cyVidPid.vid = VID; //Defined as macro
    cyVidPid.pid = PID; //Defined as macro

    //Array size of cyDeviceInfoList is 16 
    cyReturnStatus = CyGetDeviceInfoVidPid (cyVidPid, deviceID, (PCY_DEVICE_INFO)&cyDeviceInfoList, &cyNumDevices, 16);

    int deviceIndexAtSCB0 = -1;
    for (int index = 0; index < cyNumDevices; index++){                
        printf ("\nNumber of interfaces: %d\n \
                Vid: 0x%X \n\
                Pid: 0x%X \n\
                Serial name is: %s\n\
                Manufacturer name: %s\n\
                Product Name: %s\n\
                SCB Number: 0x%X \n\
                Device Type: %d \n\
                Device Class: %d\n\n\n",
                cyDeviceInfoList[index].numInterfaces,                  
                cyDeviceInfoList[index].vidPid.vid,
                cyDeviceInfoList[index].vidPid.pid,
                cyDeviceInfoList[index].serialNum,
                cyDeviceInfoList[index].manufacturerName,
                cyDeviceInfoList[index].productName,
                cyDeviceInfoList[index].deviceBlock,
                cyDeviceInfoList[index].deviceType[0],
                cyDeviceInfoList[index].deviceClass[0]);

        // Find the device at device index at SCB0
        // shabaz: set it to use any device found!
        if (1) // (cyDeviceInfoList[index].deviceBlock == <value>
            {
            deviceIndexAtSCB0 = index;
            }
        }
    return deviceIndexAtSCB0;
}

// *********** _tmain (main) function **********************
int _tmain(int argc, _TCHAR* argv[])
{
    int ret;
    char paramstr[128];
    int deviceIndexAtSCB0;
    int v;
    int i;

    if (argc < 5) {
        printf("Error, gpio expected parameters are <output|input> <gpnum> <0|1> <msec>\n");
        return(1);
    }

    // first parameter is currently ignored, only output is supported.

    for (i = 2; i <= 4; i++) {
        wcstombs(paramstr, argv[i], wcslen(argv[i]) + 1);
        ret = sscanf((const char*)paramstr, "%d", &v);
        switch (i) {
        case 2:
            gpioNumber = (unsigned char)v;
            break;
        case 3:
            value = (unsigned char)v;
            break;
        case 4:
            sleepms = v;
            break;
        default:
            break;
        }
    }

    printf("output: setting gpionum %d to %d, then %d msec delay\n", gpioNumber, value, sleepms);
    //NOTE: SCB Index is not needed for accessing GPIO. But as this function call ensures 
    //access to right SCB, this example uses this code.
    deviceIndexAtSCB0 = FindDeviceAtSCB0();

    //Open the device at index deviceIndexAtSCB0
    if (deviceIndexAtSCB0 >= 0)
        {        
        //Assuming that device at "index" is USB-Serial device        
        //Device Open, Close, Configuration, Data operations are handled in the function GPIOTasks
        int status = GPIOTasks(deviceIndexAtSCB0);

        if (status == CY_SUCCESS)
            {
            printf("gpio task success.\n");
            }
        else
            {
            printf("gpio task failed.\n");
            return(1);
            }

        } //cyNumDevices > 0 && cyReturnStatus == CY_SUCCESS

    return 0;
}
