/*!
 *****************************************************************************
   @file:    template_sensor.c

   @brief:   Data Exchange Template Sensor No Operating System Example

   @details: Example program demonstrating basic use of the Data Exchange
             profile to send random data from the EM9304 to a host.
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


#include "template_sensor_app.h"


extern int32_t adi_initpinmux(void);


static void ApplicationCallback(void * pCBParam, uint32_t Event, void * pArg);
static void SetAdvertisingMode (void);
static void TemplateSensorRead (uint8_t * pBuffer);


static ADI_DATA_PACKET          eDataPacket;
static bool                     gbConnected;
static ADI_BLER_CONN_INFO       sConnInfo;
static ADI_BLE_GAP_MODE         geMode;

#define PERIPHERAL_ADV_MODE      ((ADI_BLE_GAP_MODE)(ADI_BLE_GAP_MODE_CONNECTABLE | ADI_BLE_GAP_MODE_DISCOVERABLE))
#define SENSOR_ID                (1u)


int main(int argc, char *argv[])
{
    ADI_BLER_RESULT      eResult;
    ADI_PWR_RESULT       ePwr;
    uint32_t             nTime;
    uint8_t *            aTemplateSensorName = (unsigned char *)"ADI_BLE_TEMPLATESENSOR";

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
    
    /*Initialize Timer */
    INIT_TIME();

#ifndef ADI_DEBUG
    /* Initialize UART redirection in release mode only */
    common_Init();
#endif

    PRINTF(("Starting Template Sensor Example.\r\n"));

    /* Initialize radio */
    eResult = adi_ble_Init(ApplicationCallback, NULL);
    PRINT_ERROR("Error initializing the radio.\r\n", eResult, ADI_BLER_SUCCESS);

    eResult = adi_radio_RegisterDevice(ADI_BLE_ROLE_PERIPHERAL);
    PRINT_ERROR("Error registering the radio.\r\n", eResult, ADI_BLER_SUCCESS);

    eResult = adi_radio_SetLocalBluetoothDevName(aTemplateSensorName, strlen((const char *) aTemplateSensorName), 0u, 0u);
    PRINT_ERROR("Error setting local device name.\r\n", eResult, ADI_BLER_SUCCESS);

    SetAdvertisingMode();

    /* Initialize data exchange profile */
    eResult = adi_radio_Register_DataExchangeServer();
    PRINT_ERROR("Error registering data exchange server.\r\n", eResult, ADI_BLER_SUCCESS);

    /* Initialize static components of the data packet. For a single "sensor" example these will not change. */
    eDataPacket.nPacketHeader = ADI_SET_HEADER(ADI_DATA_PACKET_TYPE, SENSOR_ID);

    /* The sensor type that is set here is the type of sensor this demo will simulate on the Android application */
    eDataPacket.eSensorType = ADI_APP_SIMULATE_SENSOR_TYPE;

    /* Now enter infinite loop waiting for connection and then data exchange events */
    PRINTF(("Waiting for connection. Initiate connection on central device please.\r\n"));

    /* WHILE(forever) */
    while(1u)
    {
        /* Dispatch events - they will arrive in the application callback */
        eResult = adi_ble_DispatchEvents(ADI_APP_DISPATCH_TIMEOUT);
        PRINT_ERROR("Error dispatching events to the callback.\r\n", eResult, ADI_BLER_SUCCESS);
        
        /* If connected, send data */
        if (gbConnected == true)
        {
            adi_ble_GetConnectionInfo(&sConnInfo);

            /* Fill the sensor data packet according to the sensor packet documentation */
            
            nTime = GET_TIME();
            memcpy(&eDataPacket.aTimestamp ,&nTime,4u);

            TemplateSensorRead((uint8_t*)&eDataPacket.aPayload);

            eResult = adi_radio_DE_SendData(sConnInfo.nConnHandle, DATAEXCHANGE_PACKET_SIZE, (uint8_t*)&eDataPacket);
            PRINT_ERROR("Error sending the data.\r\n", eResult, ADI_BLER_SUCCESS);
        }
        /* If disconnected, make sure we are in the right mode */
        else
        {
            if (geMode != PERIPHERAL_ADV_MODE)
            {
                SetAdvertisingMode();           
            }
        }        
    }
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
    PRINT_ERROR("Error setting the mode.\r\n", eResult, ADI_BLER_SUCCESS);  

    if (geMode != PERIPHERAL_ADV_MODE)
    {
        PRINTF(("Error in SetAdvertisingMode.\r\n"));
    } 
}


/*!
 * @brief      Read from the sensor template
 *
 * @details    This function will fill a buffer with random data from 0 to 50. This is where you would put your sensor read function. 
 *             This data will be interpreted on the android application as the sensor type the user sends in the second byte of the
 *             packet.  
 *
 * @param [out] pBuffer : Buffer to be filled with random data. This is required to have a size of 6 bytes. 
 *
 */
static void TemplateSensorRead(uint8_t * pBuffer)
{
  ASSERT(pBuffer != NULL);

  /* Fill in the buffer with data from your sensor. Below is dummy data from a random number generator. */
  srand(GET_TIME());
  pBuffer[0u] = rand() % 50u;
  pBuffer[1u] = rand() % 50u;
  pBuffer[2u] = rand() % 50u;
  pBuffer[3u] = rand() % 50u;
  pBuffer[4u] = rand() % 50u;
  pBuffer[5u] = rand() % 50u;
}
