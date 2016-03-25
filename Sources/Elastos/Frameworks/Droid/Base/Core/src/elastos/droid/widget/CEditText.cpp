#include "elastos/droid/ext/frameworkext.h"
#include "CEditText.h"

using Elastos::Droid::View::EIID_View;

namespace Elastos {
namespace Droid {
namespace Widget {

IVIEW_METHODS_IMPL(CEditText, EditText)
ITEXTVIEW_METHODS_IMPL(CEditText, EditText);
IDRAWABLECALLBACK_METHODS_IMPL(CEditText, EditText)
IKEYEVENTCALLBACK_METHODS_IMPL(CEditText, EditText)
IACCESSIBILITYEVENTSOURCE_METHODS_IMPL(CEditText, EditText)


PInterface CEditText::Probe(
    /* [in] */ REIID riid)
{
    if (riid == EIID_View) {
        return reinterpret_cast<PInterface>(this);
    }
    else if (riid == EIID_TextView) {
        return reinterpret_cast<PInterface>(this);
    }
    return _CEditText::Probe(riid);
}

ECode CEditText::constructor(
        /* [in] */ IContext* context)
{
    return EditText::Init(context);
}

ECode CEditText::constructor(
        /* [in] */ IContext* context,
        /* [in] */ IAttributeSet* attrs)
{
    return EditText::Init(context, attrs);
}

ECode CEditText::constructor(
        /* [in] */ IContext* context,
        /* [in] */ IAttributeSet* attrs,
        /* [in] */ Int32 defStyle)
{
    return EditText::Init(context, attrs, defStyle);
}

ECode CEditText::SetSelection(
        /* [in] */ Int32 start,
        /* [in] */ Int32 stop)
{
    return EditText::SetSelection(start, stop);
}

ECode CEditText::SetSelection(
        /* [in] */ Int32 index)
{
    return EditText::SetSelection(index);
}

ECode CEditText::SelectAll()
{
    return EditText::SelectAll();
}

ECode CEditText::ExtendSelection(
        /* [in] */ Int32 index)
{
    return EditText::ExtendSelection(index);
}

ECode CEditText::OnPreDraw(
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(result);
    *result = EditText::OnPreDraw();

    return NOERROR;
}

}// namespace Widget {
}// namespace Droid
}// namespace Elastos
