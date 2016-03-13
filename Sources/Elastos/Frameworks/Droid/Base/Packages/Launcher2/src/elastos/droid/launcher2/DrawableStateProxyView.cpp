
#include "elastos/droid/launcher2/DrawableStateProxyView.h"
#include "Elastos.Droid.Service.h"
#include "R.h"

namespace Elastos {
namespace Droid {
namespace Launcher2 {

CAR_INTERFACE_IMPL(DrawableStateProxyView, LinearLayout, IDrawableStateProxyView);

DrawableStateProxyView::DrawableStateProxyView()
    : mViewId(0)
{
}

ECode DrawableStateProxyView::constructor(
    /* [in] */ IContext* context)
{
    return constructor(context, NULL);
}

ECode DrawableStateProxyView::constructor(
    /* [in] */ IContext* context,
    /* [in] */ IAttributeSet* attrs)
{
    return constructor(context, attrs, 0);
}

ECode DrawableStateProxyView::constructor(
    /* [in] */ IContext* context,
    /* [in] */ IAttributeSet* attrs,
    /* [in] */ Int32 defStyle)
{
    LinearLayout::constructor(context, attrs, defStyle);

    AutoPtr<ITypedArray> a;
    context->ObtainStyledAttributes(attrs,
            Elastos::Droid::Launcher2::R::styleable::DrawableStateProxyView,
            defStyle, 0, (ITypedArray**)&a);
    a->GetResourceId(
            Elastos::Droid::Launcher2::R::styleable::DrawableStateProxyView_sourceViewId,
            -1, &mViewId);
    a->Recycle();

    return SetFocusable(FALSE);
}

ECode DrawableStateProxyView::DrawableStateChanged()
{
    LinearLayout::DrawableStateChanged();

    if (mView == NULL) {
        AutoPtr<IViewParent> res;
        GetParent((IViewParent**)&res);
        AutoPtr<IView> parent = IView::Probe(res);
        parent->FindViewById(mViewId, (IView**)&mView);
    }
    Boolean res;
    IsPressed(&res);
    mView->SetPressed(res);
    IsHovered(&res);
    return mView->SetHovered(res);
}

ECode DrawableStateProxyView::OnHoverEvent(
    /* [in] */ IMotionEvent* event,
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(result);

    *result = FALSE;
    return NOERROR;
}

} // namespace Launcher2
} // namespace Droid
} // namespace Elastos