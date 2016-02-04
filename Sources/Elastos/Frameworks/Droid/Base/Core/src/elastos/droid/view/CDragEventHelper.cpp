
#include "elastos/droid/view/CDragEventHelper.h"
#include "elastos/droid/view/CDragEvent.h"

namespace Elastos {
namespace Droid {
namespace View {

CAR_INTERFACE_IMPL(CDragEventHelper, Singleton, IDragEventHelper)
CAR_SINGLETON_IMPL(CDragEventHelper)

ECode CDragEventHelper::Obtain(
    /* [out] */ IDragEvent** event)
{
    VALIDATE_NOT_NULL(event)

    AutoPtr<IDragEvent> tmp = CDragEvent::Obtain();
    *event = tmp;
    REFCOUNT_ADD(*event)

    return NOERROR;
}

ECode CDragEventHelper::Obtain(
    /* [in] */ Int32 action,
    /* [in] */ Float x,
    /* [in] */ Float y,
    /* [in] */ IInterface* localState,
    /* [in] */ IClipDescription* description,
    /* [in] */ IClipData* data,
    /* [in] */ Boolean result,
    /* [out] */ IDragEvent** event)
{
    VALIDATE_NOT_NULL(event)

    AutoPtr<IDragEvent> tmp = CDragEvent::Obtain(
        action, x, y, localState, description,
        data, result);
    *event = tmp;
    REFCOUNT_ADD(*event)

    return NOERROR;
}

ECode CDragEventHelper::Obtain(
    /* [in] */ IDragEvent* source,
    /* [out] */ IDragEvent** event)
{
    VALIDATE_NOT_NULL(event)

    AutoPtr<IDragEvent> tmp = CDragEvent::Obtain(source);
    *event = tmp;
    REFCOUNT_ADD(*event)

    return NOERROR;
}

} //view
} //Droid
} //Elastos

