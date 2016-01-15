
#include "Elastos.Droid.Accounts.h"
#include "Elastos.Droid.App.h"
#include "Elastos.Droid.Content.h"
#include "Elastos.Droid.Location.h"
#include "Elastos.Droid.Os.h"
#include "Elastos.Droid.Widget.h"
#include "elastos/droid/ext/frameworkdef.h"
#include "elastos/droid/view/accessibility/CAccessibilityEventHelper.h"
#include "elastos/droid/view/accessibility/CAccessibilityEvent.h"

namespace Elastos {
namespace Droid {
namespace View {
namespace Accessibility {

CAR_INTERFACE_IMPL(CAccessibilityEventHelper, Singleton, IAccessibilityEventHelper)

CAR_SINGLETON_IMPL(CAccessibilityEventHelper)

ECode CAccessibilityEventHelper::Obtain(
    /* [in] */ Int32 eventType,
    /* [out] */ IAccessibilityEvent** event)
{
    VALIDATE_NOT_NULL(event);
    return CAccessibilityEvent::Obtain(eventType, event);
}

ECode CAccessibilityEventHelper::Obtain(
    /* [in] */ IAccessibilityEvent* otherEvent,
    /* [out] */ IAccessibilityEvent** event)
{
    VALIDATE_NOT_NULL(event);
    return CAccessibilityEvent::Obtain(otherEvent, event);
}

ECode CAccessibilityEventHelper::Obtain(
    /* [out] */ IAccessibilityEvent** event)
{
    VALIDATE_NOT_NULL(event);
    return CAccessibilityEvent::Obtain(event);
}

ECode CAccessibilityEventHelper::EventTypeToString(
    /* [in] */ Int32 eventType,
    /* [out] */ String* str)
{
    VALIDATE_NOT_NULL(str);
    *str = CAccessibilityEvent::EventTypeToString(eventType);
    return NOERROR;
}

} // Accessibility
} // View
} // Droid
} // Elastos
