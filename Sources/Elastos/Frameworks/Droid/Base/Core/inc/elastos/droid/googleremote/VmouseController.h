
#ifndef __ELASTOS_DROID_GOOGLEREMOTE_VMOUCECONTROLLER_H__
#define __ELASTOS_DROID_GOOGLEREMOTE_VMOUCECONTROLLER_H__

#include <elastos.h>

namespace Elastos {
namespace Droid {
namespace GoogleRemote {

class VmouseController : public ElRefBase
{
public:
    VmouseController() {}

    CARAPI_(void) OpenVmouse();

    CARAPI_(void) CloseVmouse();

    CARAPI_(void) DispatchMouseEvent(
        /* [in] */ Int32 x,
        /* [in] */ Int32 y,
        /* [in] */ Int32 flag);

private:
    CARAPI_(void) NativeDispatchMouseEvent(
        /* [in] */ Int32 x,
        /* [in] */ Int32 y,
        /* [in] */ Int32 flag);

    CARAPI_(void) NativeOpenVmouse();

    CARAPI_(void) NativeCloseVmouse();
};

} // GoogleRemote
} // Droid
} // Elastos

#endif // __ELASTOS_DROID_GOOGLEREMOTE_VMOUCECONTROLLER_H__
