
#include "Elastos.Droid.Widget.h"
#include "elastos/droid/widget/SeekBar.h"

using Elastos::Droid::View::Accessibility::IAccessibilityRecord;
using Elastos::Core::CString;

namespace Elastos {
namespace Droid {
namespace Widget {

CAR_INTERFACE_IMPL(SeekBar, AbsSeekBar, ISeekBar);

SeekBar::SeekBar()
{
}

SeekBar::~SeekBar()
{
}

ECode SeekBar::constructor(
    /* [in] */ IContext* context)
{
    return constructor(context, NULL);
}

ECode SeekBar::constructor(
    /* [in] */ IContext* context,
    /* [in] */ IAttributeSet* attrs)
{
    return constructor(context, attrs, R::attr::seekBarStyle);
}

ECode SeekBar::constructor(
    /* [in] */ IContext* context,
    /* [in] */ IAttributeSet* attrs,
    /* [in] */ Int32 defStyleAttr)
{
    return constructor(context, attrs, defStyleAttr, 0);
}

ECode SeekBar::constructor(
    /* [in] */ IContext* context,
    /* [in] */ IAttributeSet* attrs,
    /* [in] */ Int32 defStyleAttr,
    /* [in] */ Int32 defStyleRes)
{
    return AbsSeekBar::constructor(context, attrs, defStyleAttr, defStyleRes);
}

void SeekBar::OnProgressRefresh(
    /* [in] */ Float scale,
    /* [in] */ Boolean fromUser)
{
    AbsSeekBar::OnProgressRefresh(scale, fromUser);

    Int32 progress;
    AbsSeekBar::GetProgress(&progress);

    if (mOnSeekBarChangeListener != NULL) {
        mOnSeekBarChangeListener->OnProgressChanged(
                this, progress, fromUser);
    }
}

ECode SeekBar::SetOnSeekBarChangeListener(
    /* [in] */ ISeekBarOnSeekBarChangeListener* l)
{
    mOnSeekBarChangeListener = l;

    return NOERROR;
}

void SeekBar::OnStartTrackingTouch()
{
    AbsSeekBar::OnStartTrackingTouch();

    if (mOnSeekBarChangeListener != NULL) {
        mOnSeekBarChangeListener->OnStartTrackingTouch(this);
    }
}

void SeekBar::OnStopTrackingTouch()
{
    AbsSeekBar::OnStopTrackingTouch();

    if (mOnSeekBarChangeListener != NULL) {
        mOnSeekBarChangeListener->OnStopTrackingTouch(this);
    }
}

ECode SeekBar::OnInitializeAccessibilityEvent(
    /* [in] */ IAccessibilityEvent* event)
{
    AbsSeekBar::OnInitializeAccessibilityEvent(event);
    AutoPtr<ICharSequence> clsName;
    CString::New(String("CSeekBar"), (ICharSequence**)&clsName);
    IAccessibilityRecord::Probe(event)->SetClassName(clsName);
    return NOERROR;
}

ECode SeekBar::OnInitializeAccessibilityNodeInfo(
    /* [in] */ IAccessibilityNodeInfo* info)
{
    AbsSeekBar::OnInitializeAccessibilityNodeInfo(info);
    AutoPtr<ICharSequence> clsName;
    CString::New(String("CSeekBar"), (ICharSequence**)&clsName);
    info->SetClassName(clsName);
    return NOERROR;
}

}// namespace Widget
}// namespace Droid
}// namespace Elastos


