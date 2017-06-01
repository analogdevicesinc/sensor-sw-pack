/*!
 *****************************************************************************
   @file:    adxl362_app.cpp

   @brief:   ADXL362 accelerometer example

   @details: Example demonstrating the accelerometer functionality
  -----------------------------------------------------------------------------

Copyright (c) 2017 Analog Devices, Inc.

All rights reserved.

Redistribution and use in source and binary forms, with or without modification,
are permitted provided that the following conditions are met:
  - Redistributions of source code must retain the above copyright notice,
    this list of conditions and the following disclaimer.
  - Redistributions in binary form must reproduce the above copyright notice,
    this list of conditions and the following disclaimer in the documentation
    and/or other materials provided with the distribution.
  - Modified versions of the software must be conspicuously marked as such.
  - This software is licensed solely and exclusively for use with processors
    manufactured by or for Analog Devices, Inc.
  - This software may not be combined or merged with other code in any manner
    that would cause the software to become subject to terms and conditions
    which differ from those listed here.
  - Neither the name of Analog Devices, Inc. nor the names of its
    contributors may be used to endorse or promote products derived
    from this software without specific prior written permission.
  - The use of this software may or may not infringe the patent rights of one
    or more patent holders.  This license does not release you from the
    requirement that you obtain separate licenses from these patent holders
    to use this software.

THIS SOFTWARE IS PROVIDED BY ANALOG DEVICES, INC. AND CONTRIBUTORS "AS IS" AND ANY
EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, NON-INFRINGEMENT,
TITLE, MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE DISCLAIMED. IN
NO EVENT SHALL ANALOG DEVICES, INC. OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT,
INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, PUNITIVE OR CONSEQUENTIAL DAMAGES
(INCLUDING, BUT NOT LIMITED TO, DAMAGES ARISING OUT OF CLAIMS OF INTELLECTUAL
PROPERTY RIGHTS INFRINGEMENT; PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS
OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY
THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING
NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE,
EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.

*****************************************************************************/


#include "adxl362_app.h"
#include <stdio.h>
#include <common/adi_timestamp.h>

using namespace adi_sensor_swpack;

/* Peripheral advertisement mode */
#define PERIPHERAL_ADV_MODE      ((ADI_BLE_GAP_MODE)(ADI_BLE_GAP_MODE_CONNECTABLE | ADI_BLE_GAP_MODE_DISCOVERABLE))

#if (ADI_APP_USE_BLUETOOTH == 1u)
/* Global Data */
static bool               gbConnected;
static ADI_BLE_GAP_MODE   geMode;
static ADI_DATA_PACKET    gSensorData;

/* Local Functions used when Bluetooth is enabled */
static void ApplicationCallback(void * pCBParam, uint32_t Event, void * pArg);
static void InitBluetoothLowEnergy(void);
static void SetAdvertisingMode(void);
static void AxlBluetoothMode(Accelerometer *pAxl);
#else
static void AxlStandaloneMode(Accelerometer *pAxl);
#endif

/* Local Functions */
static void InitSystem(void);
static void Trap(void);

/*!
 * @brief      Main
 *
 * @details    Application entry point.
 *
 * @param [in] argc : Number of arguments (unused)
 *
 * @param [in] argv : Arguments (unused)
 *
 */
int main(int argc, char *argv[])
{
    ADXL362         adxl362;
    Accelerometer   *pAxl = &adxl362;
    SENSOR_RESULT   eSensorResult;

    /* Initialize the system */
    InitSystem();

    /* Open Accelerometer */
    eSensorResult = pAxl->open();

    if(eSensorResult != SENSOR_ERROR_NONE)
    {
        PRINT_SENSOR_ERROR(eSensorResult);
        Trap();
    }

    /* Start measurement */
    eSensorResult = pAxl->start();

    if (pAxl->start() != SENSOR_ERROR_NONE)
    {
        PRINT_SENSOR_ERROR(eSensorResult);
        Trap();
    }
#if(ADI_APP_USE_BLUETOOTH == 1u)
    AxlBluetoothMode(pAxl);
#else
    AxlStandaloneMode(pAxl);
#endif
}

/*!
 * @brief      Initializes the system
 *
 * @details    This function is responsible for initializing the pinmuxing, power service
 *             and bluetooth subsystem. It also initializes the realtime clock for to timestamp
 *             the outgoing sensor data packets.
 */
