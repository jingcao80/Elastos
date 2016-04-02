
#include "elastos/droid/systemui/volume/Interaction.h"

using Elastos::Droid::View::EIID_IViewOnGenericMotionListener;
using Elastos::Droid::View::EIID_IViewOnTouchListener;

namespace Elastos {
namespace Droid {
namespace SystemUI {
namespace Volume {

//-------------------------------------------------------------------------
// Interaction::MyOnTouchListener
//--------------------------------------------------------------------------

CAR_INTERFACE_IMPL(Interaction::MyOnTouchListener, Object, IViewOnTouchListener)

Interaction::MyOnTouchListener::MyOnTouchListener(
    /* [in] */ IInteractionCallback* callback)
    : mCallback(callback)
{}

ECode Interaction::MyOnTouchListener::OnTouch(
    /* [in] */ IView* v,
    /* [in] */ IMotionEvent* event,
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(result)
    mCallback->OnInteraction();
    *result = FALSE;
    return NOERROR;
}

//-------------------------------------------------------------------------
// Interaction::MyOnGenericMotionListener
//--------------------------------------------------------------------------

CAR_INTERFACE_IMPL(Interaction::MyOnGenericMotionListener, Object, IViewOnGenericMotionListener)

Interaction::MyOnGenericMotionListener::MyOnGenericMotionListener(
    /* [in] */ IInteractionCallback* callback)
    : mCallback(callback)
{}

ECode Interaction::MyOnGenericMotionListener::OnGenericMotion(
    /* [in] */ IView* v,
    /* [in] */ IMotionEvent* event,
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(result)
    mCallback->OnInteraction();
    *result = FALSE;
    return NOERROR;
}

//-------------------------------------------------------------------------
// Interaction
//--------------------------------------------------------------------------

void Interaction::Register(
    /* [in] */ IView* v,
    /* [in] */ IInteractionCallback* callback)
{
    AutoPtr<MyOnTouchListener> otl = new MyOnTouchListener(callback);
    v->SetOnTouchListener(otl);
    AutoPtr<MyOnGenericMotionListener> ogml = new MyOnGenericMotionListener(callback);
    v->SetOnTouchListener(otl);
    v->SetOnGenericMotionListener(ogml);
}

} // namespace Volume
} // namespace SystemUI
} // namespace Droid
} // namespace Elastos
