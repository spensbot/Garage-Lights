/*
  ==============================================================================

    UsbDmxPro.h
    Created: 5 Feb 2018 10:38:48pm
    Author:  Spenser

  ==============================================================================
*/

//Add the FTDI dynamic library to the project. 
#pragma comment(lib, "C:/Users/Spenser/Documents/JUCE Docs/Projects/Custom Light Show/Source/ftd2xx.lib")

#include "pro_driver.h"
#include "../JuceLibraryCode/JuceHeader.h"
#include <chrono>

#define DMX_DATA_LENGTH 513 // Includes the start code 

typedef std::chrono::high_resolution_clock Clock;

class UsbDmxPro : public Timer{

public:

	UsbDmxPro(AudioProcessorValueTreeState* _params, boolean* _blackout) : params(_params), blackout(_blackout) {
		connected = false;
		getNumFtdiDevices();
		
		memset(blackoutDmx, 0, DMX_DATA_LENGTH);
		myDmx[0] = 0;
	}

	~UsbDmxPro() {
		stopTimer();
	}

	// old school globals
	DMXUSBPROParamsType PRO_Params;
	FT_HANDLE device_handle = NULL;
	uint32_t length = DMX_DATA_LENGTH;
	FT_STATUS ftStatus;

	//Device Parameters
	int numDevices;
	uint8_t major_ver, minor_ver, build_ver;
	uint8_t latencyTimer;
	int VersionMSB;
	int VersionLSB;
	int BreakTime;
	int MABTime;
	int refreshRate;
	boolean connected;
	boolean* blackout;
	AudioProcessorValueTreeState* params;
	unsigned char myDmx[DMX_DATA_LENGTH];
	unsigned char blackoutDmx[DMX_DATA_LENGTH];
	


	//Open the connected usb device, set communication timeouts, and get connection info.
	//Set connected to false if anything fails, 
	void connect() {
		int response;
		unsigned char byte = 0;
		int size = 0;

		ftStatus = FT_Open(0, &device_handle);
		if (ftStatus != FT_OK)
		{
			connected = false;
		}
		else {
			connected = true;
		}

		//Get connected device latency.
		ftStatus = FT_GetLatencyTimer(device_handle, (PUCHAR)&latencyTimer);

		// SET Default Read & Write Timeouts (in micro sec ~ 100)
		FT_SetTimeouts(device_handle, 120, 100);
		//Purge the buffer.
		FT_Purge(device_handle, FT_PURGE_RX);

		// Send Get Widget Parameters to get Device Info
		response = FTDI_SendData(GET_WIDGET_PARAMS, (unsigned char *)&size, 2);
		if (response == NO_RESPONSE)
		{
			FT_Purge(device_handle, FT_PURGE_TX);
			response = FTDI_SendData(GET_WIDGET_PARAMS, (unsigned char *)&size, 2);
			if (response == NO_RESPONSE)
			{
				FTDI_ClosePort();
				connected = false;
			}
		}

		// Recive Widget Response packet
		response = FTDI_ReceiveData(GET_WIDGET_PARAMS_REPLY, (unsigned char *)&PRO_Params, sizeof(DMXUSBPROParamsType));
		if (response == NO_RESPONSE)
		{
			response = FTDI_ReceiveData(GET_WIDGET_PARAMS_REPLY, (unsigned char *)&PRO_Params, sizeof(DMXUSBPROParamsType));
			if (response == NO_RESPONSE)
			{
				FTDI_ClosePort();
				connected = false;
			}
		}

		// Firmware  Version
		VersionMSB = PRO_Params.FirmwareMSB;
		VersionLSB = PRO_Params.FirmwareLSB;
		// Display All PRO Parametrs & Info avialable
		BreakTime = (int)(PRO_Params.BreakTime * 10.67) + 100;
		MABTime = (int)(PRO_Params.MaBTime * 10.67);
		refreshRate = PRO_Params.RefreshRate;

		for (int i = 1; i <= 512; i++) {
			myDmx[i] = *params->getRawParameterValue((String)i);
		}

		startTimerHz(40);
	}

