/*!
 *****************************************************************************
  @file:   adi_ad7790.cpp

  @brief:  AD7790 Class Implementation
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

THIS SOFTWARE IS PROVIDED BY ANALOG DEVICES, INC. AND CONTRIBUTORS "AS IS"
AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, NON-
INFRINGEMENT, TITLE, MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
DISCLAIMED. IN NO EVENT SHALL ANALOG DEVICES, INC. OR CONTRIBUTORS BE LIABLE
FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, PUNITIVE OR
CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, DAMAGES ARISING OUT OF
CLAIMS OF INTELLECTUAL PROPERTY RIGHTS INFRINGEMENT; PROCUREMENT OF
SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS
INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN
CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE)
ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE
POSSIBILITY OF SUCH DAMAGE.

*****************************************************************************/


#include "adi_ad7790_cfg.h"
#include <adc/ad7790/adi_ad7790.h>
#include <base_sensor/adi_sensor_errors.h>


namespace adi_sensor_swpack {


    /*********************************************************************************
                                       PUBLIC FUNCTIONS
    *********************************************************************************/


    AD7790::AD7790(ADI_SPI_HANDLE * pDevice)
    {
        m_adc_spi_phandle = pDevice;

        // Some members of SPI transceiver struct are constant and can be configured once in order to save cycles
        m_adc_spi_config.nTxIncrement = 1u;
        m_adc_spi_config.nRxIncrement = 1u;
        m_adc_spi_config.bDMA         = false;
    }


    AD7790::~AD7790()
    {
        m_adc_spi_phandle = NULL;
    }


    SENSOR_RESULT AD7790::init()
    {
        ADI_SPI_RESULT eSpiResult;
        SENSOR_RESULT  eAdcResult;

        ASSERT(m_adc_spi_phandle != NULL);
        ASSERT(*m_adc_spi_phandle != NULL);

        // Use hardware chip-select
        eSpiResult = adi_spi_SetChipSelect(*m_adc_spi_phandle, AD7790_CFG_SPI_CS);

        if(eSpiResult == ADI_SPI_SUCCESS) {
            // Reset the serial interface
            eAdcResult = this->reset();

            if(eAdcResult == SENSOR_ERROR_NONE) {
                // Apply static config to the MODE and FILTER registers
                eAdcResult = this->applyStaticConfig();

                if(eAdcResult == SENSOR_ERROR_NONE) {
                    return (SENSOR_ERROR_NONE);
                }
            }
        }
        else {
            return (SET_SENSOR_ERROR(SENSOR_ERROR_SPI, eSpiResult));
        }

        return (eAdcResult);
    }


    SENSOR_RESULT AD7790::reset()
    {
        ADI_SPI_RESULT eSpiResult;
        uint8_t        aTransmitBytes[4u] = {AD7790_RESET_VALUE, AD7790_RESET_VALUE, AD7790_RESET_VALUE, AD7790_RESET_VALUE};

        ASSERT(m_adc_spi_phandle != NULL);
        ASSERT(*m_adc_spi_phandle != NULL);

        m_adc_spi_config.TransmitterBytes = 4u;
        m_adc_spi_config.ReceiverBytes    = 0u;
        m_adc_spi_config.bRD_CTL          = false;
        m_adc_spi_config.pTransmitter     = aTransmitBytes;
        m_adc_spi_config.pReceiver        = NULL;

        eSpiResult = adi_spi_MasterReadWrite(*m_adc_spi_phandle, &m_adc_spi_config);

        if(eSpiResult == ADI_SPI_SUCCESS) {
            return (SENSOR_ERROR_NONE);
        }
        else {
            // TODO: Check and log hardware errors if they occur
            return (SET_SENSOR_ERROR(SENSOR_ERROR_SPI, eSpiResult));
        }
    }


    SENSOR_RESULT AD7790::readRegister(uint32_t regAddress, uint32_t size, uint8_t * regValue)
    {
        ADI_SPI_RESULT eSpiResult;
        uint8_t        nTransmitByte = (uint8_t) regAddress;
        uint8_t        nReceivedByte = 0x00u;

        ASSERT(m_adc_spi_phandle != NULL);
        ASSERT(*m_adc_spi_phandle != NULL);
        ASSERT(regValue != NULL);
        ASSERT(size == 1u);

        m_adc_spi_config.TransmitterBytes = 1u;
        m_adc_spi_config.ReceiverBytes    = 1u;
        m_adc_spi_config.bRD_CTL          = true;
        m_adc_spi_config.pTransmitter     = &nTransmitByte;
        m_adc_spi_config.pReceiver        = &nReceivedByte;

        eSpiResult = adi_spi_MasterReadWrite(*m_adc_spi_phandle, &m_adc_spi_config);

        if(eSpiResult == ADI_SPI_SUCCESS) {
            *regValue = nReceivedByte;
            return (SENSOR_ERROR_NONE);
        }
        else {
            // TODO: Check and log hardware errors if they occur
            return (SET_SENSOR_ERROR(SENSOR_ERROR_SPI, eSpiResult));
        }
    }


    SENSOR_RESULT AD7790::writeRegister(uint32_t regAddress, uint32_t size, uint32_t regValue)
    {
        ADI_SPI_RESULT eSpiResult;
        uint8_t        aTransmitBytes[2u] = {(uint8_t) regAddress, (uint8_t) regValue};

        ASSERT(m_adc_spi_phandle != NULL);
        ASSERT(*m_adc_spi_phandle != NULL);
        ASSERT(size == 1u);

        m_adc_spi_config.TransmitterBytes = 2u;
        m_adc_spi_config.ReceiverBytes    = 0u;
        m_adc_spi_config.bRD_CTL          = false;
        m_adc_spi_config.pTransmitter     = aTransmitBytes;
        m_adc_spi_config.pReceiver        = NULL;

        eSpiResult = adi_spi_MasterReadWrite(*m_adc_spi_phandle, &m_adc_spi_config);

        if(eSpiResult == ADI_SPI_SUCCESS) {
            return (SENSOR_ERROR_NONE);
        }
        else {
            // TODO: Check and log hardware errors if they occur
            return (SET_SENSOR_ERROR(SENSOR_ERROR_SPI, eSpiResult));
        }
    }


