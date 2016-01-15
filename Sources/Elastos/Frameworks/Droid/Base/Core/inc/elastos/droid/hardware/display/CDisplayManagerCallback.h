#ifndef __ELASTOS_DROID_HARDWARE_DISPLAY_CDISPLAYMANAGERCALLBACK_H__
#define __ELASTOS_DROID_HARDWARE_DISPLAY_CDISPLAYMANAGERCALLBACK_H__

#include "_Elastos_Droid_Hardware_Display_CDisplayManagerCallback.h"
#include "elastos/droid/hardware/display/DisplayManagerGlobal.h"
#include <elastos/core/Object.h>


namespace Elastos {
namespace Droid {
namespace Hardware {
namespace Display {

CarClass(CDisplayManagerCallback)
    , public Object
    , public IIDisplayManagerCallback
{
public:
    CAR_INTERFACE_DECL()

    CAR_OBJECT_DECL()

    CARAPI constructor(
        /* [in] */ Handle32 owner);

    //@Override
    CARAPI OnDisplayEvent(
        /* [in] */ Int32 displayId,
        /* [in] */ Int32 event);

    CARAPI ToString(
        /* [out] */ String* str);

private:
    DisplayManagerGlobal* mOwner;
};

} // namespace Display
} // namespace Hardware
} // namepsace Droid
} // namespace Elastos

#endif //__ELASTOS_DROID_HARDWARE_DISPLAY_CDISPLAYMANAGERCALLBACK_H__
