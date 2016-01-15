#ifndef __ELASTOS_DROID_VIEW_ACCESSIBILITY_CACCESSIBILITYEVENTHELPER_H__
#define __ELASTOS_DROID_VIEW_ACCESSIBILITY_CACCESSIBILITYEVENTHELPER_H__

#include "_Elastos_Droid_View_Accessibility_CAccessibilityEventHelper.h"
#include <elastos/core/Singleton.h>

namespace Elastos {
namespace Droid {
namespace View {
namespace Accessibility {

CarClass(CAccessibilityEventHelper)
    , public Singleton
    , public IAccessibilityEventHelper
{
public:
    CAR_INTERFACE_DECL()

    CAR_SINGLETON_DECL()

    /**
     * Returns a cached instance if such is available or a new one is
     * instantiated with its type property set.
     *
     * @param eventType The event type.
     * @return An instance.
     */
    CARAPI Obtain(
        /* [in] */ Int32 eventType,
        /* [out] */ IAccessibilityEvent** event);

    /**
     * Returns a cached instance if such is available or a new one is
     * created. The returned instance is initialized from the given
     * <code>event</code>.
     *
     * @param event The other event.
     * @return An instance.
     */
    CARAPI Obtain(
        /* [in] */ IAccessibilityEvent* otherEvent,
        /* [out] */ IAccessibilityEvent** event);

    /**
     * Returns a cached instance if such is available or a new one is
     * instantiated.
     *
     * @return An instance.
     */
    CARAPI Obtain(
        /* [out] */ IAccessibilityEvent** event);

    /**
     * Returns the string representation of an event type. For example,
     * {@link #TYPE_VIEW_CLICKED} is represented by the string TYPE_VIEW_CLICKED.
     *
     * @param eventType The event type
     * @return The string representation.
     */
    CARAPI EventTypeToString(
        /* [in] */ Int32 eventType,
        /* [out] */ String* str);
};

} // Accessibility
} // View
} // Droid
} // Elastos

#endif //__ELASTOS_DROID_VIEW_ACCESSIBILITY_CACCESSIBILITYEVENTHELPER_H__
