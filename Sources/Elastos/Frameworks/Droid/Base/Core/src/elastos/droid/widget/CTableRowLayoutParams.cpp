#include "elastos/droid/ext/frameworkext.h"
#include "elastos/droid/widget/CTableRowLayoutParams.h"

namespace Elastos {
namespace Droid {
namespace Widget {

IVIEWGROUPLP_METHODS_IMPL(CTableRowLayoutParams, TableRowLayoutParams)
IVIEWGROUPMARGINLP_METHODS_IMPL(CTableRowLayoutParams, TableRowLayoutParams)
ILINEARLAYOUTLP_METHODS_IMPL(CTableRowLayoutParams, TableRowLayoutParams)

PInterface CTableRowLayoutParams::Probe(
    /* [in] */ REIID riid)
{
    if (riid == EIID_TableRowLayoutParams) {
        return reinterpret_cast<PInterface>(this);
    }
    return _CTableRowLayoutParams::Probe(riid);
}

ECode CTableRowLayoutParams::constructor()
{
    return TableRowLayoutParams::Init(
            IViewGroupLayoutParams::MATCH_PARENT,
            IViewGroupLayoutParams::WRAP_CONTENT);
}

ECode CTableRowLayoutParams::constructor(
    /* [in] */ IContext* context,
    /* [in] */ IAttributeSet* attrs)
{
    return TableRowLayoutParams::Init(context, attrs);
}

ECode CTableRowLayoutParams::constructor(
    /* [in] */ Int32 width,
    /* [in] */ Int32 height)
{
    return TableRowLayoutParams::Init(width, height);
}

ECode CTableRowLayoutParams::constructor(
    /* [in] */ Int32 width,
    /* [in] */ Int32 height,
    /* [in] */ Float weight)
{
    return TableRowLayoutParams::Init(width, height, weight);
}

ECode CTableRowLayoutParams::constructor(
    /* [in] */ Int32 column)
{
    return TableRowLayoutParams::Init(column);
}

ECode CTableRowLayoutParams::constructor(
    /* [in] */ IViewGroupLayoutParams* source)
{
    return TableRowLayoutParams::Init(source);
}

ECode CTableRowLayoutParams::constructor(
    /* [in] */ IViewGroupMarginLayoutParams* source)
{
    return TableRowLayoutParams::Init(source);
}

ECode CTableRowLayoutParams::SetColumn(
    /* [in] */ Int32 column)
{
    return TableRowLayoutParams::SetColumn(column);
}

ECode CTableRowLayoutParams::GetColumn(
    /* [out] */ Int32* column)
{
    VALIDATE_NOT_NULL(column);
    *column =  TableRowLayoutParams::GetColumn();
    return NOERROR;
}

ECode CTableRowLayoutParams::SetSpan(
    /* [in] */ Int32 span)
{
    return TableRowLayoutParams::SetSpan(span);
}

ECode CTableRowLayoutParams::GetSpan(
    /* [out] */ Int32* span)
{
    VALIDATE_NOT_NULL(span);
    *span =  TableRowLayoutParams::GetSpan();
    return NOERROR;
}


}// namespace Widget
}// namespace Droid
}// namespace Elastos
