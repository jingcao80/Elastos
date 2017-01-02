//=========================================================================
// Copyright (C) 2012 The Elastos Open Source Project
//
// Licensed under the Apache License, Version 2.0 (the "License");
// you may not use this file except in compliance with the License.
// You may obtain a copy of the License at
//
//      http://www.apache.org/licenses/LICENSE-2.0
//
// Unless required by applicable law or agreed to in writing, software
// distributed under the License is distributed on an "AS IS" BASIS,
// WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
// See the License for the specific language governing permissions and
// limitations under the License.
//=========================================================================

#ifndef __ELASTOS_DROID_HARDWARE_SERIALMANAGER_H__
#define __ELASTOS_DROID_HARDWARE_SERIALMANAGER_H__

#include "Elastos.Droid.Hardware.h"
#include "elastos/droid/ext/frameworkext.h"
#include <elastos/core/Object.h>

using Elastos::Droid::Content::IContext;

namespace Elastos {
namespace Droid {
namespace Hardware {

class SerialManager
    : public Object
    , public ISerialManager
{
public:
    CAR_INTERFACE_DECL()

    SerialManager();

    ~SerialManager();

    /**
     * {@hide}
     */
    CARAPI constructor(
        /* [in] */ IContext* context,
        /* [in] */ IISerialManager* service);

    /**
     * Returns a string array containing the names of available serial ports
     *
     * @return names of available serial ports
     */
    CARAPI GetSerialPorts(
        /* [out, callee] */ ArrayOf<String>** ports);

    /**
     * Opens and returns the {@link android.hardware.SerialPort} with the given name.
     * The speed of the serial port must be one of:
     * 50, 75, 110, 134, 150, 200, 300, 600, 1200, 1800, 2400, 4800, 9600,
     * 19200, 38400, 57600, 115200, 230400, 460800, 500000, 576000, 921600, 1000000, 1152000,
     * 1500000, 2000000, 2500000, 3000000, 3500000 or 4000000
     *
     * @param name of the serial port
     * @param speed at which to open the serial port
     * @return the serial port
     */
    CARAPI OpenSerialPort(
        /* [in] */ const String& name,
        /* [in] */ Int32 speed,
        /* [out] */ ISerialPort** port);

private:
    static const String TAG;

    IContext* mContext;
    AutoPtr<IISerialManager> mService;
};

} // namespace Hardware
} // namespace Droid
} // namespace Elastos

#endif //__ELASTOS_DROID_HARDWARE_SERIALMANAGER_H__
