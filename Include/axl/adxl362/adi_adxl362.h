/*!
 *****************************************************************************
  @file adi_adxl362.h

  @brief ADXL362 class definition.

  @details
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


#ifndef ADI_ADXL362_H
#define ADI_ADXL362_H

#include <axl/adi_accelerometer.h>
#include <drivers/spi/adi_spi.h>

/*! @addtogroup adxl362_macros ADXL362 Macros
 *  @ingroup sensor_macros
 *  @brief Non-configurable macros for the ADXL362 class.
 *  @{
 */
namespace adi_sensor_swpack
{
    /*! Device ID register value */
#define ADXL362_DEVID            (0xADu)
    /*! Device ID MST register value */
#define ADXL362_DEVID_MST        (0x1Du)
    /*! Part ID register value */
#define ADXL362_PARTID           (0xF2u)

    /*! Power control register standby */
#define ADXL362_PCTL_STDBY        (0u)
    /*! Power control register measure */
#define ADXL362_PCTL_MEASURE      (2u)
    /*! Power control register autosleep */
#define ADXL362_PCTL_AUTOSLEEP    (4u)
    /*! Power control register wakeup */
#define ADXL362_PCTL_WAKEUP       (8u)

    /*! Filter control register standby */
#define ADXL362_FCTL_STDBY        (0u)
    /*! Filter control register range bit position */
#define ADXL362_FCTL_RANGE_BITPOS (6u)
    /*! Filter control register output data rate bit position */
#define ADXL362_FCTL_ODR_BITPOS   (0u)

    /*! ADXL362 SPI command code write register */
#define ADXL362_CMD_WRITE_REG     (0x0A)
    /*! ADXL362 SPI command code read register */
#define ADXL362_CMD_READ_REG      (0x0B)
    /*! ADXL362 SPI command code read FIFO */
#define ADXL362_CMD_READ_FIFO     (0x0D)

    /*! Check for data ready - iteration count */
#define ADXL362_DATAREADY_ITERCOUNT (0xA)

    /*! @} */

    /**
     * @class ADXL362
     *
     * @brief ADXL362 accelerometer sensor class
     *
     **/
#pragma pack(push)
#pragma pack(4)
    class ADXL362 : public Accelerometer
    {
        public:

            /*!
             *  @enum   ADXL362_REGS
             *
             *  @brief  ADXL362 accelerometer registers
             *
             *  @details ADXL362 accelerometer registers.They are accessed through SPI interface.
             */
            typedef enum
            {
                REG_DEVID             = 0x00,  /*!< ADXL362 device ID                     */
                REG_DEVID_MST         = 0x01,  /*!< ADXL362 Analog Devices MEMS ID        */
                REG_PARTID            = 0x02,  /*!< ADXL362 Part ID                       */
                REG_STATUS            = 0x0B,  /*!< Status register                       */
                REG_XDATA_L           = 0x0E,  /*!< X-axis acceleration low byte          */
                REG_XDATA_H           = 0x0F,  /*!< X-axis acceleration high byte         */
                REG_YDATA_L           = 0x10,  /*!< Y-axis acceleration low byte          */
                REG_YDATA_H           = 0x11,  /*!< Y-axis acceleration high byte         */
                REG_ZDATA_L           = 0x12,  /*!< Z-axis acceleration low byte          */
                REG_ZDATA_H           = 0x13,  /*!< Z-axis acceleration high byte         */
                REG_TEMP_L            = 0x14,  /*!< Temperature data register low byte    */
                REG_TEMP_H            = 0x15,  /*!< Temperature data register high byte   */
                REG_SOFT_RESET        = 0x1F,  /*!< Softreset, code 0x52 value resets part*/
                REG_THRESH_ACT_L      = 0x20,  /*!< Activity thershold low register       */
                REG_THRESH_ACT_H      = 0x21,  /*!< Activity thershold high register      */
                REG_TIME_ACT          = 0x22,  /*!< Activity timer register               */
                REG_THRESH_INACT_L    = 0x23,  /*!< Inactivity thershold low register     */
                REG_THRESH_INACT_H    = 0x24,  /*!< Inactivity thershold high register    */
                REG_TIME_INACT_L      = 0x25,  /*!< Inactivity time register low          */
                REG_TIME_INACT_H      = 0x26,  /*!< Inactivity time register high         */
                REG_ACT_INACT_CTL     = 0x27,  /*!< Activity/Inactivity control register  */
                REG_INTMAP1           = 0x2A,  /*!< Interrupt-1 map register              */
                REG_INTMAP2           = 0x2B,  /*!< Interrupt-2 map register              */
                REG_FILTER_CTL        = 0x2C,  /*!< Filter control register               */
                REG_POWER_CTL         = 0x2D,  /*!< Power control register                */
            } ADXL362_REGS;

            /*!
             *  @enum   NOISE_MODE
             *
             *  @brief  ADXL362 Noise mode
             *
             */
            typedef enum
            {
                NOISE_MODE_NORMAL = 0x0,      /*!< Noise mode normal                      */
                NOISE_MODE_LOW    = 0x1,      /*!< Noise mode low                         */
                NOISE_MODE_ULOW   = 0x2,      /*!< Noise mode ultra low                   */
            } NOISE_MODE;

            ADXL362();
            virtual ~ADXL362();
            virtual SENSOR_RESULT open();
            virtual SENSOR_RESULT start();
            virtual SENSOR_RESULT stop();
            virtual SENSOR_RESULT close();
            virtual SENSOR_RESULT getX(uint8_t *pBuffer, const uint32_t sizeInBytes);
            virtual SENSOR_RESULT getY(uint8_t *pBuffer, const uint32_t sizeInBytes);
            virtual SENSOR_RESULT getZ(uint8_t *pBuffer, const uint32_t sizeInBytes);
            virtual SENSOR_RESULT getXYZ(uint8_t *pBuffer, const uint32_t sizeInBytes);
            virtual SENSOR_RESULT readRegister(const uint32_t regAddr, uint8_t *pBuffer, uint32_t sizeInBytes);
            virtual SENSOR_RESULT writeRegister(const uint8_t regAddr, const uint32_t regValue, uint32_t sizeInBytes);

        private:

            /*!
             * @brief  Initialize SPI
             *
             * @return SENSOR_RESULT
             *
             * @details Initializes SPI with default configuration.Default configuration is
             *          specified in the adi_adxl362_cfg.h.
             */
            SENSOR_RESULT  InitSPI();

            /*!
             * @brief  Initialize Accelerometer
             *
             * @return SENSOR_RESULT
             *
             * @details Initializes ADXL362 with default configuration.Default configuration is
             *          specified in the adi_adxl362_cfg.h.
             */
            SENSOR_RESULT  InitAccelerometer();

            /*!
             * @brief  Checks for the data
             *
             * @param  iterCount : Iteration counter for graceful return
             *
             * @return SENSOR_RESULT
             *
             * @details polls the accelerometer for data for specified number of times
             */
            bool           CheckDataReady(const uint32_t iterCount);

            /*! SPI driver memory */
            uint8_t        m_spi_memory[ADI_SPI_MEMORY_SIZE];
            /*! SPI driver handle */
            ADI_SPI_HANDLE m_spi_handle;
    };
#pragma pack(pop)
}

#endif /* ADI_ADXL362_H */