    SENSOR_RESULT AD7790::getData(uint16_t * pData)
    {
        ADI_SPI_RESULT eSpiResult;
        SENSOR_RESULT  eAdcResult;
        uint32_t       nCounter          = 0u;
        uint8_t        nTransmitByte     = AD7790::READ_DATA;
        uint8_t        aReceiveBytes[2u] = {0x00u, 0x00u};
        uint8_t        nStatus;

        ASSERT(m_adc_spi_phandle != NULL);
        ASSERT(*m_adc_spi_phandle != NULL);
        ASSERT(pData != NULL);

        do {
            eAdcResult = this->readRegister(AD7790::READ_STATUS, 1u, &nStatus);

            if(eAdcResult != SENSOR_ERROR_NONE) {
                return (eAdcResult);
            }

            nCounter++;

            if(nCounter == AD7790_MAX_STATUS_READS) {
                break;
            }

            for(volatile uint16_t i = 0u; i < AD7790_STATUS_DELAY_LOOPS; i++);

        }
        while((nStatus & AD7790_STATUS_NOT_RDY_BITM) == AD7790_STATUS_NOT_RDY_BITM);

        if(nCounter == AD7790_MAX_STATUS_READS) {
            return (SET_SENSOR_ERROR(SENSOR_ERROR_ADC, AD7790::DATA_READ_STATUS_TIMEOUT));
        }

        m_adc_spi_config.TransmitterBytes = 1u;
        m_adc_spi_config.ReceiverBytes    = 2u;
        m_adc_spi_config.bRD_CTL          = true;
        m_adc_spi_config.pTransmitter     = &nTransmitByte;
        m_adc_spi_config.pReceiver        = aReceiveBytes;

        eSpiResult = adi_spi_MasterReadWrite(*m_adc_spi_phandle, &m_adc_spi_config);

        if(eSpiResult == ADI_SPI_SUCCESS) {
            *pData = ((aReceiveBytes[0u] << 8u) | aReceiveBytes[1u]);
            return (SENSOR_ERROR_NONE);
        }
        else {
            // TODO: Check and log hardware errors if they occur
            return (SET_SENSOR_ERROR(SENSOR_ERROR_SPI, eSpiResult));
        }
    }


    SENSOR_RESULT AD7790::getVoltage(float * pData)
    {
        SENSOR_RESULT eResult;
        uint32_t      nAdcMid = (1u << (AD7790_WIDTH_BITS - 1u));
        uint16_t      nAdcData;

        ASSERT(pData != NULL);

        eResult = this->getData(&nAdcData);

        if(eResult == SENSOR_ERROR_NONE) {
            *pData = (((((float) nAdcData) / ((float) nAdcMid)) - 1.0F) * ((float) AD7790_REFERENCE_VOLTAGE)) / ((float) AD7790_GAIN);
        }

        return (eResult);
    }


    /*********************************************************************************
                                       PRIVATE FUNCTIONS
    *********************************************************************************/


    SENSOR_RESULT AD7790::applyStaticConfig(void)
    {
        SENSOR_RESULT eAdcResult;
        uint8_t    nMode;
        uint8_t    nModeVerify;
        uint8_t    nFilter;
        uint8_t    nFilterVerify;

        nMode = (AD7790_CFG_MODE_BURNOUT_CURRENT << AD7790_MODE_BURNOUT_BITP) |
                (AD7790_CFG_MODE_BUFFERING << AD7790_MODE_BUFFER_BITP)        |
                (AD7790_CFG_MODE_SELECT << AD7790_MODE_SELECT_BITP)           |
                (AD7790_CFG_MODE_RANGE << AD7790_MODE_RANGE_BITP);

        eAdcResult = this->writeRegister(AD7790::WRITE_MODE, 1u, nMode);

        if(eAdcResult == SENSOR_ERROR_NONE) {
            nModeVerify = ~nMode;
            eAdcResult = this->readRegister(AD7790::READ_MODE, 1u, &nModeVerify);

            if((eAdcResult == SENSOR_ERROR_NONE) && (nModeVerify == nMode)) {
                nFilter = (AD7790_CFG_FILTER_CLOCK_DIV << AD7790_FILTER_CLK_BITP) |
                          (AD7790_CFG_FILTER_WORD_RATE << AD7790_FILTER_FREQ_BITP);

                eAdcResult = this->writeRegister(AD7790::WRITE_FILTER, 1u, nFilter);

                if(eAdcResult == SENSOR_ERROR_NONE) {
                    nFilterVerify = ~nFilter;
                    eAdcResult = this->readRegister(AD7790::READ_FILTER, 1u, &nFilterVerify);

                    if((eAdcResult == SENSOR_ERROR_NONE) && (nFilterVerify == nFilter)) {
                        return (SENSOR_ERROR_NONE);
                    }
                    else if(nFilterVerify != nFilter) {
                        return (SET_SENSOR_ERROR(SENSOR_ERROR_ADC, AD7790::FILTER_READ_WRITE_MISMATCH));
                    }
                }
            }
            else if(nModeVerify != nMode) {
                return (SET_SENSOR_ERROR(SENSOR_ERROR_ADC, AD7790::MODE_READ_WRITE_MISMATCH));
            }
        }

        return (eAdcResult);
    }


}

