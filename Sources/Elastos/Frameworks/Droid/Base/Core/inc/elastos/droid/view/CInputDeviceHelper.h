
#ifndef __ELASTOS_DROID_VIEW_CINPUTDEVICEHELPER_H__
#define __ELASTOS_DROID_VIEW_CINPUTDEVICEHELPER_H__

#include "_Elastos_Droid_View_CInputDeviceHelper.h"
#include <elastos/core/Singleton.h>

using Elastos::Core::Singleton;

namespace Elastos {
namespace Droid {
namespace View {

CarClass(CInputDeviceHelper)
    , public Singleton
    , public IInputDeviceHelper
{
public:
    CAR_INTERFACE_DECL()

    CAR_SINGLETON_DECL()

    /**
     * Gets information about the input device with the specified id.
     * @param id The device id.
     * @return The input device or NULL if not found.
     */
    CARAPI GetDevice(
        /* [in] */ Int32 id,
        /* [out] */ IInputDevice** device);

    /**
     * Gets the ids of all input devices in the system.
     * @return The input device ids.
     */
    CARAPI GetDeviceIds(
        /* [out, callee] */ ArrayOf<Int32>** deviceIds);
};

}   //namespace View
}   //namespace Droid
}   //namespace Elastos

#endif //__ELASTOS_DROID_VIEW_CINPUTDEVICEHELPER_H__
