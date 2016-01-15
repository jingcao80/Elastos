#include "elastos/droid/ext/frameworkext.h"
#include "elastos/droid/view/CDragShadowBuilder.h"

namespace Elastos {
namespace Droid {
namespace View {

ECode CDragShadowBuilder::constructor(
    /* [in] */ IView* view)
{
    //mView = new WeakReference<View>(view);
    mView = view;
    return NOERROR;
}

ECode CDragShadowBuilder::constructor()
{
    //mView = new WeakReference<View>(null);
    mView = NULL;
    return NOERROR;
}

ECode CDragShadowBuilder::GetView(
    /* [out] */ IView** view)
{
    VALIDATE_NOT_NULL(view);
    *view = mView;
    REFCOUNT_ADD(*view);
    return NOERROR;
}

ECode CDragShadowBuilder::OnProvideShadowMetrics(
    /* [in] */ IPoint* shadowSize,
    /* [in] */ IPoint* shadowTouchPoint)
{
    if (mView != NULL) {
        Int32 w = 0, h = 0;
        mView->GetWidth(&w);
        mView->GetHeight(&h);
        shadowSize->Set(w, h);
        Int32 x, y;
        shadowSize->GetX(&x);
        shadowSize->GetY(&y);
        shadowTouchPoint->Set(x / 2, y / 2);
    }
    else {
        //Log.e(View.VIEW_LOG_TAG, "Asked for drag thumb metrics but no view");
    }

    return NOERROR;
}

ECode CDragShadowBuilder::OnDrawShadow(
    /* [in] */ ICanvas* canvas)
{
    if (mView != NULL) {
        return mView->Draw(canvas);
    }
    else {
        //Log.e(View.VIEW_LOG_TAG, "Asked to draw drag shadow but no view");
    }

    return NOERROR;
}

}   //namespace View
}   //namespace Droid
}   //namespace Elastos