static void InitSystem(void)
{
    ADI_PWR_RESULT  ePwr;

    /* Explicitly disable the watchdog timer */
    *pREG_WDT0_CTL = 0x0u;

    /* Pinmux */
    adi_initpinmux();

    /* Initialize clocks */
    ePwr = adi_pwr_Init();
    PRINT_ERROR("Error initializing the power service.\r\n", ePwr, ADI_PWR_SUCCESS);

    ePwr = adi_pwr_SetClockDivider(ADI_CLOCK_HCLK, 1u);
    PRINT_ERROR("Error configuring the core clock.\r\n", ePwr, ADI_PWR_SUCCESS);

    ePwr = adi_pwr_SetClockDivider(ADI_CLOCK_PCLK, 1u);
    PRINT_ERROR("Error configuring the peripheral clock.\r\n", ePwr, ADI_PWR_SUCCESS);

    /* Init timestamping */
    INIT_TIME();

#ifndef ADI_DEBUG
    /* Initialize UART redirection in release mode only */
    common_Init();
#endif

    PRINTF(("Starting ADXL362 accelerometer demo application\r\n"));

}

/*!
 * @brief      Trap function    
 *
 * @details    In case of catastrophic errors this function is called to block
 *             infinitely. 
 */
static void Trap()
{
    while(1);
}


#if (ADI_APP_USE_BLUETOOTH == 1u)
/*!
 * @brief      Bluetooth Accelerometer demo
 *
 * @details    Sends data over bluetooth to the connected host. 
 */
static void AxlBluetoothMode(Accelerometer *pAxl)
{
    ADI_BLER_RESULT     eResult;
    uint32_t            nTime = 0ul;
    ADI_BLER_CONN_INFO  sConnInfo;

    /* Initialize Bluetooth */
    InitBluetoothLowEnergy();

    /* Bit:0 Sensor Data Packet Bits:1-7 : Sensor ID */
    gSensorData.nPacketHeader = ADI_SET_HEADER(ADI_DATA_PACKET_TYPE, ADI_ACCELEROMETER_ID);

    /* Set Sensor Type */
    gSensorData.eSensorType = ADI_ACCELEROMETER_2G_TYPE;

    /* WHILE(forever) */
    while(1u)
    {
        /* Dispatch events for two seconds - they will arrive in the application callback */
        eResult = adi_ble_DispatchEvents(ADI_APP_DISPATCH_TIMEOUT);
        PRINT_ERROR("Error dispatching events to the callback.\r\n", eResult, ADI_BLER_SUCCESS);

        /* If connected, send data */
        if (gbConnected == true)
        {
            adi_ble_GetConnectionInfo(&sConnInfo);

            /* Get timestamp */
            nTime = GET_TIME();
            memcpy(&gSensorData.aTimestamp,&nTime,4u);

            /* Get x,y,x accelerometer data */
            pAxl->getXYZ((uint8_t*)&gSensorData.aPayload,6u);

            eResult = adi_radio_DE_SendData(sConnInfo.nConnHandle, DATAEXCHANGE_PACKET_SIZE, (uint8_t*)&gSensorData);
            PRINT_ERROR("Error sending the data.\r\n", eResult, ADI_BLER_SUCCESS);
        }
        /* If disconnected switch to advertising mode */
        else
        {
            if (geMode != PERIPHERAL_ADV_MODE)
            {
                SetAdvertisingMode();
            }
        }
    } /* ENDWHILE */
}

/*!
 * @brief      Set Advertising Mode
 *
 * @details    Helper function to avoid repeated code in main.
 *
 */
static void SetAdvertisingMode(void)
{
    ADI_BLER_RESULT eResult;

    eResult = adi_radio_SetMode(PERIPHERAL_ADV_MODE, 0u, 0u);
    PRINT_ERROR("Error setting the mode.\r\n", eResult, ADI_BLER_SUCCESS);

    eResult = adi_ble_WaitForEventWithTimeout(GAP_EVENT_MODE_CHANGE, 5000u);
    PRINT_ERROR("Error waiting for GAP_EVENT_MODE_CHANGE.\r\n", eResult, ADI_BLER_SUCCESS);

    eResult = adi_radio_GetMode(&geMode);
    PRINT_ERROR("Error getting the mode.\r\n", eResult, ADI_BLER_SUCCESS);

    if (geMode != PERIPHERAL_ADV_MODE)
    {
        PRINTF(("Error in SetAdvertisingMode.\r\n"));
    }
}

/*!
 * @brief      Initializes the bluetooth
 *
 * @details    Data Exchange profile is initialized to send
 *             data to the connected central device.
 */
