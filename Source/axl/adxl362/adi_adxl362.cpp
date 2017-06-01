/*!
 *****************************************************************************
  @file:  adi_adxl362.cpp

  @brief: Defines the accelerometer interface file

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

/** @addtogroup Sensor class
 *  @ingroup Sensor
 *  @{
 *
 *  @brief    ADXL362 accelerometer interface class
 *
 *  @details  ADXL362 accelerometer interface class. This class implements the
 *            accelerometer functions for the ADXL362 accelerometer.
 */
#include <axl/adxl362/adi_adxl362.h>
#include <base_sensor/adi_sensor_errors.h>
#include "adi_adxl362_cfg.h"

namespace adi_sensor_swpack {

    /**
     *  Default constructor
     */
    ADXL362::ADXL362()
    {
        m_range = ADI_CFG_ADXL362_RANGE;
    }

    /**
     *  Default destructor
     */
    ADXL362::~ADXL362()
    {
    }
    /**
     *  Initializes and configures ADXL362 accelerometer
     */
    SENSOR_RESULT ADXL362::open()
    {
        SENSOR_RESULT eResult;

        /* Initialize SPI */
        if((eResult = this->InitSPI()) == SENSOR_ERROR_NONE) {
            if((eResult = this->InitAccelerometer()) == SENSOR_ERROR_NONE) {
                return SENSOR_ERROR_NONE;
            }
        }

        return (eResult);
    }


    /**
     * Starts ADXL362 accelerometer by starting measurement
     */
    SENSOR_RESULT   ADXL362::start()
    {
        SENSOR_RESULT eResult;

        /* Turn on the measurement mode */
        if((eResult = writeRegister(REG_POWER_CTL, ADXL362_PCTL_MEASURE, 1u)) == SENSOR_ERROR_NONE) {
            uint8_t filterReg = ((m_range  << ADXL362_FCTL_RANGE_BITPOS) |
                                 (ADI_CFG_ADXL362_RATE   << ADXL362_FCTL_ODR_BITPOS));

            /* ODR 100Hz Range - 2G */
            if((eResult = writeRegister(REG_FILTER_CTL, filterReg, 1u)) == SENSOR_ERROR_NONE) {
                return(SENSOR_ERROR_NONE);
            }
        }

        return (eResult);
    }

    /**
     * Stops accelerometer by moving it to standby mode.
     */
    SENSOR_RESULT   ADXL362::stop()
    {
        /* put accelerometer in standby mode */
        return(writeRegister(REG_POWER_CTL, ADXL362_PCTL_STDBY, 1u));
    }


    /**
     * Close accelerometer by closing the underlying peripheral.
     */
    SENSOR_RESULT   ADXL362::close()
    {
        ADI_SPI_RESULT eSpiResult;

        eSpiResult = adi_spi_Close(m_spi_handle);

        if(eSpiResult == ADI_SPI_SUCCESS) {
            return SENSOR_ERROR_NONE;
        }
        else {
            return (SET_SENSOR_ERROR(SENSOR_ERROR_SPI, eSpiResult));
        }
    }

    /**
     *  Get X axis acceleration
     */
    SENSOR_RESULT ADXL362::getX(uint8_t *pBuffer, const uint32_t sizeInBytes)
    {
        SENSOR_RESULT eResult;

        ASSERT(sizeInBytes >= 2u);

        CheckDataReady(ADXL362_DATAREADY_ITERCOUNT);

        if((eResult = readRegister(REG_XDATA_L, (uint8_t*)&pBuffer[0], 1u)) == SENSOR_ERROR_NONE) {
            if((eResult = readRegister(REG_XDATA_H, (uint8_t*)&pBuffer[1], 1u)) == SENSOR_ERROR_NONE) {
                return(SENSOR_ERROR_NONE);
            }
        }

        return (eResult);
    }

