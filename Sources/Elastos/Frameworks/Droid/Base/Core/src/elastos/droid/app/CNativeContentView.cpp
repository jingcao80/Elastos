
#include "elastos/droid/app/CNativeContentView.h"

using Elastos::Droid::View::EIID_View;

namespace Elastos {
namespace Droid {
namespace App {

IVIEW_METHODS_IMPL(CNativeContentView, Elastos::Droid::View::View);

IDRAWABLECALLBACK_METHODS_IMPL(CNativeContentView, Elastos::Droid::View::View);

IKEYEVENTCALLBACK_METHODS_IMPL(CNativeContentView, Elastos::Droid::View::View);

IACCESSIBILITYEVENTSOURCE_METHODS_IMPL(CNativeContentView, Elastos::Droid::View::View);

ECode CNativeContentView::constructor(
    /* [in] */ IContext* ctx)
{
    return Init(ctx);
}

ECode CNativeContentView::constructor(
    /* [in] */ IContext* ctx,
    /* [in] */ IAttributeSet* attrs)
{
    return Init(ctx, attrs);
}

PInterface CNativeContentView::Probe(
    /* [in] */ REIID riid)
{
    if (riid == EIID_View) {
        return reinterpret_cast<PInterface>((View*)this);
    }
    return _CNativeContentView::Probe(riid);
}

} // namespace App
} // namespace Droid
} // namespace Elastos\
