
#include "elastos/droid/systemui/statusbar/CNotificationOverflowContainer.h"
#include "R.h"

using Elastos::Droid::SystemUI::StatusBar::Phone::IIconMerger;
using Elastos::Droid::Widget::ITextView;

namespace Elastos {
namespace Droid {
namespace SystemUI {
namespace StatusBar {

CAR_OBJECT_IMPL(CNotificationOverflowContainer)
CAR_INTERFACE_IMPL(CNotificationOverflowContainer, ActivatableNotificationView, INotificationOverflowContainer)
ECode CNotificationOverflowContainer::constructor(
    /* [in] */ IContext* context,
    /* [in] */ IAttributeSet* attrs)
{
    return ActivatableNotificationView::constructor(context, attrs);
}

ECode CNotificationOverflowContainer::OnFinishInflate()
{
    ActivatableNotificationView::OnFinishInflate();
    AutoPtr<IView> view;
    FindViewById(R::id::overflow_icons_view, (IView**)&view);
    mIconsView = INotificationOverflowIconsView::Probe(view);

    view = NULL;
    FindViewById(R::id::more_text, (IView**)&view);
    mIconsView->SetMoreText(ITextView::Probe(view));

    view = NULL;
    FindViewById(R::id::more_icon_overflow, (IView**)&view);
    IIconMerger::Probe(mIconsView)->SetOverflowIndicator(view);
    return NOERROR;
}

ECode CNotificationOverflowContainer::GetIconsView(
    /* [out] */ INotificationOverflowIconsView** view)
{
    VALIDATE_NOT_NULL(view);
    *view = mIconsView;
    REFCOUNT_ADD(*view);
    return NOERROR;
}

} // namespace StatusBar
} // namespace SystemUI
} // namespace Droid
} // namespace Elastos
