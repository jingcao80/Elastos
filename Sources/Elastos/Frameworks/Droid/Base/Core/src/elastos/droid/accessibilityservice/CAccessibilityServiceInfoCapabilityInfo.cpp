#include "elastos/droid/accessibilityservice/CAccessibilityServiceInfoCapabilityInfo.h"

namespace Elastos {
namespace Droid {
namespace AccessibilityService {

CAR_INTERFACE_IMPL(CAccessibilityServiceInfoCapabilityInfo, Object, IAccessibilityServiceInfoCapabilityInfo)

CAR_OBJECT_IMPL(CAccessibilityServiceInfoCapabilityInfo)

CAccessibilityServiceInfoCapabilityInfo::CAccessibilityServiceInfoCapabilityInfo()
    : mCapability(0)
    , mTitleResId(0)
    , mDescResId(0)
{}

CAccessibilityServiceInfoCapabilityInfo::~CAccessibilityServiceInfoCapabilityInfo()
{}

ECode CAccessibilityServiceInfoCapabilityInfo::constructor(
    /* [in] */ Int32 capability,
    /* [in] */ Int32 titleResId,
    /* [in] */ Int32 descResId)
{
    mCapability = capability;
    mTitleResId = titleResId;
    mDescResId = descResId;
    return NOERROR;
}

ECode CAccessibilityServiceInfoCapabilityInfo::GetCapability(
    /*[out]*/ Int32* capability)
{
    VALIDATE_NOT_NULL(capability);
    *capability = mCapability;
    return NOERROR;
}

ECode CAccessibilityServiceInfoCapabilityInfo::GetTitleResId(
    /*[out]*/ Int32* titleResId)
{
    VALIDATE_NOT_NULL(titleResId);
    *titleResId = mTitleResId;
    return NOERROR;
}

ECode CAccessibilityServiceInfoCapabilityInfo::GetDescResId(
    /*[out]*/ Int32* descResId)
{
    VALIDATE_NOT_NULL(descResId);
    *descResId = mDescResId;
    return NOERROR;
}

}
}
}
