
#include "elastos/droid/systemui/statusbar/phone/CPanelHolder.h"
#include <elastos/utility/logging/Logger.h>

using Elastos::Utility::Logging::Logger;

namespace Elastos {
namespace Droid {
namespace SystemUI {
namespace StatusBar {
namespace Phone {

const Boolean CPanelHolder::DEBUG_GESTURES = TRUE;

CAR_OBJECT_IMPL(CPanelHolder);
CAR_INTERFACE_IMPL(CPanelHolder, FrameLayout, IPanelHolder);
CPanelHolder::CPanelHolder()
    : mSelectedPanelIndex(-1)
{}

ECode CPanelHolder::constructor(
    /* [in] */ IContext* context,
    /* [in] */ IAttributeSet* attrs)
{
    FrameLayout::constructor(context, attrs);
    SetChildrenDrawingOrderEnabled(TRUE);
    return NOERROR;
}

ECode CPanelHolder::OnFinishInflate()
{
    FrameLayout::OnFinishInflate();
    SetChildrenDrawingOrderEnabled(TRUE);
    return NOERROR;
}

ECode CPanelHolder::GetPanelIndex(
    /* [in] */ IPanelView* pv,
    /* [out] */ Int32* result)
{
    VALIDATE_NOT_NULL(result);
    Int32 N = 0;
    GetChildCount(&N);
    for (Int32 i = 0; i<N; i++) {
        AutoPtr<IView> obj;
        GetChildAt(i, (IView**)&obj);
        AutoPtr<IPanelView> v = IPanelView::Probe(obj);
        if (pv == v.Get()) {
            *result = i;
            return NOERROR;
        }
    }
    *result = -1;
    return NOERROR;
}

ECode CPanelHolder::SetSelectedPanel(
    /* [in] */ IPanelView* pv)
{
    GetPanelIndex(pv, &mSelectedPanelIndex);
    return NOERROR;
}

Int32 CPanelHolder::GetChildDrawingOrder(
    /* [in] */ Int32 childCount,
    /* [in] */ Int32 i)
{
    if (mSelectedPanelIndex == -1) {
        return i;
    } else {
        if (i == childCount - 1) {
            return mSelectedPanelIndex;
        }
        else if (i >= mSelectedPanelIndex) {
            return i + 1;
        }
        else {
            return i;
        }
    }
}

ECode CPanelHolder::OnTouchEvent(
    /* [in] */ IMotionEvent* event,
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(result);
    if (DEBUG_GESTURES) {
        Int32 masked = 0;
        event->GetActionMasked(&masked);
        if (masked != IMotionEvent::ACTION_MOVE) {
            Float x = 0, y = 0;
            event->GetX(&x);
            event->GetY(&y);
            Logger::D("CPanelHolder", "OnTouchEvent masked=[%d], x=[%d], y=[%d]"
                    , masked, (Int32)x, (Int32)y);
            // EventLog.writeEvent(EventLogTags.SYSUI_PANELHOLDER_TOUCH,
            //         masked, (Int32) event.getX(), (Int32) event.getY());
        }
    }
    *result = FALSE;
    return NOERROR;
}

ECode CPanelHolder::SetBar(
    /* [in] */ IPanelBar* panelBar)
{
    mBar = panelBar;
    return NOERROR;
}

}// namespace Phone
}// namespace StatusBar
}// namespace SystemUI
}// namespace Droid
}// namespace Elastos
