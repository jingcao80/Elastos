
#include "elastos/droid/systemui/statusbar/CDismissView.h"
#include "../R.h"

namespace Elastos {
namespace Droid {
namespace SystemUI {
namespace StatusBar {

CAR_OBJECT_IMPL(CDismissView)
CAR_INTERFACE_IMPL(CDismissView, StackScrollerDecorView, IDismissView)
ECode CDismissView::constructor(
    /* [in] */ IContext* context,
    /* [in] */ IAttributeSet* attrs)
{
    return StackScrollerDecorView::constructor(context, attrs);
}

AutoPtr<IView> CDismissView::FindContentView()
{
    AutoPtr<IView> view;
    FindViewById(R::id::dismiss_text, (IView**)&view);
    return view;
}

ECode CDismissView::SetOnButtonClickListener(
    /* [in] */ IViewOnClickListener* listener)
{
    mContent->SetOnClickListener(listener);
    return NOERROR;
}

} // namespace StatusBar
} // namespace SystemUI
} // namespace Droid
} // namespace Elastos