static void InitBluetoothLowEnergy(void)
{
    ADI_BLER_RESULT eResult;
    uint8_t *       aDataExchangeName = (unsigned char *) "ADXL362 Demo";

    /* Initialize radio and framework layer */
    eResult = adi_ble_Init(ApplicationCallback, NULL);
    PRINT_ERROR("Error initializing the radio.\r\n", eResult, ADI_BLER_SUCCESS);

    /* Configure radio */
    eResult = adi_radio_RegisterDevice(ADI_BLE_ROLE_PERIPHERAL);
    PRINT_ERROR("Error registering the radio.\r\n", eResult, ADI_BLER_SUCCESS);

    eResult = adi_radio_SetLocalBluetoothDevName(aDataExchangeName, strlen((const char *) aDataExchangeName), 0u, 0u);
    PRINT_ERROR("Error setting local device name.\r\n", eResult, ADI_BLER_SUCCESS);

    SetAdvertisingMode();

    /* Initialize data exchange profile */
    eResult = adi_radio_Register_DataExchangeServer();
    PRINT_ERROR("Error registering data exchange server.\r\n", eResult, ADI_BLER_SUCCESS);

    /* Now enter infinite loop waiting for connection and then data exchange events */
    PRINTF(("Waiting for connection. Initiate connection on central device please.\r\n"));
}

/*!
 * @brief      Application Callback
 *
 * @details    Called by the framework layer (adi_ble_noos.c) when an event occurs that the application did NOT
 *             explicity expect by calling #adi_ble_WaitForEventWithTimeout.
 *
 * @param [in] pCBParam : Callback parameter (unused)
 *
 * @param [in] Event : Event of type #ADI_BLER_EVENT.
 *
 * @param [in] pArg : Callback argument (unused)
 *
 * @note       The application should NOT call other radio functions (adi_ble_radio.c) from this callback that issue
 *             a command to the radio. The application may call radio functions that simply extract data from the
 *             companion module, these are located below #adi_ble_GetEvent in adi_ble_radio.c. Ideally this callback
 *             should just be used for flags and semaphores.
 */
static void ApplicationCallback(void * pCBParam, uint32_t Event, void * pArg)
{
    switch(Event)
    {
        case GAP_EVENT_CONNECTED:
            PRINTF(("Connected!\r\n"));
            gbConnected = true;
            break;

        case GAP_EVENT_DISCONNECTED:
            PRINTF(("Disconnected!\r\n"));
            geMode      = ADI_BLE_GAP_MODE_NOTCONNECTABLE;
            gbConnected = false;
            break;

        case DATA_EXCHANGE_RX_EVENT:
            PRINTF(("Data received!\r\n"));
            break;

        case DATA_EXCHANGE_TX_COMPLETE:
            PRINTF(("Data sent!\r\n"));
            break;

        case GAP_EVENT_MODE_CHANGE:
            PRINTF(("GAP mode changed.\r\n"));
            break;

        case GAP_EVENT_CONNECTION_UPDATED:
            PRINTF(("Connection interval updated.\r\n"));
            break;

        case BLE_RADIO_ERROR_READING:
        	/* If you want to enable this print statement, please be aware that the first 
        	 * packet sent from the radio on startup will cause this error. It is a known bug 
        	 * and will not have any adverse effects on the application.
			 *
        	 *PRINTF(("Failed to read a packet from the radio.\r\n"));
			 *
        	 */ 
            break;

        case BLE_RADIO_ERROR_PARSING:
            PRINTF(("Failed to parse a packet from the radio.\r\n"));
            break;

        case BLE_RADIO_ERROR_PROCESSING:
            PRINTF(("Failed to process a packet from the radio.\r\n"));
            break;

        default:
            PRINTF(("Unexpected event received.\r\n"));
            break;
    }
}
#else
/*!
 * @brief      Standalone Accelerometer demo
 *
 * @details    Standalone mode just prints the x,y,z values to the console.
 */
static void AxlStandaloneMode(Accelerometer *pAxl)
{
    int16_t x,y,z;

    /* WHILE(forever) */
    while(1u)
    {
        /* Get x,y,x accelerometer data */
        pAxl->getX((uint8_t*)&x,2u);
        pAxl->getY((uint8_t*)&y,2u);
        pAxl->getZ((uint8_t*)&z,2u);

        PRINTF(("\tX value = %d \t\t\r\n", x));
        PRINTF(("\tY value = %d \t\t\r\n", y));
        PRINTF(("\tZ value = %d \t\t\r\n", z));

        /* delay */
        for(volatile uint32_t i = 0; i < 0xfff; i++);
    } /* ENDWHILE */
}
#endif