	void disconnect() {
		FTDI_PurgeBuffer();
		FTDI_ClosePort();
		connected = false;
		getNumFtdiDevices();
	}

	boolean sendDmx(boolean blackout) {
		int response;

		if (blackout) {
			response = FTDI_SendData(SET_DMX_TX_MODE, blackoutDmx, DMX_DATA_LENGTH);		    
		}
		else {
			response = FTDI_SendData(SET_DMX_TX_MODE, myDmx, DMX_DATA_LENGTH);
		}
		
		// check response from Send function
		if (response < 0)
		{
			FTDI_ClosePort();
			connected = false;
			return false;
		}
		return true;
	}

	String getConnectionInfoString() {
		String part1 = "Connection Successful!!! Firmware Version: " + (String)VersionMSB + "." + (String)VersionLSB;
		String part2 = "\nConnection Latency: " + (String)latencyTimer;
		String part3 = "\nRefresh Rate: " + (String)refreshRate;
		return part1 + part2 + part3;
	}


private:

	void timerCallback() {
		long long duration;
		long long averageDuration;
		int iterations = 1;
		if (connected) {
			auto start = Clock::now();
			for (int i = 0; i < iterations; i++) {
				
				sendDmx(*blackout);
				
			}
			auto end = Clock::now();
			duration = std::chrono::duration_cast<std::chrono::milliseconds>(end - start).count();
			averageDuration = duration / iterations;
		}
	}

	void getNumFtdiDevices() {
		numDevices = FTDI_ListDevices();
	}

	/* Function : FTDI_ClosePort
	* Author	: ENTTEC
	* Purpose  : Closes the Open DMX USB PRO Device Handle
	* Parameters: none
	**/
	void FTDI_ClosePort()
	{
		if (device_handle != NULL)
			FT_Close(device_handle);
	}

	/* Function : FTDI_PurgeBuffer
	* Author	: ENTTEC
	* Purpose  : Clears the buffer used internally by the PRO
	* Parameters: none
	**/
	void FTDI_PurgeBuffer()
	{
		FT_Purge(device_handle, FT_PURGE_TX);
		FT_Purge(device_handle, FT_PURGE_RX);
	}

	/* Function : FTDI_ListDevices
	* Author	: ENTTEC
	* Purpose  : Returns the no. of PRO's conneced to the PC
	* Parameters: none
	**/
	int FTDI_ListDevices()
	{
		FT_STATUS ftStatus;
		DWORD numDevs = 0;
		ftStatus = FT_ListDevices((PVOID)&numDevs, NULL, FT_LIST_NUMBER_ONLY);
		if (ftStatus == FT_OK)
			return numDevs;
		return NO_RESPONSE;
	}

	/* Function : FTDI_SendData
	* Author	: ENTTEC
	* Purpose  : Send Data (DMX or other packets) to the PRO
	* Parameters: Label, Pointer to Data Structure, Length of Data
	**/
	int FTDI_SendData(int label, unsigned char *data, int length)
	{
		unsigned char end_code = DMX_END_CODE;
		FT_STATUS res = 0;
		DWORD bytes_to_write = length;
		DWORD bytes_written = 0;
		HANDLE event = NULL;
		int size = 0;
		// Form Packet Header
		unsigned char header[DMX_HEADER_LENGTH];
		header[0] = DMX_START_CODE;
		header[1] = label;
		header[2] = length & OFFSET;
		header[3] = length >> BYTE_LENGTH;
		// Write The Header
		res = FT_Write(device_handle, (unsigned char *)header, DMX_HEADER_LENGTH, &bytes_written);
		if (bytes_written != DMX_HEADER_LENGTH) return  NO_RESPONSE;
		// Write The Data
		res = FT_Write(device_handle, (unsigned char *)data, length, &bytes_written);
		if (bytes_written != length) return  NO_RESPONSE;
		// Write End Code
		res = FT_Write(device_handle, (unsigned char *)&end_code, ONE_BYTE, &bytes_written);
		if (bytes_written != ONE_BYTE) return  NO_RESPONSE;
		if (res == FT_OK)
			return TRUE;
		else
			return FALSE;
	}

