/*!
 *****************************************************************************
  @file:  adi_adt7420.cpp

  @brief: Defines the temperature class implementation

  @details:
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

#include <temp/adt7420/adi_adt7420.h>
#include <base_sensor/adi_sensor_errors.h>
#include "adi_adt7420_cfg.h"

namespace adi_sensor_swpack {
    /*
     * Opend ADT7420 temperature sensor
     */
    SENSOR_RESULT ADT7420::open(void)
    {
        pADI_GPIO0->DS |= (1 << 4) | (1 << 5);
        m_slave_addr = ADI_CFG_I2C_ADDR;

        return(this->InitI2C());
    }

    /*
     * Starts measurement
     */
    SENSOR_RESULT   ADT7420::start()
    {
        return(SENSOR_ERROR_NONE);
    }

    /*
     * Stops measurement
     */
    SENSOR_RESULT   ADT7420::stop()
    {
        return(SENSOR_ERROR_NONE);
    }

    /*
     * Close device
     */
    SENSOR_RESULT   ADT7420::close()
    {
        ADI_I2C_RESULT eI2cResult;

        if((eI2cResult = adi_i2c_Close(m_i2c_handle)) == ADI_I2C_SUCCESS)
            return SENSOR_ERROR_NONE;
        else
            return SET_SENSOR_ERROR(SENSOR_ERROR_I2C, eI2cResult);
    }

    /**
     * @brief  Return temperature.
     *
     * @param [in] pTemperature : Buffer pointer where temperature is returned.
     *
     * @param [in] sizeInBytes  : Size of the buffer in bytes
     *
     * @details getTemperature returns the current temperature in to the supplied buffer
     */
    SENSOR_RESULT ADT7420::getTemperature(uint8_t *pTemperature, const uint32_t sizeInBytes)
    {
        return(this->ReadTemperature(pTemperature, sizeInBytes));
    }

    /**
     * @brief        Returns the temperature in celsius
     *
     * @param [in]   pTemperature Pointer to memory where temperature data is stored
     *
     * @return      SENSOR_RESULT
     *
     * @details     Returns the current temperature value in celsius
     */
    SENSOR_RESULT ADT7420::getTemperatureInCelsius(float *pTemperature)
    {
        uint8_t pBuffer[2];
        SENSOR_RESULT Result = this->ReadTemperature(pBuffer, 2u);

        if(Result == SENSOR_ERROR_NONE) {
            uint16_t nTemp = ((pBuffer[0] << 8) | (pBuffer[1]));
            *pTemperature = (nTemp >> 3u) / 16.0F;
        }

        return Result;
    }
    /**
     * @brief  returns the temperature in fahrenheit
     *
     * @param [in]   pTemperature Pointer to memory where temperature data is stored
     *
     * @return     SENSOR_RESULT
     *
     * @details     Returns the current temperature value in celsius
     */
    SENSOR_RESULT ADT7420::getTemperatureInFahrenheit(float *pTemperature)
    {
        uint8_t pBuffer[2];

        ASSERT(pTemperature != NULL);
        SENSOR_RESULT Result = this->ReadTemperature(pBuffer, 2u);

        if(Result == SENSOR_ERROR_NONE) {
            uint16_t nTemp = ((pBuffer[0] << 8) | (pBuffer[1]));
            float nTempCel = (nTemp >> 3u) / 16.0F;
            *pTemperature = (9.0F / 5.0F) * nTempCel + 32.0F;
        }

        return (Result);
    }

    /**
     * @brief  Returns the temperature value
     *
     * @return SENSOR_RESULT
     *
     * @details
     */
    SENSOR_RESULT ADT7420::InitI2C()
    {
        ADI_I2C_RESULT eI2cResult = ADI_I2C_SUCCESS;

        eI2cResult = adi_i2c_Open(m_slave_addr, &m_I2C_memory, ADI_I2C_MEMORY_SIZE, &m_i2c_handle);

        if(eI2cResult == ADI_I2C_SUCCESS) {
            if((eI2cResult = adi_i2c_Reset(m_i2c_handle)) == ADI_I2C_SUCCESS) {
                if((eI2cResult = adi_i2c_SetBitRate(m_i2c_handle, ADI_CFG_I2C_BIT_RATE))  == ADI_I2C_SUCCESS) {
                    return (SENSOR_ERROR_NONE);
                }
            }
        }

        return (SET_SENSOR_ERROR(SENSOR_ERROR_I2C, eI2cResult));
    }


    /**
     * @brief  returns the temperature value
     *
     * @param  void
     *
     * @return returns temperature sensor value
     *
     * @details
     */
    SENSOR_RESULT ADT7420::ReadTemperature(uint8_t *pTemperature, const uint32_t sizeInBytes)
    {
        ADI_I2C_TRANSACTION sTransfer;
        ADI_I2C_RESULT      eI2cResult = ADI_I2C_SUCCESS;
        uint32_t            nHwErrors;
        uint8_t             aPrologueData[5u];

        ASSERT(sizeInBytes >= 2u);
        ASSERT(pTemperature != NULL);

        /* Set slave address for ADI ADT7420 */
        eI2cResult = adi_i2c_SetSlaveAddress(m_i2c_handle, 0x48);

        if(eI2cResult == ADI_I2C_SUCCESS) {
            /* Read temperature registers, the first temperature register is at address 0 */
            aPrologueData[0]           = 0u;
            sTransfer.pPrologue       = &aPrologueData[0u];
            sTransfer.nPrologueSize   = 1u;
            sTransfer.pData           = pTemperature;
            sTransfer.nDataSize       = 2u;
            sTransfer.bReadNotWrite   = true;
            sTransfer.bRepeatStart    = true;

            eI2cResult = adi_i2c_ReadWrite(m_i2c_handle, &sTransfer, &nHwErrors);

            if(eI2cResult == ADI_I2C_SUCCESS) {
                return(SENSOR_ERROR_NONE);
            }
        }

        return (SET_SENSOR_ERROR(SENSOR_ERROR_I2C, eI2cResult));
    }

} /* namespace */
