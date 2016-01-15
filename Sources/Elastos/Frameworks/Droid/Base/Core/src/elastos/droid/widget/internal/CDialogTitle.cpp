#include "elastos/droid/widget/internal/CDialogTitle.h"

using Elastos::Droid::View::EIID_View;
using Elastos::Droid::Widget::EIID_TextView;

namespace Elastos {
namespace Droid {
namespace Widget {
namespace Internal {

IVIEW_METHODS_IMPL(CDialogTitle, DialogTitle);
IDRAWABLECALLBACK_METHODS_IMPL(CDialogTitle, DialogTitle);
IKEYEVENTCALLBACK_METHODS_IMPL(CDialogTitle, DialogTitle);
IACCESSIBILITYEVENTSOURCE_METHODS_IMPL(CDialogTitle, DialogTitle);
ITEXTVIEW_METHODS_IMPL(CDialogTitle, DialogTitle);

PInterface CDialogTitle::Probe(
    /* [in] */ REIID riid)
{
    if (riid == EIID_View) {
        return reinterpret_cast<PInterface>((View*)this);
    }
    else if (riid == EIID_TextView) {
        return reinterpret_cast<PInterface>((TextView*)this);
    }
    return _CDialogTitle::Probe(riid);
}

ECode CDialogTitle::constructor(
    /* [in] */ IContext * pCtx)
{
    return DialogTitle::Init(pCtx);
}

ECode CDialogTitle::constructor(
    /* [in] */ IContext * pCtx,
    /* [in] */ IAttributeSet * pAttrs)
{
    return DialogTitle::Init(pCtx, pAttrs);
}

ECode CDialogTitle::constructor(
    /* [in] */ IContext * pContext,
    /* [in] */ IAttributeSet * pAttrs,
    /* [in] */ Int32 defStyle)
{
    return DialogTitle::Init(pContext, pAttrs, defStyle);
}

ECode CDialogTitle::OnPreDraw(
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(result);
    *result = DialogTitle::OnPreDraw();

    return NOERROR;
}

}// namespace Internal
}// namespace Widget
}// namespace Droid
}// namespace Elastos
