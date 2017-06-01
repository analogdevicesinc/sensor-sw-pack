/*!
 *****************************************************************************
  @file adi_adt7420.h

  @brief ADT7420 class definition.

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


#ifndef ADI_ADI7420_H
#define ADI_ADI7420_H

#include <temp/adi_temperature.h>
#include <drivers/i2c/adi_i2c.h>

namespace adi_sensor_swpack
{
    /**
     * @class ADT7420 Temperature Class
     *
     * @brief ADT7420 temperature class interface.
     *
     **/
    class ADT7420 : public Temperature
    {
        public:
            /*!< Pure virtual functions must be implemented by the derived class */
            virtual SENSOR_RESULT open();
            virtual SENSOR_RESULT start();
            virtual SENSOR_RESULT stop();
            virtual SENSOR_RESULT close();

            virtual SENSOR_RESULT getTemperature(uint8_t *pTemperature, const uint32_t sizeInBytes);
            virtual SENSOR_RESULT getTemperatureInFahrenheit(float *pTemperatureFahrenheit);

            virtual SENSOR_RESULT getTemperatureInCelsius(float *pTemperatureCelsius);

        private:

            /*!
              * @brief   Initialize I2C.
              *
              * @details This method is used to initialize I2C.
              */
            SENSOR_RESULT InitI2C();
            /*!
              * @brief   Reads the temperature into the supplied buffer.
            *
            	* @parm [in] pTemperatureCelsius : pointer to the data where temperature is stored
            	*
            	* @parm [in] sizeInBytesd
                          *
                          * @notes This method is used to initialize I2C.
                          */
            SENSOR_RESULT ReadTemperature(uint8_t *pTemperature, const uint32_t sizeInBytes);

            uint8_t        m_slave_addr;
            ADI_I2C_HANDLE m_i2c_handle;
            uint8_t        m_I2C_memory[ADI_I2C_MEMORY_SIZE];
    };
}

#endif /* ADI_ADI7420_H */
