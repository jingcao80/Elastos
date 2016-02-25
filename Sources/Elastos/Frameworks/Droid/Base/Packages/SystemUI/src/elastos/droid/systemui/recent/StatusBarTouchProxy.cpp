#include "elastos/droid/systemui/recent/StatusBarTouchProxy.h"

namespace Elastos {
namespace Droid {
namespace SystemUI {
namespace Recent {

StatusBarTouchProxy::StatusBarTouchProxy()
{}

ECode StatusBarTouchProxy::Init(
    /* [in] */ IContext* context,
    /* [in] */ IAttributeSet* attrs)
{
    return FrameLayout::Init(context, attrs);
}

ECode StatusBarTouchProxy::SetStatusBar(
    /* [in] */ IView* statusBar)
{
    mStatusBar = statusBar;
    return NOERROR;
}

Boolean StatusBarTouchProxy::OnTouchEvent (
    /* [in] */ IMotionEvent* event)
{
    Boolean result;
    mStatusBar->DispatchTouchEvent(event, &result);
    return result;
}

}// namespace Recent
}// namespace SystemUI
}// namespace Droid
}// namespace Elastos
