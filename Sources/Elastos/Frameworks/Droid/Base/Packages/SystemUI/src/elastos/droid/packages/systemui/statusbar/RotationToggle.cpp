#include "elastos/droid/systemui/statusbar/RotationToggle.h"

using Elastos::Droid::Widget::EIID_ICompoundButton;
using Elastos::Droid::SystemUI::StatusBar::Policy::EIID_IRotationLockCallbacks;

namespace Elastos {
namespace Droid {
namespace SystemUI {
namespace StatusBar {


RotationToggle::RotationToggle()
{}

RotationToggle::RotationToggle(
    /* [in] */ IContext* context)
    : CompoundButton(context)
{}

RotationToggle::RotationToggle(
    /* [in] */ IContext* context,
    /* [in] */ IAttributeSet* attrs)
    : CompoundButton(context, attrs)
{}

RotationToggle::RotationToggle(
    /* [in] */ IContext* context,
    /* [in] */ IAttributeSet* attrs,
    /* [in] */ Int32 defStyle)
    : CompoundButton(context, attrs, defStyle)
{}

ECode RotationToggle::SetRotationLockControlVisibility(
    /* [in] */ Boolean show)
{
    SetVisibility(show ? IView::VISIBLE : IView::GONE);
    return NOERROR;
}

ECode RotationToggle::OnAttachedToWindow()
{
    CompoundButton::OnAttachedToWindow();

    AutoPtr<IContext> context = GetContext();
    mRotater = new AutoRotateController(
        context,
        THIS_PROBE(ICompoundButton),
        THIS_PROBE(IRotationLockCallbacks));
    return NOERROR;
}

ECode RotationToggle::OnDetachedFromWindow()
{
    CompoundButton::OnDetachedFromWindow();
    if (mRotater != NULL) {
        mRotater->UnRegister();
        mRotater = NULL;
    }
    return NOERROR;
}


}// namespace StatusBar
}// namespace SystemUI
}// namespace Droid
}// namespace Elastos
