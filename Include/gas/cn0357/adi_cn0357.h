/*!
 *****************************************************************************
  @file adi_cn0357.h

  @brief CN0357 class definition.

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


#ifndef ADI_CN0357_H
#define ADI_CN0357_H


#include <gas/adi_gas.h>
#include <drivers/spi/adi_spi.h>
#include <drivers/gpio/adi_gpio.h>
#include <adc/ad7790/adi_ad7790.h>


/*! @addtogroup cn0357_macros CN0357 Macros
 *  @ingroup sensor_macros
 *  @brief Non-configurable macros for the CN0357 class.
 *  @{
 */


/*!
 * @brief   Sensor sensitivity.
 *
 * @details This value is taken from Table 1 in the CN0357 datasheet.
 *          The units are amperes per PPM. It is used when computing
 *          the gas concentration in PPM in the getPPM function.
 *
 */
#define CN0357_SENSOR_SENSITIVTY (0.0000000065)

/*!
 * @brief   Feedback resistor.
 *
 * @details This is the value that the rheostat needs to be configured to
 *          for proper operation (ohms). This value is dervied from equation
 *           2 (located on page 2) on the CN0357 datasheet. Which is:
 *
 *          Rf = Vref / (Range * Sensitivty)
 *
 *          Where the sensitivty is given above and the range is 2000 PPM.
 *
 */
#define CN0357_FEEDBACK_RESISTOR (9230.76)


/*! @} */


/*!
 * @brief The bit poistion of the command in the 16-bit short
 *        send to the AD5270.
 *
 */
#define AD5270_BITP_CMD               (10u)

/*!
 * @brief The bit mask of the data in the 16-bit short send to
 *        the AD5270.
 *
 */
#define AD5270_BITM_DATA              (0x03FFu)

/*!
 * @brief The nominal value (or full-scale value) of the AD5270 (in ohms).
 *
 */
#define AD5270_NOMINAL_VALUE          (20000.0F)

/*!
 * @brief The number of wiper postions in the AD5270 (10-bits wide).
 *
 */
#define AD5270_NUM_POSITIONS          (1024.0F)


namespace adi_sensor_swpack
{

    /*!
     * @class   CN0357
     *
     * @brief   Carbon Monoxide (CO) sensor.
     *
     * @details This class defines a software interface for applications
     *          that want to use the CN0357 CO sensor.
     *
     */
#pragma pack(push)
#pragma pack(4)
    class CN0357 : public Gas
    {
        public:

            /*!
             *  @enum    ERROR_CODES
             *
             *  @brief   CN0357 error codes.
             *
             *  @details Error codes specific to the CN0357 class that will
             *           get packed into the Gas base class error code functions
             *           as a SENSOR_ERROR_GAS in the SENSOR_RESULT type.
             */
            enum ERROR_CODES
            {
                SET_WIPER_FAILED
            };


            /*!
             * @brief   Constructor.
             *
             * @details Set constant parameters used to identify the sensor.
             *
             */
            CN0357();

            /* Inherited functions */
            virtual SENSOR_RESULT open();
            virtual SENSOR_RESULT start();
            virtual SENSOR_RESULT stop();
            virtual SENSOR_RESULT close();
            virtual SENSOR_RESULT getPPM(float * pData);

        private:

            /*!
             * @class   AD5270
             *
             * @brief   Digital rheostat.
             *
             * @details This defines a software interface for using the AD5270 within the
             *          context of the CN0357.
             *
             */
#pragma pack(push)
#pragma pack(4)
            class AD5270
            {
                public:

                    /*!
                     *  @enum    COMMANDS
                     *
                     *  @brief   CN0357 commands.
                     *
                     *  @details The first four bits of the 16-bits sent when communicating with
                     *           the AD5270 represent a command.
                     *
                     */
                    enum COMMANDS
                    {
                        /*!< Write control reigster */
                        WRITE_CTL_REG  = 0x7u,
                        /*!< Write RDAC register */
                        WRITE_RDAC_REG = 0x1u,
                        /*!< Read RDAC register */
                        READ_RDAC_REG  = 0x2u,
                        /*!< Part shutdown */
                        SHUTDOWN       = 0x9u,
                        /*!< No operation */
                        NOOP           = 0x0u
                    };

                    /*!
                     *  @enum    DATA
                     *
                     *  @brief   CN0357 data.
                     *
                     *  @details The remaining 10-bits represent the data to go along with the command.
                     *
                     */
                    enum DATA
                    {
                        /*!< Issue shutdown */
                        ISSUE_SHUTDOWN = 0x1u,
                        /*!< Issue no operation */
                        ISSUE_NOOP     = 0x0u,
                        /*!< Enable RDAC register */
                        ENABLE_RDAC    = 0x2u
                    };

