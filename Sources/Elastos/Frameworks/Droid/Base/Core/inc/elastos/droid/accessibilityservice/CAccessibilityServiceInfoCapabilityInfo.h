#ifndef __ELASTOS_DROID_ACCESSIBILITYSERVICE_CACCESSIBILITYSERVICEINFOCAPABILITYINFO_H__
#define __ELASTOS_DROID_ACCESSIBILITYSERVICE_CACCESSIBILITYSERVICEINFOCAPABILITYINFO_H__

#include "elastos/droid/ext/frameworkdef.h"
#include "_Elastos_Droid_AccessibilityService_CAccessibilityServiceInfoCapabilityInfo.h"
#include <elastos/core/Object.h>

namespace Elastos {
namespace Droid {
namespace AccessibilityService {

CarClass(CAccessibilityServiceInfoCapabilityInfo)
    , public Object
    , public IAccessibilityServiceInfoCapabilityInfo
{
public:
    CAR_INTERFACE_DECL()

    CAR_OBJECT_DECL()

    CAccessibilityServiceInfoCapabilityInfo();

    ~CAccessibilityServiceInfoCapabilityInfo();

    CARAPI constructor(
        /* [in] */ Int32 capability,
        /* [in] */ Int32 titleResId,
        /* [in] */ Int32 descResId);

    CARAPI GetCapability(
        /*[out]*/ Int32* capability);

    CARAPI GetTitleResId(
        /*[out]*/ Int32* titleResId);

    CARAPI GetDescResId(
        /*[out]*/ Int32* descResId);

public:
    Int32 mCapability;
    Int32 mTitleResId;
    Int32 mDescResId;
};

}
}
}

#endif // __ELASTOS_DROID_ACCESSIBILITYSERVICE_CACCESSIBILITYSERVICEINFOCAPABILITYINFO_H__
