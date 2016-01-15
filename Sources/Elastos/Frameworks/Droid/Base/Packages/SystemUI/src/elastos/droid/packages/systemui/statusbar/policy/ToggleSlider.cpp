#include "elastos/droid/systemui/statusbar/policy/ToggleSlider.h"
#include "elastos/droid/systemui/SystemUIR.h"
#include "elastos/droid/R.h"

using Elastos::Droid::R;
using Elastos::Core::CString;
using Elastos::Droid::View::IViewGroup;
using Elastos::Droid::View::EIID_IViewGroup;
using Elastos::Droid::Widget::IRelativeLayout;
using Elastos::Droid::Widget::ICompoundButtonOnCheckedChangeListener;
using Elastos::Droid::Widget::ISeekBarOnSeekBarChangeListener;
using Elastos::Droid::Widget::EIID_ICompoundButtonOnCheckedChangeListener;
using Elastos::Droid::Widget::EIID_ISeekBarOnSeekBarChangeListener;
using Elastos::Droid::Widget::IRelativeLayoutLayoutParams;
using Elastos::Droid::Widget::ICheckable;
using Elastos::Droid::SystemUI::SystemUIR;

namespace Elastos {
namespace Droid {
namespace SystemUI {
namespace StatusBar {
namespace Policy {


ToggleSlider::ToggleSlider()
    : mTracking(FALSE)
{
}

ToggleSlider::ToggleSlider(
    /* [in] */ IContext* context)
    : RelativeLayout(context)
    , mTracking(FALSE)
{
    InitImpl(context, NULL, 0);
}

ToggleSlider::ToggleSlider(
    /* [in] */ IContext* context,
    /* [in] */ IAttributeSet* attrs)
    : RelativeLayout(context, attrs)
    , mTracking(FALSE)
{
    InitImpl(context, attrs, 0);
}

ToggleSlider::ToggleSlider(
    /* [in] */ IContext* context,
    /* [in] */ IAttributeSet* attrs,
    /* [in] */ Int32 defStyle)
    : RelativeLayout(context, attrs, defStyle)
    , mTracking(FALSE)
{
    InitImpl(context, attrs, defStyle);
}

ECode ToggleSlider::Init(
    /* [in] */ IContext* context)
{
    return Init(context, NULL);
}

ECode ToggleSlider::Init(
    /* [in] */ IContext* context,
    /* [in] */ IAttributeSet* attrs)
{
    return Init(context, NULL, 0);
}

ECode ToggleSlider::Init(
    /* [in] */ IContext* context,
    /* [in] */ IAttributeSet* attrs,
    /* [in] */ Int32 defStyle)
{
    RelativeLayout::Init(context, attrs, defStyle);
    return InitImpl(context, attrs, defStyle);
}

ECode ToggleSlider::InitImpl(
    /* [in] */ IContext* context,
    /* [in] */ IAttributeSet* attrs,
    /* [in] */ Int32 defStyle)
{
    AutoPtr<IView> tmpView;
    View::Inflate(context, SystemUIR::layout::status_bar_toggle_slider,
        THIS_PROBE(IViewGroup), (IView**)&tmpView);

    AutoPtr<IResources> res;
    context->GetResources((IResources**)&res);

    AutoPtr<ArrayOf<Int32> > attrIds = ArrayOf<Int32>::Alloc(
            const_cast<Int32 *>(SystemUIR::styleable::ToggleSlider),
            ArraySize(SystemUIR::styleable::ToggleSlider));
    AutoPtr<ITypedArray> a;
    context->ObtainStyledAttributes(attrs, attrIds, defStyle, 0, (ITypedArray**)&a);

    tmpView = FindViewById(SystemUIR::id::toggle);
    mToggle = ICompoundButton::Probe(tmpView.Get());
    mToggle->SetOnCheckedChangeListener(THIS_PROBE(ICompoundButtonOnCheckedChangeListener));
    AutoPtr<IDrawable> drawable;
    res->GetDrawable(SystemUIR::drawable::status_bar_toggle_button, (IDrawable**)&drawable);
    mToggle->SetBackgroundDrawable(drawable);

    tmpView = FindViewById(SystemUIR::id::slider);
    mSlider = ISeekBar::Probe(tmpView.Get());
    mSlider->SetOnSeekBarChangeListener(THIS_PROBE(ISeekBarOnSeekBarChangeListener));

    tmpView = FindViewById(SystemUIR::id::label);
    mLabel = ITextView::Probe(tmpView.Get());
    String text;
    a->GetString(SystemUIR::styleable::ToggleSlider_text, &text);
    AutoPtr<ICharSequence> textSeq;
    CString::New(text, (ICharSequence**)&textSeq);
    mLabel->SetText(textSeq);
    a->Recycle();
    return NOERROR;
}

ECode ToggleSlider::OnAttachedToWindow()
{
    RelativeLayout::OnAttachedToWindow();
    if (mListener != NULL) {
        mListener->OnInit(THIS_PROBE(IToggleSlider));
    }
    return NOERROR;
}

ECode ToggleSlider::OnCheckedChanged(
    /* [in] */ ICompoundButton* buttonView,
    /* [in] */ Boolean isChecked)
{
    AutoPtr<IContext> context = GetContext();
    AutoPtr<IResources> res;
    context->GetResources((IResources**)&res);
    AutoPtr<IDrawable> thumb, slider;
    if (isChecked) {
        res->GetDrawable(R::drawable::scrubber_control_disabled_holo, (IDrawable**)&thumb);
        res->GetDrawable(SystemUIR::drawable::status_bar_settings_slider_disabled, (IDrawable**)&slider);
    }
    else {
        res->GetDrawable(R::drawable::scrubber_control_selector_holo, (IDrawable**)&thumb);
        res->GetDrawable(R::drawable::scrubber_progress_horizontal_holo_dark, (IDrawable**)&slider);
    }
    mSlider->SetThumb(thumb);
    mSlider->SetProgressDrawable(slider);

    if (mListener != NULL) {
        Int32 process;
        mSlider->GetProgress(&process);
        mListener->OnChanged(THIS_PROBE(IToggleSlider), mTracking, isChecked, process);
    }
    return NOERROR;
}

ECode ToggleSlider::OnProgressChanged(
    /* [in] */ ISeekBar* seekBar,
    /* [in] */ Int32 progress,
    /* [in] */ Boolean fromUser)
{
    if (mListener != NULL) {
        mListener->OnChanged(THIS_PROBE(IToggleSlider), mTracking,
            IsChecked(), progress);
    }
    return NOERROR;
}

ECode ToggleSlider::OnStartTrackingTouch(
    /* [in] */ ISeekBar* seekBar)
{
    mTracking = TRUE;
    if (mListener != NULL) {
        Int32 process;
        mSlider->GetProgress(&process);
        mListener->OnChanged(THIS_PROBE(IToggleSlider), mTracking, IsChecked(), process);
    }
    ICheckable::Probe(mToggle.Get())->SetChecked(FALSE);
    return NOERROR;
}

ECode ToggleSlider::OnStopTrackingTouch(
    /* [in] */ ISeekBar* seekBar)
{
    mTracking = FALSE;
    if (mListener != NULL) {
        Int32 process;
        mSlider->GetProgress(&process);
        mListener->OnChanged(THIS_PROBE(IToggleSlider), mTracking, IsChecked(), process);
    }
    return NOERROR;
}

ECode ToggleSlider::SetOnChangedListener(
    /* [in] */ IToggleSliderListener* listener)
{
    mListener = listener;
    return NOERROR;
}

ECode ToggleSlider::SetChecked(
    /* [in] */ Boolean checked)
{
    ICheckable::Probe(mToggle.Get())->SetChecked(checked);
    return NOERROR;
}

Boolean ToggleSlider::IsChecked()
{
    Boolean value;
    ICheckable::Probe(mToggle.Get())->IsChecked(&value);
    return value;
}

ECode ToggleSlider::SetMax(
    /* [in] */ Int32 maxValue)
{
    mSlider->SetMax(maxValue);
    return NOERROR;
}

ECode ToggleSlider::SetValue(
    /* [in] */ Int32 value)
{
    mSlider->SetProgress(value);
    return NOERROR;
}


}// namespace Policy
}// namespace StatusBar
}// namespace SystemUI
}// namespace Droid
}// namespace Elastos