    /**
     * Get Y axis acceleration
     */
    SENSOR_RESULT ADXL362::getY(uint8_t *pBuffer, const uint32_t sizeInBytes)
    {
        SENSOR_RESULT eResult;

        ASSERT(sizeInBytes >= 2u);

        CheckDataReady(ADXL362_DATAREADY_ITERCOUNT);

        if((eResult = readRegister(REG_YDATA_L, (uint8_t*)&pBuffer[0], 1u)) == SENSOR_ERROR_NONE) {
            if((eResult = readRegister(REG_YDATA_H, (uint8_t*)&pBuffer[1], 1u)) == SENSOR_ERROR_NONE) {
                return(SENSOR_ERROR_NONE);
            }
        }

        return (eResult);
    }

    /**
     * Get Z axis acceleration
     */
    SENSOR_RESULT ADXL362::getZ(uint8_t *pBuffer, const uint32_t sizeInBytes)
    {
        SENSOR_RESULT eResult;

        ASSERT(sizeInBytes >= 2u);

        CheckDataReady(ADXL362_DATAREADY_ITERCOUNT);

        if((eResult = readRegister(REG_ZDATA_L, (uint8_t*)&pBuffer[0], sizeInBytes)) == SENSOR_ERROR_NONE) {
            if((eResult = readRegister(REG_ZDATA_H, (uint8_t*)&pBuffer[1], sizeInBytes)) == SENSOR_ERROR_NONE) {
                return(SENSOR_ERROR_NONE);
            }
        }

        return (eResult);
    }

    /**
     *  Get all three axis acceleration values in to a buffer
     */
    SENSOR_RESULT ADXL362::getXYZ(uint8_t *pBuffer, const uint32_t sizeInBytes)
    {
        SENSOR_RESULT eResult;

        ASSERT(pBuffer != NULL);
        ASSERT(sizeInBytes >= 6u);

        if((eResult = this->getX((uint8_t*)&pBuffer[0], sizeInBytes)) == SENSOR_ERROR_NONE) {
            if((eResult = this->getY((uint8_t*)&pBuffer[2], sizeInBytes)) == SENSOR_ERROR_NONE) {
                if((eResult = this->getZ((uint8_t*)&pBuffer[4], sizeInBytes)) == SENSOR_ERROR_NONE) {
                    return SENSOR_ERROR_NONE;
                }
            }
        }

        return(eResult);
    }

    /**
     * Read accelerometer value to a register
     */
    SENSOR_RESULT  ADXL362::readRegister(const uint32_t regAddr, uint8_t *pBuffer, uint32_t sizeInBytes)
    {
        ADI_SPI_TRANSCEIVER sTransceive;
        ADI_SPI_RESULT      eSpiResult;
        uint8_t             aTxBuffer[2u] = {0x0Bu, (uint8_t)regAddr};
        uint8_t             aRxBuffer[1u] = {0x00u};

        ASSERT(pBuffer != NULL);

        sTransceive.TransmitterBytes = 2u;
        sTransceive.ReceiverBytes    = 1u;
        sTransceive.nTxIncrement     = 1u;
        sTransceive.nRxIncrement     = 1u;
        sTransceive.bRD_CTL          = true;
        sTransceive.bDMA             = false;
        sTransceive.pTransmitter     = aTxBuffer;
        sTransceive.pReceiver        = aRxBuffer;

        eSpiResult = adi_spi_MasterReadWrite(m_spi_handle, &sTransceive);

        if(eSpiResult == ADI_SPI_SUCCESS) {
            *pBuffer   = aRxBuffer[0u];
            return (SENSOR_ERROR_NONE);
        }

        return (SET_SENSOR_ERROR(SENSOR_ERROR_SPI, eSpiResult));
    }


