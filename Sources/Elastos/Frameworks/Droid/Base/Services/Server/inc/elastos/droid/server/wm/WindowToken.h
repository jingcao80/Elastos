#ifndef __ELASTOS_DROID_SERVER_WM_WINDOWTOKEN_H__
#define __ELASTOS_DROID_SERVER_WM_WINDOWTOKEN_H__

#include "elastos/droid/ext/frameworkext.h"
#include "_Elastos.Droid.Server.h"
#include <elastos/utility/etl/List.h>

using Elastos::Utility::Etl::List;
using Elastos::Droid::Os::IBinder;

namespace Elastos {
namespace Droid {
namespace Server {
namespace Wm {

class AppWindowToken;
class CWindowManagerService;
class WindowState;

class WindowToken
    : public ElRefBase
    , public IWeakReferenceSource
{
public:
    CAR_INTERFACE_DECL()

    CARAPI GetWeakReference(
        /* [out] */ IWeakReference** weakReference);

    WindowToken(
        /* [in] */ CWindowManagerService* service,
        /* [in] */ IBinder* token,
        /* [in] */ Int32 type,
        /* [in] */ Boolean _explicit);

public:
    // The window manager!
    AutoPtr<CWindowManagerService> mService;

    // The actual token.
    AutoPtr<IBinder> mToken;

    // The type of window this token is for, as per WindowManager.LayoutParams.
    Int32 mWindowType;

    // Set if this token was explicitly added by a client, so should
    // not be removed when all windows are removed.
    Boolean mExplicit;

    // For printing.
    String mStringName;

    // If this is an AppWindowToken, this is non-null.
    AppWindowToken* mAppWindowToken;

    // All of the windows associated with this token.
    List< AutoPtr<WindowState> > mWindows;

    // Is key dispatching paused for this token?
    Boolean mPaused;

    // Should this token's windows be hidden?
    Boolean mHidden;

    // Temporary for finding which tokens no longer have visible windows.
    Boolean mHasVisible;

    // Set to true when this token is in a pending transaction where it
    // will be shown.
    Boolean mWaitingToShow;

    // Set to true when this token is in a pending transaction where it
    // will be hidden.
    Boolean mWaitingToHide;

    // Set to true when this token is in a pending transaction where its
    // windows will be put to the bottom of the list.
    Boolean mSendingToBottom;

    // Set to true when this token is in a pending transaction where its
    // windows will be put to the top of the list.
    Boolean mSendingToTop;

};

} // Wm
} // Server
} // Droid
} // Elastos

#endif //__ELASTOS_DROID_SERVER_WM_WINDOWTOKEN_H__