                    /*!
                     * @brief   Constructor.
                     *
                     * @param   [in] pDevice : Pointer to the handle of the SPI driver used to communicate with the AD5270.
                     *
                     * @details When creating an instance of the AD5270, it must be passed a SPI device handle,
                     *          which is filled when calling adi_spi_Open. In some cases, like the CN0357 CO
                     *          Sensor, the SPI bus is shared between multiple chips. This flexability allows the
                     *          application to define and configure the SPI bus and then simply pass a pointer of it
                     *          to the AD5270. Note that there is no thread safety implemented in the AD5270 functions.
                     *          The application must verify that only one object uses the SPI driver at a time.
                     *
                     */
                    AD5270(ADI_SPI_HANDLE * pDevice);

                    /*!
                     * @brief   Destructor.
                     *
                     * @details Cleans up the AD5270 driver SPI handle to avoid unexpected calls after object is deleted.
                     *
                     */
                    ~AD5270();

                    /*!
                     * @brief   Open.
                     *
                     * @details Initialize the GPIO driver and SPI chip select. Enable write access to the RDAC register so
                     *          that setWiper can be called freely.
                     *
                     * @return  SENSOR_RESULT
                     *
                     * @note    Must be called before setWiper or close can be called.
                     *
                     */
                    SENSOR_RESULT open();

                    /*!
                     * @brief   Set wiper.
                     *
                     * @details Write to the RDAC register in the AD5270, setting the rheostat to a particular resistance.
                     *          Note that only valid values are between [0:AD5270_NOMINAL_VALUE].
                     *
                     * @return  SENSOR_RESULT
                     *
                     */
                    SENSOR_RESULT setWiper(float fResistance);

                    /*!
                     * @brief   Close.
                     *
                     * @details Put the AD5270 into shutdown. It will stop driving the MISO line and place it into high
                     *          impedance mode, allowing other peripherals to use it.
                     *
                     * @return  SENSOR_RESULT
                     *
                     * @note    There appears to be a hardware issue on the CN0357 where the AD5270 MUST be placed into
                     *          shutdown mode before the AD7790 can be used. So be sure to call open and close before
                     *          using an AD7790 functions.
                     *
                     */
                    SENSOR_RESULT close();

                private:

                    /*!
                     * @brief   Send data.
                     *
                     * @details Issue a command over the SPI bus that has data to be sent to the part.
                     *
                     * @param [in] nCommand : The command to send, must be one of the following:
                     *               - AD5270_COMMAND_WRITE_CTL_REG
                     *               - AD5270_COMMAND_WRITE_RDAC_REG
                     *               - AD5270_COMMAND_SHUTDOWN
                     *               - AD5270_COMMAND_NOOP
                     *
                     * @param [in] nData : The data to send with the command. See the datasheet for more details.
                     *
                     * @return  SENSOR_RESULT
                     *
                     */
                    SENSOR_RESULT sendData(uint8_t nCommand, uint16_t nData);

                    /*!
                     * @brief   Receive data.
                     *
                     * @details Issue a command over the SPI bus that has data to be recevied from the part.
                     *
                     * @param [in] nCommand : The command to send, must be one of the following:
                     *               - AD5270_COMMAND_READ_RDAC_REG
                     *
                     * @param [in] nData : The data to send with the command. See the datasheet for more details.
                     *
                     * @param [in] pResult : Pointer to the data to be received (allocated by caller).
                     *
                     * @return  SENSOR_RESULT
                     *
                     */
                    SENSOR_RESULT receiveData(uint8_t nCommand, uint16_t nData, uint16_t * pResult);

                    /*! GPIO driver memory */
                    uint8_t             m_gpio_memory[ADI_GPIO_MEMORY_SIZE];
                    /*! SPI configuration, moved off of stack */
                    ADI_SPI_TRANSCEIVER m_rheostat_spi_config;
                    /*! Pointer to SPI handle, passed in constructor */
                    ADI_SPI_HANDLE    * m_rheostat_spi_phandle;
            };
#pragma pack(pop)

            /*! AD5270 rheostat instance */
            AD5270         m_rheostat;
            /*! AD7790 ADC instance */
            AD7790         m_adc;
            /*! SPI driver memory */
            uint8_t        m_spi_memory[ADI_SPI_MEMORY_SIZE];
            /*! SPI driver handle */
            ADI_SPI_HANDLE m_spi_handle;

    };
#pragma pack(pop)
}


#endif /* ADI_CN0357_H */

