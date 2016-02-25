#include "elastos/droid/systemui/statusbar/tablet/CompatModePanel.h"
#include "elastos/droid/systemui/SystemUIR.h"

using Elastos::Droid::View::IViewOnClickListener;
using Elastos::Droid::View::EIID_IViewOnClickListener;
using Elastos::Droid::Widget::ICheckable;
using Elastos::Droid::SystemUI::SystemUIR;

namespace Elastos {
namespace Droid {
namespace SystemUI {
namespace StatusBar {
namespace Tablet {

const String CompatModePanel::TAG("CompatModePanel");

CompatModePanel::CompatModePanel()
    : mAttached(FALSE)
{
}

CompatModePanel::CompatModePanel(
    /* [in] */ IContext* context,
    /* [in] */ IAttributeSet* attrs)
    : FrameLayout(context, attrs)
    , mAttached(FALSE)
{
    mContext = context;
    AutoPtr<IInterface> tmpObj;
    context->GetSystemService(IContext::ACTIVITY_SERVICE, (IInterface**)&tmpObj);
    mAM = IActivityManager::Probe(tmpObj.Get());
    assert(mAM != NULL);
}

ECode CompatModePanel::Init(
    /* [in] */ IContext* context,
    /* [in] */ IAttributeSet* attrs)
{
    ASSERT_SUCCEEDED(FrameLayout::Init(context, attrs));
    mContext = context;
    AutoPtr<IInterface> tmpObj;
    context->GetSystemService(IContext::ACTIVITY_SERVICE, (IInterface**)&tmpObj);
    mAM = IActivityManager::Probe(tmpObj.Get());
    assert(mAM != NULL);
    return NOERROR;
}

ECode CompatModePanel::OnFinishInflate()
{
    AutoPtr<IView> tmpView = FindViewById(SystemUIR::id::compat_mode_on_radio);
    mOnButton  = IRadioButton::Probe(tmpView.Get());

    tmpView = FindViewById(SystemUIR::id::compat_mode_off_radio);
    mOffButton  = IRadioButton::Probe(tmpView.Get());

    mOnButton->SetOnClickListener(THIS_PROBE(IViewOnClickListener));
    mOffButton->SetOnClickListener(THIS_PROBE(IViewOnClickListener));

    Refresh();
    return NOERROR;
}

ECode CompatModePanel::OnDetachedFromWindow()
{
    FrameLayout::OnDetachedFromWindow();
    if (mAttached) {
        mAttached = FALSE;
    }
    return NOERROR;
}

ECode CompatModePanel::OnAttachedToWindow()
{
    FrameLayout::OnAttachedToWindow();
    if (!mAttached) {
        mAttached = TRUE;
    }
    return NOERROR;
}

ECode CompatModePanel::OnClick(
    /* [in] */ IView* v)
{
    if (v == mOnButton) {
        mAM->SetFrontActivityScreenCompatMode(IActivityManager::COMPAT_MODE_ENABLED);
    }
    else if (v == mOffButton) {
        mAM->SetFrontActivityScreenCompatMode(IActivityManager::COMPAT_MODE_DISABLED);
    }
    return NOERROR;
}

Boolean CompatModePanel::IsInContentArea(
    /* [in] */ Int32 x,
    /* [in] */ Int32 y)
{
    return FALSE;
}

Boolean CompatModePanel::DispatchHoverEvent(
    /* [in] */ IMotionEvent* event)
{
    // Ignore hover events outside of this panel bounds since such events
    // generate spurious accessibility events with the panel content when
    // tapping outside of it, thus confusing the user.
    Float fx, fy;
    event->GetX(&fx);
    event->GetY(&fy);
    Int32 x = (Int32) fx;
    Int32 y = (Int32) fy;
    if (x >= 0 && x < GetWidth() && y >= 0 && y < GetHeight()) {
        return FrameLayout::DispatchHoverEvent(event);
    }
    return TRUE;
}

ECode CompatModePanel::SetTrigger(
    /* [in] */ IView* v)
{
    mTrigger = v;
    return NOERROR;
}

ECode CompatModePanel::OpenPanel()
{
    SetVisibility(IView::VISIBLE);
    if (mTrigger != NULL) mTrigger->SetSelected(TRUE);
    Refresh();
    return NOERROR;
}

ECode CompatModePanel::ClosePanel()
{
    SetVisibility(IView::GONE);
    if (mTrigger != NULL) mTrigger->SetSelected(FALSE);
    return NOERROR;
}

void CompatModePanel::Refresh()
{
    Int32 mode;
    mAM->GetFrontActivityScreenCompatMode(&mode);
    if (mode == IActivityManager::COMPAT_MODE_ALWAYS
        || mode == IActivityManager::COMPAT_MODE_NEVER) {
        // No longer have something to switch.
        ClosePanel();
        return;
    }
    Boolean on = (mode == IActivityManager::COMPAT_MODE_ENABLED);
    ICheckable::Probe(mOnButton.Get())->SetChecked(on);
    ICheckable::Probe(mOnButton.Get())->SetChecked(!on);
}


}// namespace Tablet
}// namespace StatusBar
}// namespace SystemUI
}// namespace Droid
}// namespace Elastos
