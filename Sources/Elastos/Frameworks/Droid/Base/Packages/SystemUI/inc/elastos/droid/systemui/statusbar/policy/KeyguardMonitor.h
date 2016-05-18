
#ifndef __ELASTOS_DROID_SYSTEMUI_STATUSBAR_POLICY_KEYGUARDMONITOR_H__
#define __ELASTOS_DROID_SYSTEMUI_STATUSBAR_POLICY_KEYGUARDMONITOR_H__

#include "_Elastos.Droid.SystemUI.h"
#include "Elastos.CoreLibrary.Utility.h"
#include <elastos/droid/ext/frameworkext.h>
#include <elastos/core/Object.h>

using Elastos::Core::Object;
using Elastos::Utility::IArrayList;

namespace Elastos {
namespace Droid {
namespace SystemUI {
namespace StatusBar {
namespace Policy {

class KeyguardMonitor
    : public Object
    , public IKeyguardMonitor
{
public:
    CAR_INTERFACE_DECL()

    KeyguardMonitor();

    CARAPI AddCallback(
        /* [in] */ IKeyguardMonitorCallback* callback);

    CARAPI RemoveCallback(
        /* [in] */ IKeyguardMonitorCallback* callback);

    CARAPI IsShowing(
        /* [out] */ Boolean* result);

    CARAPI IsSecure(
        /* [out] */ Boolean* result);

    CARAPI NotifyKeyguardState(
        /* [in] */ Boolean showing,
        /* [in] */ Boolean secure);

private:
    AutoPtr<IArrayList> mCallbacks;  /*<Callback*/

    Boolean mShowing;
    Boolean mSecure;
};

} // namespace Policy
} // namespace StatusBar
} // namespace SystemUI
} // namespace Droid
} // namespace Elastos

#endif // __ELASTOS_DROID_SYSTEMUI_STATUSBAR_POLICY_KEYGUARDMONITOR_H__
