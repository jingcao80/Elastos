#include "elastos/droid/widget/CTableRow.h"

using Elastos::Droid::View::EIID_View;
using Elastos::Droid::View::EIID_ViewGroup;

namespace Elastos {
namespace Droid {
namespace Widget {


IVIEW_METHODS_IMPL(CTableRow, TableRow)
IVIEWGROUP_METHODS_IMPL(CTableRow, TableRow)
IVIEWPARENT_METHODS_IMPL(CTableRow, TableRow)
IVIEWMANAGER_METHODS_IMPL(CTableRow, TableRow)
IDRAWABLECALLBACK_METHODS_IMPL(CTableRow, TableRow)
IKEYEVENTCALLBACK_METHODS_IMPL(CTableRow, TableRow)
IACCESSIBILITYEVENTSOURCE_METHODS_IMPL(CTableRow, TableRow)
ILINEARLAYOUT_METHODS_IMPL(CTableRow, TableRow)

PInterface CTableRow::Probe(
    /* [in] */ REIID riid)
{
    if (riid == EIID_View) {
        return reinterpret_cast<PInterface>((View*)this);
    }
    else if (riid == EIID_ViewGroup) {
        return reinterpret_cast<PInterface>((ViewGroup*)this);
    }
    else if (riid == EIID_TableRow) {
        return reinterpret_cast<PInterface>((TableRow*)this);
    }
    return _CTableRow::Probe(riid);
}

ECode CTableRow::constructor(
    /* [in] */ IContext* context)
{
    return TableRow::Init(context);
}

ECode CTableRow::constructor(
    /* [in] */ IContext* context,
    /* [in] */ IAttributeSet* attrs)
{
    return TableRow::Init(context, attrs);
}

ECode CTableRow::GetVirtualChildAt(
    /* [in] */  Int32 index,
    /* [out] */  IView** child)
{
    VALIDATE_NOT_NULL(child);
    AutoPtr<IView> view = TableRow::GetVirtualChildAt(index);
    *child = view;
    REFCOUNT_ADD(*child);
    return NOERROR;
}

ECode CTableRow::GetVirtualChildCount(
    /* [out] */ Int32* count)
{
    VALIDATE_NOT_NULL(count);
    *count = TableRow::GetVirtualChildCount();
    return NOERROR;
}

}// namespace Widget
}// namespace Droid
}// namespace Elastos
