/*!
 *****************************************************************************
  @file adi_gas.h

  @brief Gas class definition.

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


#ifndef ADI_GAS_H
#define ADI_GAS_H


#include <base_sensor/adi_sensor.h>


namespace adi_sensor_swpack
{

    /*!
     * @class Gas
     *
     * @brief Gas sensor.
     *
     * @details Functions and member variables that must be present in all gas sensor
     *          implementations.
     *
     */
    class Gas : public Sensor
    {
        public:

            /*!
             * @brief   Initialize the gas sensor.
             *
             * @details This function will initialize the underlying hardware used to
             *          communicate with the gas sensor and configure the hardware to
             *          the desired settings specified in the static configuration.
             *
             * @note    This function must be called before any others.
             *
             * @return  SENSOR_RESULT
             *
             */
            virtual SENSOR_RESULT open() = 0;

            /*!
             * @brief   Place the gas sensor into continuous measurement mode.
             *
             * @details Puts the gas sensor in continuous measurement mode. If this
             *          function is not called, the gas sensor will only make a
             *          meaurement when getPPM is called. This will take more time,
             *          but save power.
             *
             * @return  SENSOR_RESULT
             *
             */
            virtual SENSOR_RESULT start() = 0;

            /*!
             * @brief   Place the gas sensor into single measurement mode.
             *
             * @details Puts the gas sensor in single measurement mode. This will
             *          cause each measurement to take longer, but will save power
             *          between calls to getPPM.
             *
             * @return  SENSOR_RESULT
             *
             */
            virtual SENSOR_RESULT stop() = 0;

            /*!
             * @brief   Uninitialize the gas sensor.
             *
             * @details Perform hardware uninitializations, allowing the hardware
             *          resources used by the gas sensor to be used elsewhere.
             *
             * @return  SENSOR_RESULT
             *
             */
            virtual SENSOR_RESULT close() = 0;

            /*!
             * @brief   Get gas concentration.
             *
             * @details Returns the gas concentration in parts per million.
             *
             * @param [in] pData : Pointer to the result (allocated by caller).
             *
             * @return  SENSOR_RESULT
             *
             */
            virtual SENSOR_RESULT getPPM(float * pData) = 0;
    };
}

#endif /* ADI_GAS_H */