    /**
     * Write specified accelerometer value to a register
     */
    SENSOR_RESULT  ADXL362::writeRegister(const uint8_t regAddr, const uint32_t regValue, uint32_t sizeInBytes)
    {
        ADI_SPI_TRANSCEIVER sTransceive;
        ADI_SPI_RESULT      eSpiResult;
        uint8_t             aTxBuffer[3u] = {0x0Au, regAddr, (uint8_t)(regValue & 0xFF)};
        uint8_t             aRxBuffer[3u] = {0x00u, 0x00u, 0x00u};

        sTransceive.TransmitterBytes = 3u;
        sTransceive.ReceiverBytes    = 3u;
        sTransceive.nTxIncrement     = 1u;
        sTransceive.nRxIncrement     = 1u;
        sTransceive.bRD_CTL          = false;
        sTransceive.bDMA             = false;
        sTransceive.pTransmitter     = aTxBuffer;
        sTransceive.pReceiver        = aRxBuffer;

        eSpiResult = adi_spi_MasterReadWrite(m_spi_handle, &sTransceive);

        if(eSpiResult == ADI_SPI_SUCCESS) {
            return  SENSOR_ERROR_NONE;
        }

        return (SET_SENSOR_ERROR(SENSOR_ERROR_SPI, eSpiResult));
    }

    /**
     * Initializes SPI
     */
    SENSOR_RESULT ADXL362::InitSPI()
    {
        ADI_SPI_RESULT eSpiResult = ADI_SPI_SUCCESS;

        /* Initialize SPI driver */
        eSpiResult = adi_spi_Open(ADI_CFG_SPI_DEV, m_spi_memory,
                                  ADI_SPI_MEMORY_SIZE, &m_spi_handle);

        if(eSpiResult == ADI_SPI_SUCCESS) {
            if((eSpiResult = adi_spi_SetMasterMode(m_spi_handle, true)) == ADI_SPI_SUCCESS) {
                if((eSpiResult = adi_spi_SetBitrate(m_spi_handle, ADI_CFG_SPI_BITRATE)) == ADI_SPI_SUCCESS) {
                    if((eSpiResult = adi_spi_SetChipSelect(m_spi_handle, ADI_CFG_SPI_CS)) == ADI_SPI_SUCCESS)
                        return SENSOR_ERROR_NONE;
                }
            }
        }

        return (SET_SENSOR_ERROR(SENSOR_ERROR_SPI, eSpiResult));
    }

    /**
     * Initialize accelerometer.
     */
    SENSOR_RESULT ADXL362::InitAccelerometer()
    {
        SENSOR_RESULT eResult;
        uint8_t accData;

        /* check if we are talking to correct accelerometer */
        if((eResult = readRegister(REG_DEVID, &accData, 1u)) == SENSOR_ERROR_NONE) {
            if(accData == ADXL362_DEVID) {
                if((eResult = readRegister(REG_DEVID_MST, &accData, 1u)) == SENSOR_ERROR_NONE) {
                    if(accData == ADXL362_DEVID_MST) {
                        if((eResult = readRegister(REG_PARTID, &accData, 1u)) == SENSOR_ERROR_NONE) {
                            if(accData == ADXL362_PARTID) {
                                return (SENSOR_ERROR_NONE);
                            }
                            else {
                                return (SET_SENSOR_ERROR(SENSOR_ERROR_AXL, AXL_ERROR_INVALID_PARTID));
                            }
                        }
                    }
                    else {
                        return (SET_SENSOR_ERROR(SENSOR_ERROR_AXL, Accelerometer::AXL_ERROR_INVALID_DEVID_MST));
                    }
                }
            }
            else {
                return (SET_SENSOR_ERROR(SENSOR_ERROR_AXL, Accelerometer::AXL_ERROR_INVALID_DEVID));
            }
        }

        return(eResult);
    }



    bool ADXL362::CheckDataReady(const uint32_t iterCount)
    {
        uint8_t nValue = 0x00u;
        uint8_t nCount = 0x00u;

        /* Ensure there is valid data to be read */
        do {
            /* Read status register, bottom bit represents data ready */
            readRegister(REG_STATUS, &nValue, 1);
            nCount++;
        }
        while(((nValue & 0x01u) != 0x01u) && (nCount < iterCount));

        if(nCount == iterCount)
            return false;

        return true;
    }

}

/* @} */
