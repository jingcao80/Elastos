
#include "elastos/droid/systemui/statusbar/CEmptyShadeView.h"
#include "R.h"

using Elastos::Droid::Widget::ITextView;

namespace Elastos {
namespace Droid {
namespace SystemUI {
namespace StatusBar {

CAR_OBJECT_IMPL(CEmptyShadeView)
CAR_INTERFACE_IMPL(CEmptyShadeView, StackScrollerDecorView, IEmptyShadeView)
ECode CEmptyShadeView::constructor(
    /* [in] */ IContext* context,
    /* [in] */ IAttributeSet* attrs)
{
    return StackScrollerDecorView::constructor(context, attrs);
}

void CEmptyShadeView::OnConfigurationChanged(
    /* [in] */ IConfiguration* newConfig)
{
    StackScrollerDecorView::OnConfigurationChanged(newConfig);
    AutoPtr<IView> view;
    FindViewById(R::id::no_notifications, (IView**)&view);
    ITextView::Probe(view)->SetText(R::string::empty_shade_text);
}

AutoPtr<IView> CEmptyShadeView::FindContentView()
{
    AutoPtr<IView> view;
    FindViewById(R::id::no_notifications, (IView**)&view);
    return view;
}

} // namespace StatusBar
} // namespace SystemUI
} // namespace Droid
} // namespace Elastos
