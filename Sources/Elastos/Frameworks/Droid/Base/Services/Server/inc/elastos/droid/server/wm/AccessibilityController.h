
#ifndef __ELASTOS_DROID_SERVER_WM_ACCESSIBILITYCONTROLLER_H__
#define __ELASTOS_DROID_SERVER_WM_ACCESSIBILITYCONTROLLER_H__

#include "_Elastos.Droid.Server.h"
#include "elastos/droid/ext/frameworkext.h"
#include <elastos/core/Object.h>
#include <elastos/utility/etl/List.h>

using Elastos::Droid::Content::IContext;
using Elastos::Droid::View::Accessibility::IAccessibilityManagerAccessibilityStateChangeListener;
using Elastos::Droid::View::Accessibility::IAccessibilityManagerTouchExplorationStateChangeListener;

namespace Elastos {
namespace Droid {
namespace Server {
namespace Wm {

class AccessibilityController
    : public Object
    , public IAccessibilityManagerAccessibilityStateChangeListener
    , public IAccessibilityManagerTouchExplorationStateChangeListener
{
public:
    interface IAccessibilityStateChangedCallback : public IInterface
    {
        virtual CARAPI_(void) OnStateChanged(
            /* [in] */ Boolean accessibilityEnabled,
            /* [in] */ Boolean touchExplorationEnabled) = 0;
    };

public:
    AccessibilityController(
        /* [in] */ IContext* context);

    CARAPI_(Boolean) IsAccessibilityEnabled();

    CARAPI_(Boolean) IsTouchExplorationEnabled();

    CARAPI_(void) Dump(
        /* [in] */ IFileDescriptor* fd,
        /* [in] */ IPrintWriter* pw,
        /* [in] */ ArrayOf<String>* args);

    CARAPI_(void) AddStateChangedCallback(
        /* [in] */ IAccessibilityStateChangedCallback* cb);

    CARAPI_(void) RemoveStateChangedCallback(
        /* [in] */ IAccessibilityStateChangedCallback* cb);

    CARAPI OnAccessibilityStateChanged(
        /* [in] */ Boolean enabled);

    CARAPI OnTouchExplorationStateChanged(
        /* [in] */ Boolean enabled);

private:
    CARAPI_(void) FireChanged();

private:
    List<AutoPtr<IAccessibilityStateChangedCallback> > mChangeCallbacks;

    Boolean mAccessibilityEnabled;
    Boolean mTouchExplorationEnabled;
};

} // Wm
} // Server
} // Droid
} // Elastos

#endif //__ELASTOS_DROID_SERVER_WM_ACCESSIBILITYCONTROLLER_H__
