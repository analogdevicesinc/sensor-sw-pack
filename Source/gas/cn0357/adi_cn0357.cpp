/*!
 *****************************************************************************
  @file:   adi_cn0357.cpp

  @brief:  CN0357 Class Implementation
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


#include "adi_cn0357_cfg.h"
#include <gas/cn0357/adi_cn0357.h>
#include <base_sensor/adi_sensor_errors.h>


namespace adi_sensor_swpack {


    /*********************************************************************************
                                       PUBLIC FUNCTIONS
    *********************************************************************************/


    CN0357::CN0357()
        : m_rheostat(&m_spi_handle), m_adc(&m_spi_handle)
    {
        this->setType(ADI_CO_TYPE);
        this->setID(CN0357_CFG_ID);
        this->setVersion(CN0357_CFG_VERSION);
    }


    SENSOR_RESULT CN0357::open()
    {
        ADI_SPI_RESULT eSpiResult;
        SENSOR_RESULT  eResult;

        eSpiResult = adi_spi_Open(CN0357_CFG_SPI_DEV_NUMBER, m_spi_memory, ADI_SPI_MEMORY_SIZE, &m_spi_handle);

        if(eSpiResult == ADI_SPI_SUCCESS) {
            eSpiResult = adi_spi_SetContinuousMode(m_spi_handle, true);

            if(eSpiResult == ADI_SPI_SUCCESS) {
                eResult = m_rheostat.open();

                if(eResult == SENSOR_ERROR_NONE) {
                    eResult = m_rheostat.setWiper(CN0357_FEEDBACK_RESISTOR);

                    if(eResult == SENSOR_ERROR_NONE) {
                        eResult = m_rheostat.close();

                        if(eResult == SENSOR_ERROR_NONE) {
                            return m_adc.init();

                        }
                    }
                }

                return (eResult);
            }
        }

        return (SET_SENSOR_ERROR(SENSOR_ERROR_SPI, eSpiResult));
    }


    SENSOR_RESULT CN0357::start()
    {
        // TODO: Place ADC into continuous measurement mode (make "open" put ADC into single by default)
        return (SENSOR_ERROR_NONE);
    }


    SENSOR_RESULT CN0357::stop()
    {
        // TODO: Place ADC into single measurement mode to save power
        return (SENSOR_ERROR_NONE);
    }


    SENSOR_RESULT CN0357::close()
    {
        ADI_SPI_RESULT eSpiResult;

        eSpiResult = adi_spi_Close(m_spi_handle);

        if(eSpiResult == ADI_SPI_SUCCESS) {
            return (SENSOR_ERROR_NONE);
        }

        return (SET_SENSOR_ERROR(SENSOR_ERROR_SPI, eSpiResult));
    }


    SENSOR_RESULT CN0357::getPPM(float * pData)
    {
        SENSOR_RESULT eResult;
        float         fVoltage = 0.0F;

        ASSERT(pData != NULL);

        eResult = m_adc.getVoltage(&fVoltage);

        if(eResult == SENSOR_ERROR_NONE) {
            if(fVoltage < 0.0F) {
                fVoltage = -1.0F * fVoltage;
            }

            *pData = (fVoltage / CN0357_FEEDBACK_RESISTOR) / CN0357_SENSOR_SENSITIVTY;
        }

        return (eResult);
    }


    /*********************************************************************************
                                       PRIVATE CLASSES
    *********************************************************************************/


    CN0357::AD5270::AD5270(ADI_SPI_HANDLE * pDevice)
    {
        m_rheostat_spi_phandle = pDevice;

        // Some members of SPI transceiver struct are constant and can be configured once in order to save cycles
        m_rheostat_spi_config.TransmitterBytes = 2u;
        m_rheostat_spi_config.nTxIncrement     = 1u;
        m_rheostat_spi_config.nRxIncrement     = 1u;
        m_rheostat_spi_config.bRD_CTL          = false;
        m_rheostat_spi_config.bDMA             = false;
    }


    CN0357::AD5270::~AD5270()
    {
        m_rheostat_spi_phandle = NULL;
    }


    SENSOR_RESULT CN0357::AD5270::open()
    {
        ADI_GPIO_RESULT eGpioResult;
        ADI_SPI_RESULT  eSpiResult;

        ASSERT(m_rheostat_spi_phandle != NULL);
        ASSERT(*m_rheostat_spi_phandle != NULL);

        eSpiResult = adi_spi_SetChipSelect(*m_rheostat_spi_phandle, ADI_SPI_CS_NONE);

        if(eSpiResult == ADI_SPI_SUCCESS) {
            eGpioResult = adi_gpio_Init(m_gpio_memory, ADI_GPIO_MEMORY_SIZE);

            if(eGpioResult == ADI_GPIO_SUCCESS) {
                eGpioResult = adi_gpio_OutputEnable(CN0357_CFG_AD5270_CS_GPIO_PORT, CN0357_CFG_AD5270_CS_GPIO_PIN, true);

                if(eGpioResult == ADI_GPIO_SUCCESS) {
                    eGpioResult = adi_gpio_SetHigh(CN0357_CFG_AD5270_CS_GPIO_PORT, CN0357_CFG_AD5270_CS_GPIO_PIN);

                    if(eGpioResult == ADI_GPIO_SUCCESS) {
                        return this->sendData(AD5270::WRITE_CTL_REG, AD5270::ENABLE_RDAC);
                    }
                }
            }

            return (SET_SENSOR_ERROR(SENSOR_ERROR_GPIO, eGpioResult));
        }

        return (SET_SENSOR_ERROR(SENSOR_ERROR_SPI, eSpiResult));
    }


    SENSOR_RESULT CN0357::AD5270::setWiper(float fResistance)
    {
        SENSOR_RESULT eSensorResult;
        uint16_t      nResistanceVerify;
        uint16_t      nResistance;
        float         fSteps;

        ASSERT((fResistance < AD5270_NOMINAL_VALUE) && (fResistance >= 0.0F));

        // Convert from the resistance to the number of steps in the rheostat
        fSteps = (fResistance / AD5270_NOMINAL_VALUE) * AD5270_NUM_POSITIONS;
        nResistance = (uint16_t) fSteps;

        eSensorResult = this->sendData(AD5270::WRITE_RDAC_REG, nResistance);

        if(eSensorResult == SENSOR_ERROR_NONE) {
            eSensorResult = this->receiveData(AD5270::READ_RDAC_REG, 0x00u, &nResistanceVerify);

            if(eSensorResult == SENSOR_ERROR_NONE) {
                if(nResistance != (nResistanceVerify & AD5270_BITM_DATA)) {
                    return (SET_SENSOR_ERROR(SENSOR_ERROR_GAS, CN0357::SET_WIPER_FAILED));
                }
            }
        }

        return (eSensorResult);
    }


    SENSOR_RESULT CN0357::AD5270::close()
    {
        ADI_GPIO_RESULT eGpioResult;
        SENSOR_RESULT   eSensorResult;

        eSensorResult = this->sendData(AD5270::SHUTDOWN, AD5270::ISSUE_SHUTDOWN);

        if(eSensorResult == SENSOR_ERROR_NONE) {
            eSensorResult = this->sendData(AD5270::NOOP, AD5270::ISSUE_NOOP);

            if(eSensorResult == SENSOR_ERROR_NONE) {
                eGpioResult = adi_gpio_UnInit();

                if(eGpioResult == ADI_GPIO_SUCCESS) {
                    return (SENSOR_ERROR_NONE);
                }

                return (SET_SENSOR_ERROR(SENSOR_ERROR_GPIO, eGpioResult));
            }
        }

        return (eSensorResult);
    }


    SENSOR_RESULT CN0357::AD5270::sendData(uint8_t nCommand, uint16_t nData)
    {
        ADI_GPIO_RESULT eGpioResult;
        ADI_SPI_RESULT  eSpiResult;
        uint16_t        nCommandAndData;
        uint8_t         aTransmitBytes[2u];

        ASSERT(m_rheostat_spi_phandle != NULL);
        ASSERT(*m_rheostat_spi_phandle != NULL);

        nCommandAndData    = (((uint16_t) nCommand) << AD5270_BITP_CMD) | (nData & AD5270_BITM_DATA);
        aTransmitBytes[0u] = (uint8_t)((nCommandAndData & 0xFF00u) >> 8u);
        aTransmitBytes[1u] = (uint8_t)(nCommandAndData & 0x00FFu);

        m_rheostat_spi_config.ReceiverBytes = 0u;
        m_rheostat_spi_config.pTransmitter  = aTransmitBytes;
        m_rheostat_spi_config.pReceiver     = NULL;

        eGpioResult = adi_gpio_SetLow(CN0357_CFG_AD5270_CS_GPIO_PORT, CN0357_CFG_AD5270_CS_GPIO_PIN);

        if(eGpioResult == ADI_GPIO_SUCCESS) {
            eSpiResult = adi_spi_MasterReadWrite(*m_rheostat_spi_phandle, &m_rheostat_spi_config);

            if(eSpiResult == ADI_SPI_SUCCESS) {
                eGpioResult = adi_gpio_SetHigh(CN0357_CFG_AD5270_CS_GPIO_PORT, CN0357_CFG_AD5270_CS_GPIO_PIN);

                if(eGpioResult == ADI_GPIO_SUCCESS) {
                    return (SENSOR_ERROR_NONE);
                }
            }
            else {
                return (SET_SENSOR_ERROR(SENSOR_ERROR_SPI, eSpiResult));
            }
        }

        return (SET_SENSOR_ERROR(SENSOR_ERROR_GPIO, eGpioResult));
    }


    SENSOR_RESULT CN0357::AD5270::receiveData(uint8_t nCommand, uint16_t nData, uint16_t * pResult)
    {
        ADI_GPIO_RESULT eGpioResult;
        ADI_SPI_RESULT  eSpiResult;
        uint16_t        nCommandAndData;
        uint8_t         aTransmitBytes[2u];
        uint8_t         aReceiveBytes[2u] = {0x00u, 0x00u};

        ASSERT(m_rheostat_spi_phandle != NULL);
        ASSERT(*m_rheostat_spi_phandle != NULL);
        ASSERT(pResult != NULL);

        nCommandAndData    = (((uint16_t) nCommand) << AD5270_BITP_CMD) | (nData & AD5270_BITM_DATA);
        aTransmitBytes[0u] = (uint8_t)((nCommandAndData & 0xFF00u) >> 8u);
        aTransmitBytes[1u] = (uint8_t)(nCommandAndData & 0x00FFu);

        m_rheostat_spi_config.ReceiverBytes = 2u;
        m_rheostat_spi_config.pTransmitter  = aTransmitBytes;
        m_rheostat_spi_config.pReceiver     = aReceiveBytes;

        eGpioResult = adi_gpio_SetLow(CN0357_CFG_AD5270_CS_GPIO_PORT, CN0357_CFG_AD5270_CS_GPIO_PIN);

        if(eGpioResult == ADI_GPIO_SUCCESS) {
            eSpiResult = adi_spi_MasterReadWrite(*m_rheostat_spi_phandle, &m_rheostat_spi_config);

            if(eSpiResult == ADI_SPI_SUCCESS) {
                eGpioResult = adi_gpio_SetHigh(CN0357_CFG_AD5270_CS_GPIO_PORT, CN0357_CFG_AD5270_CS_GPIO_PIN);

                if(eGpioResult == ADI_GPIO_SUCCESS) {
                    *pResult = ((aReceiveBytes[0u] << 8u) | aReceiveBytes[1u]);
                    return (SENSOR_ERROR_NONE);
                }
            }
            else {
                return (SET_SENSOR_ERROR(SENSOR_ERROR_SPI, eSpiResult));
            }
        }

        return (SET_SENSOR_ERROR(SENSOR_ERROR_GPIO, eGpioResult));
    }


}

