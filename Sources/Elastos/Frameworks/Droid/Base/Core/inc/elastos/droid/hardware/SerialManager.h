
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

    AutoPtr<IContext> mContext;
    AutoPtr<IISerialManager> mService;
};

} // namespace Hardware
} // namespace Droid
} // namespace Elastos

#endif //__ELASTOS_DROID_HARDWARE_SERIALMANAGER_H__
