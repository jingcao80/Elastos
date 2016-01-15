#include "elastos/droid/ext/frameworkext.h"
#include "elastos/droid/view/CDragEventHelper.h"
#include "elastos/droid/view/CDragEvent.h"

namespace Elastos {
namespace Droid {
namespace View {

ECode CDragEventHelper::Obtain(
    /* [out] */ IDragEvent** event)
{
    CDragEvent* ev;
    FAIL_RETURN(CDragEvent::Obtain(&ev));
    *event = (IDragEvent*)ev;
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
    AutoPtr<IDragEvent> ev;
    FAIL_RETURN(CDragEvent::Obtain(
        action, x, y, localState, description,
        data, result, (CDragEvent**)&ev));

    *event = ev;
    REFCOUNT_ADD(*event);
    return NOERROR;
}

ECode CDragEventHelper::Obtain(
    /* [in] */ IDragEvent* source,
    /* [out] */ IDragEvent** event)
{
    AutoPtr<IDragEvent> ev;
    FAIL_RETURN(CDragEvent::Obtain((CDragEvent*)source, (CDragEvent**)&ev));

    *event = ev;
    REFCOUNT_ADD(*event);
    return NOERROR;
}

} //view
} //Droid
} //Elastos

