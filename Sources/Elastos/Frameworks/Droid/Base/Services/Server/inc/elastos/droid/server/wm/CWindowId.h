#ifndef __ELASTOS_DROID_SERVER_WM_CWINDOWID_H__
#define __ELASTOS_DROID_SERVER_WM_CWINDOWID_H__

#include "_Elastos_Droid_Server_Wm_CWindowId.h"
#include "wm/WindowState.h"

using Elastos::Droid::View::IIWindowId;
using Elastos::Droid::View::IIWindowFocusObserver;
using Elastos::Droid::View::IWindowState;

namespace Elastos {
namespace Droid {
namespace Server {
namespace Wm {

CarClass(CWindowId)
    , public Object
    , public IIWindowId
{
public:
    CAR_INTERFACE_DECL()

    CAR_OBJECT_DECL()

    CARAPI RegisterFocusObserver(
        /* [in] */ IIWindowFocusObserver* observer);

    CARAPI UnregisterFocusObserver(
        /* [in] */ IIWindowFocusObserver* observer);

    CARAPI IsFocused(
        /* [out] */ Boolean* result);

    CARAPI constructor(
        /* [in] */ IWindowState* host);

private:
    AutoPtr<IWeakReference> mWeakOwner;
};

} // Wm
} // Server
} // Droid
} // Elastos

#endif //__ELASTOS_DROID_SERVER_WM_CWindowId_H__
