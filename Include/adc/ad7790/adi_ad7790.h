/*!
 *****************************************************************************
  @file adi_ad7790.h

  @brief AD7790 class definition.

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


#ifndef ADI_AD7790_H
#define ADI_AD7790_H


#include <adc/adi_adc.h>
#include <drivers/spi/adi_spi.h>


/*! @addtogroup ad7790_macros AD7790 Macros
 *  @ingroup sensor_macros
 *  @brief Non-configurable macros for the AD7790 class.
 *  @{
 */


/*! Reference voltage of the AD7790. */
#define AD7790_REFERENCE_VOLTAGE   (1.2)

/*! Gain of the AD7790. */
#define AD7790_GAIN                (1.0)

/*! Bit width of the data register in the AD7790. */
#define AD7790_WIDTH_BITS          (16u)

/*! Value to issue a reset. */
#define AD7790_RESET_VALUE         (0xFFu)

/*! Measurement mode select bit position in MODE register. */
#define AD7790_MODE_SELECT_BITP    (5u)

/*! Buffering mode bit position in MODE register. */
#define AD7790_MODE_BUFFER_BITP    (1u)

/*! Range bit position in MODE register. */
#define AD7790_MODE_RANGE_BITP     (3u)

/*! Burnout bit position in MODE register. */
#define AD7790_MODE_BURNOUT_BITP   (2u)

/*! Clock divider bit position in FILTER register. */
#define AD7790_FILTER_CLK_BITP     (3u)

/*! Data rate (frequency) bit position in FILTER register. */
#define AD7790_FILTER_FREQ_BITP    (0u)

/*! Bit mask of the data ready bit in the STATUS register. */
#define AD7790_STATUS_NOT_RDY_BITM (0x80u)

/*! Max number of status reads before timing out when reading data. */
#define AD7790_MAX_STATUS_READS    (1000u)

/*! Small delay between status reads when reading data. */
#define AD7790_STATUS_DELAY_LOOPS  (1024u)


/*! @} */


namespace adi_sensor_swpack
{

    /*!
      * @class AD7790
      *
      * @brief 16-bit sigma-delta analog-to-digital converter.
      *
      */
    class AD7790: public ADC
    {

        public:

            /*!
             *  @enum    READ_REGISTER_ADDRESS
             *
             *  @brief   AD7790 register addresses (for read operations).
             */
            enum READ_REGISTER_ADDRESS
            {
                /*!< Status register read address */
                READ_STATUS = 0x08u,
                /*!< Mode register read address */
                READ_MODE = 0x18u,
                /*!< Filter register read address */
                READ_FILTER = 0x28u,
                /*!< Data register read address */
                READ_DATA = 0x38u
            };

            /*!
             *  @enum    WRITE_REGISTER_ADDRESS
             *
             *  @brief   AD7790 register addresses (for write operations).
             */
            enum WRITE_REGISTER_ADDRESS
            {
                /*!< Mode register write address */
                WRITE_MODE = 0x10u,
                /*!< Filter register write address */
                WRITE_FILTER = 0x20u
            };

            /*!
             *  @enum    ERROR_CODES
             *
             *  @brief   AD7790 error codes.
             *
             *  @details Error codes specific to the AD7790 class that will
             *           get packed into the ADC base class error code functions
             *           as a SENSOR_ERROR_ADC in the SENSOR_RESULT type.
             */
            enum ERROR_CODES
            {
                /*!< Timed out waiting for the data ready bit in getData */
                DATA_READ_STATUS_TIMEOUT,
                /*!< Write to mode register did not work in init */
                MODE_READ_WRITE_MISMATCH,
                /*!< Write to filter register did not work in init */
                FILTER_READ_WRITE_MISMATCH
            };

            /*!
             * @brief   Constructor.
             *
             * @param   [in] pDevice : Pointer to the handle of the SPI driver used to communicate with the AD7790.
             *
             * @details When creating an instance of the AD7790, it must be passed a SPI device handle,
             *          which is filled when calling adi_spi_Open. In some cases, like the CN0357 CO
             *          Sensor, the SPI bus is shared between multiple chips. This flexability allows the
             *          application to define and configure the SPI bus and then simply pass a pointer of it
             *          to the AD7790. Note that there is no thread safety implemented in the AD7790 functions.
             *          The application must verify that only one object uses the SPI driver at a time.
             *
             */
            AD7790(ADI_SPI_HANDLE * pDevice);

            /*!
             * @brief   Destructor.
             *
             * @details Cleans up the AD7790 driver SPI handle to avoid unexpected calls after object is deleted.
             *
             */
            virtual ~AD7790();

            /* Inherited functions */
            virtual SENSOR_RESULT init();
            virtual SENSOR_RESULT reset();
            virtual SENSOR_RESULT readRegister(uint32_t regAddress, uint32_t size, uint8_t * regValue);
            virtual SENSOR_RESULT writeRegister(uint32_t regAddress, uint32_t size, uint32_t regValue);

            /*!
             * @brief  Read data from the AD7790.
             *
             * @param  [out] pData : The data from the ADC (allocated by caller).
             *
             * @return SENSOR_RESULT
             *
             * @note   pData only contains valid data if the function returns SENSOR_ERROR_NONE.
             *
             */
            SENSOR_RESULT getData(uint16_t * pData);

            /*!
             * @brief   Get the voltage from the ADC using the getData function.
             *
             * @details This function is a direct implementation of the equation on page 18 of the
             *          datasheet, but solving for AIN. The parameters in this equation are constants
             *          defined on in this header file.
             *
             * @param  [out] pData : The voltage at the ADC input terminal.
             *
             * @return SENSOR_RESULT
             *
             * @note   pData only contains valid data if the function returns SENSOR_ERROR_NONE.
             *
             */
            SENSOR_RESULT getVoltage(float * pData);

        private:

            /*!
             * @brief  Apply static configuration.
             *
             * @return SENSOR_RESULT
             *
             */
            SENSOR_RESULT applyStaticConfig(void);

            /*! SPI configuration structure, moved off of stack */
            ADI_SPI_TRANSCEIVER m_adc_spi_config;
            /*! Pointer to SPI handle passed from constructor */
            ADI_SPI_HANDLE    * m_adc_spi_phandle;
    };

}

#endif /* ADI_AD7790_H */

