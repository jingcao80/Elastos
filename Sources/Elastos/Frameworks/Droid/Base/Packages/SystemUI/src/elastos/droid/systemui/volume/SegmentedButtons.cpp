#include "elastos/droid/systemui/volume/SegmentedButtons.h"
#include "Elastos.CoreLibrary.Core.h"
#include "elastos/droid/systemui/volume/Interaction.h"
#include "elastos/droid/view/LayoutInflater.h"
#include "../R.h"
#include <elastos/core/StringUtils.h>
#include <elastos/utility/Objects.h>

using Elastos::Droid::Graphics::CTypefaceHelper;
using Elastos::Droid::Graphics::ITypefaceHelper;
using Elastos::Droid::View::EIID_IViewOnClickListener;
using Elastos::Droid::View::IViewGroupMarginLayoutParams;
using Elastos::Droid::View::LayoutInflater;
using Elastos::Droid::Widget::IButton;
using Elastos::Droid::Widget::ILinearLayout;
using Elastos::Droid::Widget::ITextView;
using Elastos::Core::CInteger32;
using Elastos::Core::CString;
using Elastos::Core::ICharSequence;
using Elastos::Core::IInteger32;
using Elastos::Core::StringUtils;
using Elastos::Utility::Objects;

namespace Elastos {
namespace Droid {
namespace SystemUI {
namespace Volume {

//==========================================
// SegmentedButtons::MyClick
//==========================================

CAR_INTERFACE_IMPL(SegmentedButtons::MyClick, Object, IViewOnClickListener)

SegmentedButtons::MyClick::MyClick(
    /* [in] */ SegmentedButtons* host)
    : mHost(host)
{}

ECode SegmentedButtons::MyClick::OnClick(
    /* [in] */ IView* v)
{
    AutoPtr<IInterface> obj;
    v->GetTag((IInterface**)&obj);
    mHost->SetSelectedValue(obj);
    return NOERROR;
}

//==========================================
// SegmentedButtons::MyInteractionCallback
//==========================================

CAR_INTERFACE_IMPL(SegmentedButtons::MyInteractionCallback, Object, IInteractionCallback)

SegmentedButtons::MyInteractionCallback::MyInteractionCallback(
    /* [in] */ SegmentedButtons* host)
    : mHost(host)
{}

ECode SegmentedButtons::MyInteractionCallback::OnInteraction()
{
    mHost->FireInteraction();
    return NOERROR;
}

//==========================================
// SegmentedButtons
//==========================================

static AutoPtr<ITypeface> initMEDIUM()
{
    AutoPtr<ITypefaceHelper> th;
    CTypefaceHelper::AcquireSingleton((ITypefaceHelper**)&th);
    AutoPtr<ITypeface> tf;
    th->Create(String("sans-serif-medium"), ITypeface::NORMAL, (ITypeface**)&tf);
    return tf;
}

static AutoPtr<ITypeface> initBOLD()
{
    AutoPtr<ITypefaceHelper> th;
    CTypefaceHelper::AcquireSingleton((ITypefaceHelper**)&th);
    AutoPtr<ITypeface> tf;
    th->Create(String("sans-serif"), ITypeface::BOLD, (ITypeface**)&tf);
    return tf;
}

const AutoPtr<ITypeface> SegmentedButtons::MEDIUM = initMEDIUM();
const AutoPtr<ITypeface> SegmentedButtons::BOLD = initBOLD();
const Int32 SegmentedButtons::LABEL_RES_KEY = R::id::label;

CAR_INTERFACE_IMPL(SegmentedButtons, LinearLayout, ISegmentedButtons)

SegmentedButtons::SegmentedButtons()
{
    mClick = new MyClick(this);
}

ECode SegmentedButtons::constructor(
    /* [in] */ IContext* context,
    /* [in] */ IAttributeSet* attrs)
{
    LinearLayout::constructor(context, attrs);
    mContext = context;
    LayoutInflater::From(mContext, (ILayoutInflater**)&mInflater);
    SetOrientation(ILinearLayout::HORIZONTAL);
    return NOERROR;
}

ECode SegmentedButtons::SetCallback(
    /* [in] */ ISegmentedButtonsCallback* callback)
{
    mCallback = callback;
    return NOERROR;
}

ECode SegmentedButtons::GetSelectedValue(
    /* [out]] */ IInterface** sv)
{
    VALIDATE_NOT_NULL(sv)
    *sv = mSelectedValue;
    REFCOUNT_ADD(*sv)
    return NOERROR;
}

ECode SegmentedButtons::SetSelectedValue(
    /* [in] */ IInterface* value)
{
    if (Objects::Equals(value, (IInterface*)mSelectedValue)) return E_NULL_POINTER_EXCEPTION;
    mSelectedValue = value;
    Int32 count;
    GetChildCount(&count);
    for (Int32 i = 0; i < count; i++) {
        AutoPtr<IView> v;
        GetChildAt(i, (IView**)&v);
        const AutoPtr<ITextView> c = ITextView::Probe(v);
        AutoPtr<IInterface> tag;
        IView::Probe(c)->GetTag((IInterface**)&tag);
        const Boolean selected = Objects::Equals(mSelectedValue, tag);
        IView::Probe(c)->SetSelected(selected);
        c->SetTypeface(selected ? BOLD : MEDIUM);
    }
    FireOnSelected();
    return NOERROR;
}

ECode SegmentedButtons::AddButton(
    /* [in] */ Int32 labelResId,
    /* [in] */ IInterface* value)
{
    AutoPtr<IView> view;
    mInflater->Inflate(R::layout::segmented_button, this, FALSE, (IView**)&view);
    const AutoPtr<IButton> b = IButton::Probe(view);

    AutoPtr<IInteger32> i;
    CInteger32::New(labelResId, (IInteger32**)&i);
    IView::Probe(b)->SetTag(R::id::label/*LABEL_RES_KEY*/, i);

    AutoPtr<ICharSequence> cs;
    CString::New(StringUtils::ToString(labelResId), (ICharSequence**)&cs);
    ITextView::Probe(b)->SetText(cs);

    AutoPtr<IViewGroupLayoutParams> lp;
    IView::Probe(b)->GetLayoutParams((IViewGroupLayoutParams**)&lp);
    Int32 count;
    GetChildCount(&count);
    if (count == 0) {
        IViewGroupMarginLayoutParams::Probe(lp)->SetRightMargin(0);
        IViewGroupMarginLayoutParams::Probe(lp)->SetLeftMargin(0); // first button has no margin
    }
    IView::Probe(b)->SetLayoutParams(lp);
    AddView(IView::Probe(b));
    IView::Probe(b)->SetTag(value);
    IView::Probe(b)->SetOnClickListener(mClick);
    AutoPtr<MyInteractionCallback> icb = new MyInteractionCallback(this);
    Interaction::Register(IView::Probe(b), icb);
    return NOERROR;
}

ECode SegmentedButtons::UpdateLocale()
{
    Int32 count;
    GetChildCount(&count);
    for (Int32 i = 0; i < count; i++) {
        AutoPtr<IView> c;
        GetChildAt(i, (IView**)&c);
        const AutoPtr<IButton> b = IButton::Probe(c);
        AutoPtr<IInterface> tag;
        IView::Probe(b)->GetTag(R::id::label/*LABEL_RES_KEY*/, (IInterface**)&tag);
        AutoPtr<IInteger32> i32 = IInteger32::Probe(tag);
        Int32 value;
        i32->GetValue(&value);
        const Int32 labelResId = value;
        AutoPtr<ICharSequence> cs;
        CString::New(StringUtils::ToString(labelResId), (ICharSequence**)&cs);
        ITextView::Probe(b)->SetText(cs);
    }
    return NOERROR;
}

void SegmentedButtons::FireOnSelected()
{
    if (mCallback != NULL) {
        mCallback->OnSelected(mSelectedValue);
    }
}

void SegmentedButtons::FireInteraction()
{
    if (mCallback != NULL) {
        IInteractionCallback::Probe(mCallback)->OnInteraction();
    }
}

} // namespace Volume
} // namespace SystemUI
} // namespace Droid
} // namespace Elastos