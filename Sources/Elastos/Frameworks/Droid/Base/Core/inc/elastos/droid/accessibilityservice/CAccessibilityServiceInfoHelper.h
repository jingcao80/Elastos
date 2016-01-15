#ifndef __ELASTOS_DROID_ACCESSIBILITYSERVICE_CACCESSIBILITYSERVICEINFOHELPER_H__
#define __ELASTOS_DROID_ACCESSIBILITYSERVICE_CACCESSIBILITYSERVICEINFOHELPER_H__

#include "_Elastos_Droid_AccessibilityService_CAccessibilityServiceInfoHelper.h"
#include <elastos/core/Singleton.h>

namespace Elastos {
namespace Droid {
namespace AccessibilityService {

CarClass(CAccessibilityServiceInfoHelper)
    , public Singleton
    , public IAccessibilityServiceInfoHelper
{
public:
    CAR_INTERFACE_DECL()

    CAR_SINGLETON_DECL()

    /**
     * Returns the string representation of a feedback type. For example,
     * {@link #FEEDBACK_SPOKEN} is represented by the string FEEDBACK_SPOKEN.
     *
     * @param feedbackType The feedback type.
     * @return The string representation.
     */
    CARAPI FeedbackTypeToString(
        /* [in] */ Int32 feedbackType,
        /* [out] */ String* str);

    /**
     * Returns the string representation of a flag. For example,
     * {@link #DEFAULT} is represented by the string DEFAULT.
     *
     * @param flag The flag.
     * @return The string representation.
     */
    CARAPI FlagToString(
        /* [in] */ Int32 flag,
        /* [out] */ String* str);

    /**
     * Returns the string representation of a capability. For example,
     * {@link #CAPABILITY_CAN_RETRIEVE_WINDOW_CONTENT} is represented
     * by the string CAPABILITY_CAN_RETRIEVE_WINDOW_CONTENT.
     *
     * @param capability The capability.
     * @return The string representation.
     */
    CARAPI CapabilityToString(
        /* [in] */ Int32 capability,
        /* [out] */ String* str);
};

}
}
}

#endif // __ELASTOS_DROID_ACCESSIBILITYSERVICE_CACCESSIBILITYSERVICEINFOHELPER_H__