	/* Function : FTDI_ReceiveData
	* Author	: ENTTEC
	* Purpose  : Receive Data (DMX or other packets) from the PRO
	* Parameters: Label, Pointer to Data Structure, Length of Data
	**/
	int FTDI_ReceiveData(int label, unsigned char *data, unsigned int expected_length)
	{

		FT_STATUS res = 0;
		DWORD length = 0;
		DWORD bytes_to_read = 1;
		DWORD bytes_read = 0;
		unsigned char byte = 0;
		HANDLE event = NULL;
		char buffer[600];
		// Check for Start Code and matching Label
		while (byte != label)
		{
			while (byte != DMX_START_CODE)
			{
				res = FT_Read(device_handle, (unsigned char *)&byte, ONE_BYTE, &bytes_read);
				if (bytes_read == NO_RESPONSE) return  NO_RESPONSE;
			}
			res = FT_Read(device_handle, (unsigned char *)&byte, ONE_BYTE, &bytes_read);
			if (bytes_read == NO_RESPONSE) return  NO_RESPONSE;
		}
		// Read the rest of the Header Byte by Byte -- Get Length
		res = FT_Read(device_handle, (unsigned char *)&byte, ONE_BYTE, &bytes_read);
		if (bytes_read == NO_RESPONSE) return  NO_RESPONSE;
		length = byte;
		res = FT_Read(device_handle, (unsigned char *)&byte, ONE_BYTE, &bytes_read);
		if (res != FT_OK) return  NO_RESPONSE;
		length += ((uint32_t)byte) << BYTE_LENGTH;
		// Check Length is not greater than allowed
		if (length > DMX_PACKET_SIZE)
			return  NO_RESPONSE;
		// Read the actual Response Data
		res = FT_Read(device_handle, buffer, length, &bytes_read);
		if (bytes_read != length) return  NO_RESPONSE;
		// Check The End Code
		res = FT_Read(device_handle, (unsigned char *)&byte, ONE_BYTE, &bytes_read);
		if (bytes_read == NO_RESPONSE) return  NO_RESPONSE;
		if (byte != DMX_END_CODE) return  NO_RESPONSE;
		// Copy The Data read to the buffer passed
		memcpy(data, buffer, expected_length);
		return TRUE;
	}

	/*
	Read a DMX packet
	*/
	uint8_t FTDI_RxDMX(uint8_t label, unsigned char *data, uint32_t* expected_length)
	{

		FT_STATUS res = 0;
		DWORD length = 0;
		DWORD bytes_to_read = 1;
		DWORD bytes_read = 0;
		unsigned char byte = 0;
		unsigned char header[3];
		HANDLE event = NULL;
		unsigned char buffer[600];
		// Check for Start Code and matching Label
		while (byte != DMX_START_CODE)
		{
			res = FT_Read(device_handle, (unsigned char *)&byte, ONE_BYTE, &bytes_read);
			if (bytes_read == NO_RESPONSE) return  NO_RESPONSE;
		}
		res = FT_Read(device_handle, header, 3, &bytes_read);
		if (bytes_read == NO_RESPONSE) return  NO_RESPONSE;
		if (header[0] != label) return NO_RESPONSE;
		length = header[1];
		length += ((uint32_t)header[2]) << BYTE_LENGTH;
		length += 1;
		// Check Length is not greater than allowed
		if (length > DMX_PACKET_SIZE + 3)
			return  NO_RESPONSE;
		// Read the actual Response Data
		res = FT_Read(device_handle, buffer, length, &bytes_read);
		if (bytes_read != length) return  NO_RESPONSE;
		// Check The End Code
		if (buffer[length - 1] != DMX_END_CODE) return  NO_RESPONSE;
		*expected_length = (unsigned int)length;
		// Copy The Data read to the buffer passed
		memcpy(data, buffer, *expected_length);
		return TRUE;
	}

};