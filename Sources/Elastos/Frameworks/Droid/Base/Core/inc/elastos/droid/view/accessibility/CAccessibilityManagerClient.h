#ifndef __ELASTOS_DROID_VIEW_ACCESSIBILITY_CACCESSIBILITYMANAGERCLIENT_H__
#define __ELASTOS_DROID_VIEW_ACCESSIBILITY_CACCESSIBILITYMANAGERCLIENT_H__

#include "_Elastos_Droid_View_Accessibility_CAccessibilityManagerClient.h"
#include "elastos/droid/view/accessibility/CAccessibilityManager.h"

using Elastos::Droid::Os::IBinder;

namespace Elastos {
namespace Droid {
namespace View {
namespace Accessibility {

CarClass(CAccessibilityManagerClient)
    , public Object
    , public IIAccessibilityManagerClient
    , public IBinder
{
public:
    CAR_INTERFACE_DECL()

    CAR_OBJECT_DECL()

    CAccessibilityManagerClient();

    ~CAccessibilityManagerClient();

    CARAPI constructor(
        /* [in] */ IAccessibilityManager* host);

    CARAPI SetState(
        /* [in] */ Int32 stateFlags);

    CARAPI ToString(
        /* [out] */ String* str);

private:
    AutoPtr<CAccessibilityManager> mHost;
};

} // Accessibility
} // View
} // Droid
} // Elastos

#endif //__ELASTOS_DROID_VIEW_ACCESSIBILITY_CACCESSIBILITYMANAGERCLIENT_H__
