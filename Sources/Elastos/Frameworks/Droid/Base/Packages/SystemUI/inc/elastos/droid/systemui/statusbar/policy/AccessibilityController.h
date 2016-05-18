
#ifndef __ELASTOS_DROID_SYSTEMUI_STATUSBAR_POLICY_ACCESSIBILITYCONTROLLER_H__
#define __ELASTOS_DROID_SYSTEMUI_STATUSBAR_POLICY_ACCESSIBILITYCONTROLLER_H__

#include "_Elastos.Droid.SystemUI.h"
#include "Elastos.Droid.View.h"
#include <elastos/droid/ext/frameworkext.h>
#include <elastos/core/Object.h>

using Elastos::Droid::Content::IContext;
using Elastos::Droid::View::Accessibility::IAccessibilityManagerAccessibilityStateChangeListener;
using Elastos::Droid::View::Accessibility::IAccessibilityManagerTouchExplorationStateChangeListener;
using Elastos::Core::Object;
using Elastos::IO::IFileDescriptor;
using Elastos::IO::IPrintWriter;
using Elastos::Utility::IArrayList;

namespace Elastos {
namespace Droid {
namespace SystemUI {
namespace StatusBar {
namespace Policy {

class AccessibilityController
    : public Object
    , public IAccessibilityController
    , public IAccessibilityManagerAccessibilityStateChangeListener
    , public IAccessibilityManagerTouchExplorationStateChangeListener
{
public:
    CAR_INTERFACE_DECL()

    AccessibilityController(
        /* [in] */ IContext* context);

    CARAPI IsAccessibilityEnabled(
        /* [out] */ Boolean* enabled);

    CARAPI IsTouchExplorationEnabled(
        /* [out] */ Boolean* enabled);

    CARAPI Dump(
        /* [in] */ IFileDescriptor* fd,
        /* [in] */ IPrintWriter* pw,
        /* [in] */ ArrayOf<String>* args);

    CARAPI AddStateChangedCallback(
        /* [in] */ IAccessibilityStateChangedCallback* cb);

    CARAPI RemoveStateChangedCallback(
        /* [in] */ IAccessibilityStateChangedCallback* cb);

    // @Override
    CARAPI OnAccessibilityStateChanged(
        /* [in] */ Boolean enabled);

    // @Override
    CARAPI OnTouchExplorationStateChanged(
        /* [in] */ Boolean enabled);

private:
    CARAPI_(void) FireChanged();

private:
    AutoPtr<IArrayList> mChangeCallbacks;  /*<AccessibilityStateChangedCallback*/

    Boolean mAccessibilityEnabled;
    Boolean mTouchExplorationEnabled;
};

} // namespace Policy
} // namespace StatusBar
} // namespace SystemUI
} // namespace Droid
} // namespace Elastos

#endif // __ELASTOS_DROID_SYSTEMUI_STATUSBAR_POLICY_ACCESSIBILITYCONTROLLER_H__
