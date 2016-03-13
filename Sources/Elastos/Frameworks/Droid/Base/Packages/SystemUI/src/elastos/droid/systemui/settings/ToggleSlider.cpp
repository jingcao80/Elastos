
#include "elastos/droid/systemui/settings/ToggleSlider.h"
#include "Elastos.Droid.View.h"
#include "Elastos.CoreLibrary.Core.h"
#include "elastos/droid/view/View.h"
#include "../R.h"

using Elastos::Droid::Content::Res::IResources;
using Elastos::Droid::Content::Res::ITypedArray;
using Elastos::Droid::View::IView;
using Elastos::Droid::View::View;
using Elastos::Droid::View::IViewParent;
using Elastos::Droid::Widget::EIID_ICompoundButtonOnCheckedChangeListener;
using Elastos::Droid::Widget::EIID_ISeekBarOnSeekBarChangeListener;
using Elastos::Droid::Widget::ICheckable;
using Elastos::Droid::Widget::IProgressBar;
using Elastos::Core::CString;
using Elastos::Core::ICharSequence;


namespace Elastos {
namespace Droid {
namespace SystemUI {
namespace Settings {

//==================================
//   ToggleSlider::MyCheckListener
//==================================

CAR_INTERFACE_IMPL(ToggleSlider::MyCheckListener, Object, ICompoundButtonOnCheckedChangeListener)

ToggleSlider::MyCheckListener::MyCheckListener(
    /* [in] */ ToggleSlider* host)
    : mHost(host)
{}

ECode ToggleSlider::MyCheckListener::OnCheckedChanged(
    /* [in] */ ICompoundButton* buttonView,
    /* [in] */ Boolean isChecked)
{
    IView::Probe(mHost->mSlider)->SetEnabled(!isChecked);

    if (mHost->mListener != NULL) {
        Int32 progress;
        IProgressBar::Probe(mHost->mSlider)->GetProgress(&progress);
        mHost->mListener->OnChanged(mHost, mHost->mTracking, isChecked, progress);
    }

    if (mHost->mMirror != NULL) {
        ICheckable::Probe(mHost->mMirror->mToggle)->SetChecked(isChecked);
    }
    return NOERROR;
}

//==================================
//   ToggleSlider::MySeekListener
//==================================

CAR_INTERFACE_IMPL(ToggleSlider::MySeekListener, Object, ISeekBarOnSeekBarChangeListener)

ToggleSlider::MySeekListener::MySeekListener(
    /* [in] */ ToggleSlider* host)
    : mHost(host)
{}

ECode ToggleSlider::MySeekListener::OnProgressChanged(
    /* [in] */ ISeekBar* seekBar,
    /* [in] */ Int32 progress,
    /* [in] */ Boolean fromUser)
{
    if (mHost->mListener != NULL) {
        Boolean isChecked;
        ICheckable::Probe(mHost->mToggle)->IsChecked(&isChecked);
        mHost->mListener->OnChanged(
                mHost, mHost->mTracking, isChecked, progress);
    }

    if (mHost->mMirror != NULL) {
        mHost->mMirror->SetValue(progress);
    }
    return NOERROR;
}

ECode ToggleSlider::MySeekListener::OnStartTrackingTouch(
    /* [in] */ ISeekBar* seekBar)
{
    return NOERROR;
}

ECode ToggleSlider::MySeekListener::OnStopTrackingTouch(
     /* [in] */ ISeekBar* seekBar)
{
    return NOERROR;
}

//==================================
//   ToggleSlider
//==================================
CAR_INTERFACE_IMPL(ToggleSlider, RelativeLayout, IToggleSlider)

ToggleSlider::ToggleSlider()
{
    mCheckListener = new MyCheckListener(this);
    mSeekListener = new MySeekListener(this);
}

ToggleSlider::ToggleSlider(
    /* [in] */ IContext* context)
{
    ToggleSlider(context, NULL);
}

ToggleSlider::ToggleSlider(
    /* [in] */ IContext* context,
    /* [in] */ IAttributeSet* attrs)
{
    ToggleSlider(context, attrs, 0);
}

ToggleSlider::ToggleSlider(
    /* [in] */ IContext* context,
    /* [in] */ IAttributeSet* attrs,
    /* [in] */ Int32 defStyle)
{
    RelativeLayout::constructor(context, attrs, defStyle);

    AutoPtr<IView> view;
    View::Inflate(context, R::layout::status_bar_toggle_slider, this, (IView**)&view);

    AutoPtr<IResources> res;
    context->GetResources((IResources**)&res);
#if 0   //R::styleable::ToggleSlider
    AutoPtr<ITypedArray> a;
    context->ObtainStyledAttributes(
        attrs, R::styleable::ToggleSlider, defStyle, 0, (ITypedArray**)&a);


    AutoPtr<IView> v;
    FindViewById(R::id::toggle, (IView**)&v);
    mToggle = ICompoundButton::Probe(v);
    mToggle->SetOnCheckedChangeListener(mCheckListener);

    v = NULL;
    FindViewById(R::id::slider, (IView**)&v);
    mSlider = ISeekBar::Probe(v);
    mSlider->SetOnSeekBarChangeListener(mSeekListener);

    v = NULL;
    FindViewById(R::id::label, (IView**)&v);
    mLabel = ITextView::Probe(v);
    String str;
    a->GetString(R::styleable::ToggleSlider_text, &str);
    AutoPtr<ICharSequence> cs;
    CString::New(str, (ICharSequence**)&cs);
    mLabel->SetText(cs);

    a->Recycle();
#endif
}

ECode ToggleSlider::SetMirror(
    /* [in] */ IToggleSlider* toggleSlider)
{
    mMirror = (ToggleSlider*)toggleSlider;
    if (mMirror != NULL) {
        Boolean isChecked;
        ICheckable::Probe(mToggle)->IsChecked(&isChecked);
        mMirror->SetChecked(isChecked);
        Int32 max;
        IProgressBar::Probe(mSlider)->GetMax(&max);
        mMirror->SetMax(max);
        Int32 progress;
        IProgressBar::Probe(mSlider)->GetProgress(&progress);
        mMirror->SetValue(progress);
    }
    return NOERROR;
}

ECode ToggleSlider::SetMirrorController(
    /* [in] */ IBrightnessMirrorController* c)
{
    mMirrorController = c;
    return NOERROR;
}

ECode ToggleSlider::OnAttachedToWindow()
{
    ViewGroup::OnAttachedToWindow();
    if (mListener != NULL) {
        mListener->OnInit(this);
    }
    return NOERROR;
}

ECode ToggleSlider::SetOnChangedListener(
    /* [in] */ IToggleSliderListener* l)
{
    mListener = l;
    return NOERROR;
}

ECode ToggleSlider::SetChecked(
    /* [in] */ Boolean checked)
{
    return ICheckable::Probe(mToggle)->SetChecked(checked);
}

ECode ToggleSlider::IsChecked(
    /* [out] */ Boolean* isChecked)
{
    ICheckable::Probe(mToggle)->IsChecked(isChecked);
    return NOERROR;
}

ECode ToggleSlider::SetMax(
    /* [in] */ Int32 max)
{
    IProgressBar::Probe(mSlider)->SetMax(max);
    if (mMirror != NULL) {
        mMirror->SetMax(max);
    }
    return NOERROR;
}

ECode ToggleSlider::SetValue(
    /* [in] */ Int32 value)
{
    IProgressBar::Probe(mSlider)->SetProgress(value);
    if (mMirror != NULL) {
        mMirror->SetValue(value);
    }
    return NOERROR;
}

ECode ToggleSlider::OnStartTrackingTouch(
    /* [in] */ ISeekBar* seekBar)
{
    mTracking = TRUE;

    if (mListener != NULL) {
        Boolean isChecked;
        ICheckable::Probe(mToggle)->IsChecked(&isChecked);
        Int32 progress;
        IProgressBar::Probe(mSlider)->GetProgress(&progress);
        mListener->OnChanged(this, mTracking, isChecked, progress);
    }

    ICheckable::Probe(mToggle)->SetChecked(FALSE);

    if (mMirror != NULL) {
        IView::Probe(mMirror->mSlider)->SetPressed(TRUE);
    }

    if (mMirrorController != NULL) {
        mMirrorController->ShowMirror();
        AutoPtr<IViewParent> parent;
        GetParent((IViewParent**)&parent);
        mMirrorController->SetLocation(IView::Probe(parent));
    }
    return NOERROR;
}

ECode ToggleSlider::OnStopTrackingTouch(
    /* [in] */ ISeekBar* seekBar)
{
    mTracking = FALSE;

    if (mListener != NULL) {
        Boolean isChecked;
        ICheckable::Probe(mToggle)->IsChecked(&isChecked);
        Int32 progress;
        IProgressBar::Probe(mSlider)->GetProgress(&progress);
        mListener->OnChanged(this, mTracking, isChecked, progress);
    }

    if (mMirror != NULL) {
        IView::Probe(mMirror->mSlider)->SetPressed(FALSE);
    }

    if (mMirrorController != NULL) {
        mMirrorController->HideMirror();
    }
    return NOERROR;
}

} // namespace Settings
} // namespace SystemUI
} // namespace Droid
} // namespace Elastos
