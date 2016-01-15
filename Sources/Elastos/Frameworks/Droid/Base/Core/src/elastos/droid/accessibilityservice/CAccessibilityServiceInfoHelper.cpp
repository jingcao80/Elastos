#include "elastos/droid/accessibilityservice/CAccessibilityServiceInfoHelper.h"
#include "elastos/droid/accessibilityservice/CAccessibilityServiceInfo.h"

namespace Elastos {
namespace Droid {
namespace AccessibilityService {

CAR_INTERFACE_IMPL(CAccessibilityServiceInfoHelper, Singleton, IAccessibilityServiceInfoHelper)

CAR_SINGLETON_IMPL(CAccessibilityServiceInfoHelper)

ECode CAccessibilityServiceInfoHelper::FeedbackTypeToString(
    /* [in] */ Int32 feedbackType,
    /* [out] */ String* str)
{
    VALIDATE_NOT_NULL(str);
    return CAccessibilityServiceInfo::FeedbackTypeToString(feedbackType, str);
}

ECode CAccessibilityServiceInfoHelper::FlagToString(
    /* [in] */ Int32 flag,
    /* [out] */ String* str)
{
    VALIDATE_NOT_NULL(str);
    return CAccessibilityServiceInfo::FlagToString(flag, str);
}

ECode CAccessibilityServiceInfoHelper::CapabilityToString(
    /* [in] */ Int32 capability,
    /* [out] */ String* str)
{
    VALIDATE_NOT_NULL(str);
    return CAccessibilityServiceInfo::CapabilityToString(capability, str);
}
}
}
}
