/*!
 *****************************************************************************
  @file adi_accelerometer.h

  @brief Accelerometer class definition

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


#ifndef ADI_ACCELEROMETER_H
#define ADI_ACCELEROMETER_H

#include <base_sensor/adi_sensor.h>

namespace adi_sensor_swpack
{

    /*!
     * @class Accelerometer.
     *
     * @brief Accelerometer class interface.
     *
     **/
    class Accelerometer : public Sensor
    {
        public:

            /*!
             *  @enum   AXL_ERROR_CODES.
             *
             *  @brief   Accelerometer Error Codes.
             *
             *  @details Error codes specific to the accelerometer class.
             */
            typedef enum
            {
                AXL_ERROR_INVALID_DEVID,      /*!< Invalid device ID       */
                AXL_ERROR_INVALID_DEVID_MST,  /*!< Invalid mems device ID  */
                AXL_ERROR_INVALID_PARTID,     /*!< Invalid part ID         */
            } AXL_ERROR_CODES;

            /*!
             *  @enum   AXL_DATA.
             *
             *  @brief   Accelerometer Data.
             *
             *  @details Accelerometer data, x,y,z axis values.
             */
            typedef struct
            {
                uint32_t x;     /*!< X axis acceleration value   */
                uint32_t y;     /*!< Y axis acceleration value   */
                uint32_t z;     /*!< Z axis acceleration value   */
            } AXL_DATA;

            /*!
             *  @enum   AXL_RNGE.
             *
             *  @brief   Accelerometer range.
             *
             *  @details Accelerometer range enumerations.
             */
            typedef enum
            {
                AXL_RANGE_2g = 0, /*!< 2G accelerometer range     */
                AXL_RANGE_4g,    /*!< 4G accelerometer range     */
                AXL_RANGE_8g,    /*!< 8g accelerometer range     */
            } AXL_RANGE;

            /*!
             *  @enum   AXL_ODR.
             *
             *  @brief   Accelerometer output data rate.
             *
             *  @details Accelerometer output data rate enumerations.
             */
            typedef enum
            {
                AXL_ODR_12_5,    /*!< Output data rate 12.5 Hz   */
                AXL_ODR_25,      /*!< Output data rate 25 Hz     */
                AXL_ODR_50,      /*!< Output data rate 50 Hz     */
                AXL_ODR_100,     /*!< Output data rate 100 Hz    */
                AXL_ODR_200,     /*!< Output data rate 200 Hz    */
                AXL_ODR_400,     /*!< Output data rate 400 Hz    */
            } AXL_ODR;


            /*!
             * @brief    Get X-axis acceleration of the accelerometer.
             *
             * @param [in]  pBuffer : Buffer to get the x-value of the acceleration.
             *
             * @param [in]  sizeInBytes : Specifies the size of the accelerometer data. For
             *              16-bit data the value would be 2 and for 32-bit the value would be 4.
             *
             * @return   SENSOR_RESULT  In case of success SENSOR_ERROR_NONE is returned. Upon
             *           failure applications must use GET_SENSOR_ERROR_TYPE() and
             *           GET_DRIVER_ERROR_CODE() to determine the error.
             *
             * @details  getX function can be called after accelerometer is opened and started.
             *           This function returns the X-acceleration value.
             */
            virtual SENSOR_RESULT getX(uint8_t *pBuffer, const uint32_t sizeInBytes) = 0;

            /*!
             * @brief    Get Y-axis acceleration of the accelerometer.
             *
             * @param [in]  pBuffer : Buffer to get the x-value of the acceleration.
             *
             * @param [in]  sizeInBytes : Specifies the size of the accelerometer data. For
             *              16-bit data the value would be 2 and for 32-bit the value would be 4.
             *
             * @return   SENSOR_RESULT  In case of success SENSOR_ERROR_NONE is returned. Upon
             *           failure applications must use GET_SENSOR_ERROR_TYPE() and
             *           GET_DRIVER_ERROR_CODE() to determine the error.
             *
             * @details  getY function can be called after accelerometer is opened and started.
             *           This function returns the Y-acceleration value.
             */
            virtual SENSOR_RESULT getY(uint8_t *pBuffer, const uint32_t sizeInBytes) = 0;

            /*!
             * @brief    Get Z-axis acceleration of the accelerometer.
             *
             * @param [in]  pBuffer : Buffer to get the x-value of the acceleration.
             *
             * @param [in]  sizeInBytes : Specifies the size of the accelerometer data.For
             *              16-bit data the value would be 2 and for 32-bit the value would be 4.
             *
             * @return   SENSOR_RESULT  In case of success SENSOR_ERROR_NONE is returned. Upon
             *           failure applications must use GET_SENSOR_ERROR_TYPE() and
             *           GET_DRIVER_ERROR_CODE() to determine the error.
             *
             * @details  getZ function can be called after accelerometer is opened and started.
             *           This function returns the Z-acceleration value.
             */
            virtual SENSOR_RESULT getZ(uint8_t *pBuffer, const uint32_t sizeInBytes) = 0;

            /*!
             * @brief  Returns all three axes (x,y,z) through a pointer.
             *
             * @param [in]  pBuffer : Pointer to the buffer.
             *
             * @param [in]  sizeInBytes : Specifies the size of the accelerometer data. For
             *              16-bit data the value would be 2 and for 32-bit the value would be 4.
             *
             * @return  Upon successful result pBuffer contains the acceleration values of
             *          X,Y,Z axis in the supplied buffer. The buffer should be large enough
             *          to hold the accelerometer data.
             *
             * @details
             */
            virtual SENSOR_RESULT  getXYZ(uint8_t *pBuffer, const uint32_t sizeInBytes) = 0;

            /*!
             * @brief   Set the current range of the accelerometer.
             *
             * @param   range  : Specifies the accelerometer range. Range is enumerated via
             *          AXL_RANGE.
             *
             */
            void setRange(const AXL_RANGE range)
            {
                m_range = range;
            }

            /*!
             * @brief    Get accelerometer range.
             *
             * @return   Range value.
             *
             * @details  This method returns the accelerometer range value.
             */
            AXL_RANGE getRange(void)
            {
                return m_range;
            }


            /*!
              * @brief    Reads accelerometer register value.
              *
              * @param [in] regAddr : Register address of the accelerometer.
              *
              * @param [in] pBuffer : Pointer to the buffer where the register value is stored.
              *
              * @param [in] sizeInBytes : Accelerometer register size in bytes.
              *
              * @return   Returns non-zero value upon failure.
              *
              * @details  Read register uses underlying peripheral to read the accelerometer
              *           registers.
              */
            virtual SENSOR_RESULT  readRegister(const uint32_t regAddr, uint8_t *pBuffer, uint32_t sizeInBytes) = 0;

            /*!
              * @brief    Writes accelerometer register value.
              *
              * @param [in] regAddr : Register address of the accelerometer.
              *
              * @param [in] regValue : Value to be written to the register.
              *
              * @param [in] sizeInBytes : Accelerometer register size in bytes.
              *
              * @return   Returns non-zero value upon failure.
              *
              * @details  Writes the supplied value to the specified accelerometer register.
              */
            virtual SENSOR_RESULT  writeRegister(const uint8_t regAddr, const uint32_t regValue, uint32_t sizeInBytes) = 0;

            /*!
             * @brief    Opens the accelerometer and configures it.
             *
             * @return   SENSOR_RESULT  In case of success SENSOR_ERROR_NONE is returned. Upon
             *           failure applications must use GET_SENSOR_ERROR_TYPE() and
             *           GET_DRIVER_ERROR_CODE() to determine the error.
             *
             * @details  Initializes the accelerometer and configures it. Accelerometer's
             *           use serial interfaces like SPI or I2C. This function opens the
             *           underlying peripheral and configures it with the default configuration
             *           parameters. Default configuration parameters are statically defined in
             *           the associated sensor class configuration header.For example the
             *           configuration for ADXL362 is defined in adi_adxl362_cfg.h and the sensor
             *           software is implemented in adi_adxl362.cpp
             */
            virtual SENSOR_RESULT   open() = 0;

            /*!
             * @brief   Start function puts the accelerometer in measurement mode.
             *
             * @return   SENSOR_RESULT  In case of success SENSOR_ERROR_NONE is returned. Upon
             *           failure applications must use GET_SENSOR_ERROR_TYPE() and
             *           GET_DRIVER_ERROR_CODE() to determine the error.
             *
             * @details  Start function should be called only after the accelerometer is
             *           successfully opened. Start puts the accelerometer in measurement mode.
             *           Once in measurement mode accelerometer's measure acceleration on x,y,z
             *           axis.
             */
            virtual SENSOR_RESULT   start() = 0;

            /*!
             * @brief    Stops the accelerometer and accelerometer is kept in standby mode.
             *
             * @return   SENSOR_RESULT  In case of success SENSOR_ERROR_NONE is returned. Upon
             *           failure applications must use GET_SENSOR_ERROR_TYPE() and
             *           GET_DRIVER_ERROR_CODE() to determine the error.
             *
             * @details  Stop function of an accelerometer puts the accelerometer in standby
             *           mode. With this accelerometer no longer measures the acceleration.
             *           Applications can issue start function to enable measurement mode.
             */
            virtual SENSOR_RESULT   stop() = 0;

            /*!
             * @brief    Stops the accelerometer and closes the underlying peripheral
             *
             * @return   SENSOR_RESULT  In case of success SENSOR_ERROR_NONE is returned. Upon
             *           failure applications must use GET_SENSOR_ERROR_TYPE() and
             *           GET_DRIVER_ERROR_CODE() to determine the error.
             *
             * @details  Close function stops the measurements and accelerometer is kept in
             *           stanby mode. Underlying peripheral is closed. Applications must use
             *           open again inorder to enable the measurement mode.
             */
            virtual SENSOR_RESULT   close() = 0;


        protected:
            uint32_t      m_x_axis;  /*!< x axis */
            uint32_t      m_y_axis;  /*!< y axis */
            uint32_t      m_z_axis;  /*!< z axis */
            AXL_RANGE     m_range;   /*!< accelerometer range */

    };
}

#endif /* ADI_ACCELEROMETER_H */
/* @} */
