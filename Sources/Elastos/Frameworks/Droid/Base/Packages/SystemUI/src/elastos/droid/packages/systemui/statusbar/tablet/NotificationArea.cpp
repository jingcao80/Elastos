#include "elastos/droid/systemui/statusbar/tablet/NotificationArea.h"
#include "elastos/droid/view/accessibility/CAccessibilityEventHelper.h"

using Elastos::Droid::View::Accessibility::IAccessibilityEventHelper;
using Elastos::Droid::View::Accessibility::CAccessibilityEventHelper;

namespace Elastos {
namespace Droid {
namespace SystemUI {
namespace StatusBar {
namespace Tablet {


NotificationArea::NotificationArea()
{}

NotificationArea::NotificationArea(
    /* [in] */ IContext* context,
    /* [in] */ IAttributeSet* attrs)
    : LinearLayout(context, attrs)
{}

Boolean NotificationArea::OnRequestSendAccessibilityEvent(
    /* [in] */ IView* child,
    /* [in] */ IAccessibilityEvent* event)
{
    if (LinearLayout::OnRequestSendAccessibilityEvent(child, event)) {
        // The event is coming from a descendant like battery but append
        // the content of the entire notification area so accessibility
        // services can choose how to present the content to the user.
        AutoPtr<IAccessibilityEventHelper> helper;
        CAccessibilityEventHelper::AcquireSingleton((IAccessibilityEventHelper**)&helper);
        AutoPtr<IAccessibilityEvent> record;
        helper->Obtain((IAccessibilityEvent**)&record);
        OnInitializeAccessibilityEvent(record);
        DispatchPopulateAccessibilityEvent(record);
        event->AppendRecord(record);
        return TRUE;
    }
    return FALSE;
}


}// namespace Tablet
}// namespace StatusBar
}// namespace SystemUI
}// namespace Droid
}// namespace Elastos
