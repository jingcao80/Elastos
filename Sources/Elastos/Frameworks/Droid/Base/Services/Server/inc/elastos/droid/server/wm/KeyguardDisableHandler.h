#ifndef __ELASTOS_DROID_SERVER_WM_KEYGURADDISABLEHANDLER_H__
#define __ELASTOS_DROID_SERVER_WM_KEYGURADDISABLEHANDLER_H__

#include "elastos/droid/os/HandlerBase.h"
#include "elastos/droid/os/TokenWatcher.h"

using Elastos::Droid::Os::TokenWatcher;
using Elastos::Droid::Content::IContext;
using Elastos::Droid::View::IWindowManagerPolicy;

namespace Elastos {
namespace Droid {
namespace Server {
namespace Wm {

class KeyguardDisableHandler : public HandlerBase
{
public:
    class KeyguardTokenWatcher : public TokenWatcher
    {
    public:
        KeyguardTokenWatcher(
            /* [in] */ KeyguardDisableHandler* handler);

        CARAPI Acquired();

        CARAPI Released();

    private:
        KeyguardDisableHandler* mHost;
    };

public:
    KeyguardDisableHandler(
        /* [in] */ IContext* context,
        /* [in] */ IWindowManagerPolicy* policy);

    virtual CARAPI HandleMessage(
        /* [in] */ IMessage* msg);

public:
    // Message.what constants
    static const Int32 KEYGUARD_DISABLE = 1;
    static const Int32 KEYGUARD_REENABLE = 2;
    static const Int32 KEYGUARD_POLICY_CHANGED = 3;

    AutoPtr<IContext> mContext;
    AutoPtr<IWindowManagerPolicy> mPolicy;
    AutoPtr<KeyguardTokenWatcher> mKeyguardTokenWatcher;

private:
    static const String TAG;

    static const Int32 ALLOW_DISABLE_YES;
    static const Int32 ALLOW_DISABLE_NO;
    static const Int32 ALLOW_DISABLE_UNKNOWN; // check with DevicePolicyManager
    Int32 mAllowDisableKeyguard; // sync'd by mKeyguardTokenWatcher
};

} // Wm
} // Server
} // Droid
} // Elastos

#endif //__ELASTOS_DROID_SERVER_WM_KEYGURADDISABLEHANDLER_H__
