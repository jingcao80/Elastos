#include "elastos/droid/systemui/statusbar/phone/PanelHolder.h"

namespace Elastos {
namespace Droid {
namespace SystemUI {
namespace StatusBar {
namespace Phone {


PanelHolder::PanelHolder()
    : mSelectedPanelIndex(-1)
{
}

PanelHolder::PanelHolder(
    /* [in] */ IContext* context,
    /* [in] */ IAttributeSet* attrs)
    : FrameLayout(context, attrs)
    , mSelectedPanelIndex(-1)
{
    SetChildrenDrawingOrderEnabled(TRUE);
}

ECode PanelHolder::Init(
    /* [in] */ IContext* context,
    /* [in] */ IAttributeSet* attrs)
{
    ASSERT_SUCCEEDED(FrameLayout::Init(context, attrs));
    SetChildrenDrawingOrderEnabled(TRUE);
    return NOERROR;
}

Int32 PanelHolder::GetPanelIndex(
    /* [in] */ IPanelView* panelView)
{
    Int32 N = GetChildCount();
    for (Int32 i = 0; i < N; i++) {
        AutoPtr<IView> cv = GetChildAt(i);
        IPanelView* pv = IPanelView::Probe(cv.Get());
        if (pv == panelView) return i;
    }
    return -1;
}

ECode PanelHolder::SetSelectedPanel(
    /* [in] */ IPanelView* panelView)
{
    mSelectedPanelIndex = GetPanelIndex(panelView);
    return NOERROR;
}

Int32 PanelHolder::GetChildDrawingOrder(
    /* [in] */ Int32 childCount,
    /* [in] */ Int32 i)
{
    if (mSelectedPanelIndex == -1) {
        return i;
    } else {
        if (i == childCount - 1) {
            return mSelectedPanelIndex;
        } else if (i >= mSelectedPanelIndex) {
            return i + 1;
        } else {
            return i;
        }
    }
}

Boolean PanelHolder::OnTouchEvent(
    /* [in] */ IMotionEvent* event)
{
    Int32 action;
    event->GetAction(&action);
    switch (action) {
        case IMotionEvent::ACTION_DOWN:
            // PanelBar.LOG("PanelHolder got touch in open air, closing panels");
            mBar->CollapseAllPanels(TRUE);
            break;
    }
    return FALSE;
}

ECode PanelHolder::SetBar(
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
