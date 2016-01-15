#include "elastos/droid/systemui/statusbar/LatestItemView.h"
#include "elastos/droid/view/accessibility/CAccessibilityEventHelper.h"

using Elastos::Droid::View::Accessibility::IAccessibilityEventHelper;
using Elastos::Droid::View::Accessibility::CAccessibilityEventHelper;

namespace Elastos {
namespace Droid {
namespace SystemUI {
namespace StatusBar {


LatestItemView::LatestItemView()
{}

LatestItemView::LatestItemView(
    /* [in] */ IContext* context,
    /* [in] */ IAttributeSet* attrs)
    : FrameLayout(context, attrs)
{}

ECode LatestItemView::SetOnClickListener(
    /* [in] */ IViewOnClickListener* l)
{
    return FrameLayout::SetOnClickListener(l);
}

Boolean LatestItemView::OnRequestSendAccessibilityEvent(
    /* [in] */ IView* child,
    /* [in] */ IAccessibilityEvent* event)
{
    Boolean result = FrameLayout::OnRequestSendAccessibilityEvent(child, event);
    if (result) {
        // Add a record for the entire layout since its content is somehow small.
        // The event comes from a leaf view that is interacted with.
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


}// namespace StatusBar
}// namespace SystemUI
}// namespace Droid
}// namespace Elastos
