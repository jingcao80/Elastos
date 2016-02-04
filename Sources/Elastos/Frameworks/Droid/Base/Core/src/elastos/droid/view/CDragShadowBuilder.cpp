#include "elastos/droid/ext/frameworkext.h"
#include "elastos/droid/view/CDragShadowBuilder.h"

#include <elastos/utility/logging.h>

namespace Elastos {
namespace Droid {
namespace View {

ECode CDragShadowBuilder::constructor(
    /* [in] */ IView* view)
{
    AutoPtr<IWeakReferenceSource> obj = IWeakReferenceSource::Probe(view);
    obj->GetWeakReference((IWeakReference**)&mView);
    return NOERROR;
}

ECode CDragShadowBuilder::constructor()
{
    mView = NULL;
    return NOERROR;
}

ECode CDragShadowBuilder::GetView(
    /* [out] */ IView** view)
{
    VALIDATE_NOT_NULL(view)

    if (mView == NULL) {
        view = NULL;
        return NOERROR;
    }
    return mView->Resolve(EIID_IView, (Interface**)view);

}

ECode CDragShadowBuilder::OnProvideShadowMetrics(
    /* [in] */ IPoint* shadowSize,
    /* [in] */ IPoint* shadowTouchPoint)
{
    AutoPtr<IView> view;
    mView->Resolve(EIID_IView, (Interface**)&view);
    if (view != NULL) {
        Int32 w = 0, h = 0;
        view->GetWidth(&w);
        view->GetHeight(&h);
        shadowSize->Set(w, h);
        Int32 x, y;
        shadowSize->GetX(&x);
        shadowSize->GetY(&y);
        shadowTouchPoint->Set(x / 2, y / 2);
    }
    else {
        SLOGGERE("View::DragShadowBuilder", "Asked for drag thumb metrics but no view");
    }

    return NOERROR;
}

ECode CDragShadowBuilder::OnDrawShadow(
    /* [in] */ ICanvas* canvas)
{
    AutoPtr<IView> view;
    mView->Resolve(EIID_IView, (Interface**)&view);
    if (view != NULL) {
        return view->Draw(canvas);
    }
    else {
        SLOGGERE("View::DragShadowBuilder", "Asked to draw drag shadow but no view")
    }

    return NOERROR;
}

}   //namespace View
}   //namespace Droid
}   //namespace Elastos
