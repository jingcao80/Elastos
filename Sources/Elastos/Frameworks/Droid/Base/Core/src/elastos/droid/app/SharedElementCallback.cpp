
#include "Elastos.Droid.Content.h"
#include "Elastos.Droid.View.h"
#include "elastos/droid/app/SharedElementCallback.h"
#include "elastos/droid/graphics/CMatrix.h"
#include "elastos/droid/graphics/drawable/CBitmapDrawable.h"
// #include "elastos/droid/view/CView.h"
#include "elastos/droid/transition/CTransitionUtils.h"

using Elastos::Droid::Content::Res::IResources;
using Elastos::Droid::Graphics::IBitmap;
using Elastos::Droid::Graphics::ICanvas;
using Elastos::Droid::Graphics::CMatrix;
using Elastos::Droid::Graphics::Drawable::IDrawable;
using Elastos::Droid::Graphics::Drawable::IBitmapDrawable;
using Elastos::Droid::Graphics::Drawable::CBitmapDrawable;
// using Elastos::Droid::View::CView;
using Elastos::Droid::Transition::ITransitionUtils;
using Elastos::Droid::Transition::CTransitionUtils;

namespace Elastos {
namespace Droid {
namespace App {

const AutoPtr<ISharedElementCallback> SharedElementCallback::NULL_CALLBACK = new SharedElementCallback();

CAR_INTERFACE_IMPL(SharedElementCallback, Object, ISharedElementCallback)

SharedElementCallback::SharedElementCallback()
{}

SharedElementCallback::~SharedElementCallback()
{}

ECode SharedElementCallback::OnSharedElementStart(
    /* [in] */ IList* sharedElementNames, //List<String>
    /* [in] */ IList* sharedElements, //List<View>
    /* [in] */ IList* sharedElementSnapshots) //List<View>
{
    return NOERROR;
}

ECode SharedElementCallback::OnSharedElementEnd(
    /* [in] */ IList* sharedElementNames, //List<String>
    /* [in] */ IList* sharedElements, //List<View>
    /* [in] */ IList* sharedElementSnapshots) //List<View>
{
    return NOERROR;
}

ECode SharedElementCallback::OnRejectSharedElements(
    /* [in] */ IList* rejectedSharedElements) // List<View>
{
    return NOERROR;
}

ECode SharedElementCallback::OnMapSharedElements(
    /* [in] */ IList* names, //List<String>
    /* [in] */ IMap* sharedElements) //Map<String, View>
{
    return NOERROR;
}

ECode SharedElementCallback::OnCaptureSharedElementSnapshot(
    /* [in] */ IView* sharedElement,
    /* [in] */ IMatrix* viewToGlobalMatrix,
    /* [in] */ IRectF* screenBounds,
    /* [out] */ IParcelable** parcleable)
{
    VALIDATE_NOT_NULL(parcleable)

    if (mTempMatrix == NULL) {
        CMatrix::New(viewToGlobalMatrix, (IMatrix**)&mTempMatrix);
    }
    else {
        mTempMatrix->Set(viewToGlobalMatrix);
    }

    AutoPtr<ITransitionUtils> utils;
    CTransitionUtils::AcquireSingleton((ITransitionUtils**)&utils);
    AutoPtr<IBitmap> bmp;
    utils->CreateViewBitmap(sharedElement, mTempMatrix, screenBounds, (IBitmap**)&bmp);
    *parcleable = IParcelable::Probe(bmp);
    REFCOUNT_ADD(*parcleable)
    return NOERROR;
}

ECode SharedElementCallback::OnCreateSnapshotView(
    /* [in] */ IContext* context,
    /* [in] */ IParcelable* snapshot,
    /* [out] */ IView** result)
{
    VALIDATE_NOT_NULL(result)
    *result = NULL;

    AutoPtr<IView> view;
    IBitmap* bitmap = IBitmap::Probe(snapshot);
    if (bitmap != NULL) {
        assert(0 && "TODO");
        // CView::New(context, (IView**)&view);
        AutoPtr<IResources> resources;
        context->GetResources((IResources**)&resources);
        AutoPtr<IDrawable> bd;
        CBitmapDrawable::New(resources, bitmap, (IDrawable**)&bd);
        view->SetBackground(bd);
    }
    *result = view;
    REFCOUNT_ADD(*result)
    return NOERROR;
}

} // namespace App
} // namespace Droid
} // namespace Elastos