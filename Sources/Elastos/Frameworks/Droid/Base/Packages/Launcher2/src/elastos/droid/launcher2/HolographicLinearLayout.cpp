
#include "elastos/droid/launcher2/HolographicLinearLayout.h"
#include "Elastos.Droid.Service.h"
#include "R.h"

namespace Elastos {
namespace Droid {
namespace Launcher2 {

CAR_INTERFACE_IMPL(HolographicLinearLayout, LinearLayout, IHolographicLinearLayout);

HolographicLinearLayout::HolographicLinearLayout()
    : mImageViewId(0)
{
}

ECode HolographicLinearLayout::constructor(
    /* [in] */ IContext* context)
{
    return constructor(context, NULL);
}

ECode HolographicLinearLayout::constructor(
    /* [in] */ IContext* context,
    /* [in] */ IAttributeSet* attrs)
{
    return constructor(context, attrs, 0);
}

ECode HolographicLinearLayout::constructor(
    /* [in] */ IContext* context,
    /* [in] */ IAttributeSet* attrs,
    /* [in] */ Int32 defStyle)
{
    LinearLayout::constructor(context, attrs, defStyle);

    AutoPtr<ITypedArray> a;
    context->ObtainStyledAttributes(attrs,
            Elastos::Droid::Launcher2::R::styleable::HolographicLinearLayout,
            defStyle, 0, (ITypedArray**)&a);
    a->GetResourceId(R.styleable.HolographicLinearLayout_sourceImageViewId, -1, &mImageViewId);
    a->Recycle();

    SetWillNotDraw(FALSE);
    mHolographicHelper = new HolographicViewHelper(context);
    return NOERROR;
}

ECode HolographicLinearLayout::DrawableStateChanged()
{
    LinearLayout::DrawableStateChanged();

    if (mImageView != NULL) {
        AutoPtr<IDrawable> d;
        mImageView->GetDrawable((IDrawable**)&d);
        if (IStateListDrawable::Probe(d) != NULL) {
            AutoPtr<IStateListDrawable> sld = IStateListDrawable::Probe(d);
            AutoPtr<ArrayOf<Int32> > stateSet;
            GetDrawableState((ArrayOf<Int32>**)&stateSet);
            Boolean isStateful;
            return sld->SetState(stateSet, &isStateful);
        }
    }
    return NOERROR;
}

ECode HolographicLinearLayout::InvalidatePressedFocusedStates()
{
    mHolographicHelper->InvalidatePressedFocusedStates(mImageView);
    return Invalidate();
}

ECode HolographicLinearLayout::OnDraw(
    /* [in] */ ICanvas* canvas)
{
    LinearLayout::OnDraw(canvas);

    // One time call to generate the pressed/focused state -- must be called after
    // measure/layout
    if (mImageView == NULL) {
        AutoPtr<IView> view;
        FindViewById(mImageViewId, (IView**)&view);
        mImageView = IImageView::Probe(view);
    }
    return mHolographicHelper->GeneratePressedFocusedStates(mImageView);
}

} // namespace Launcher2
} // namespace Droid
} // namespace Elastos