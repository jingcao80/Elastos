#include "elastos/droid/ext/frameworkext.h"
#include "elastos/droid/widget/CTableLayout.h"

using Elastos::Droid::View::EIID_View;
using Elastos::Droid::View::EIID_ViewGroup;

namespace Elastos {
namespace Droid {
namespace Widget {

IVIEW_METHODS_IMPL(CTableLayout, TableLayout)
IVIEWGROUP_METHODS_IMPL(CTableLayout, TableLayout)
IVIEWPARENT_METHODS_IMPL(CTableLayout, TableLayout)
IVIEWMANAGER_METHODS_IMPL(CTableLayout, TableLayout)
IDRAWABLECALLBACK_METHODS_IMPL(CTableLayout, TableLayout)
IKEYEVENTCALLBACK_METHODS_IMPL(CTableLayout, TableLayout)
IACCESSIBILITYEVENTSOURCE_METHODS_IMPL(CTableLayout, TableLayout)
ILINEARLAYOUT_METHODS_IMPL(CTableLayout, TableLayout)

PInterface CTableLayout::Probe(
    /* [in] */ REIID riid)
{
    if (riid == EIID_View) {
        return reinterpret_cast<PInterface>(this);
    }
    else if (riid == EIID_ViewGroup) {
        return reinterpret_cast<PInterface>(this);
    }
    return _CTableLayout::Probe(riid);
}

ECode CTableLayout::constructor(
    /* [in] */ IContext* context)
{
    return TableLayout::Init(context);
}

ECode CTableLayout::constructor(
    /* [in] */ IContext* context,
    /* [in] */ IAttributeSet* attrs)
{
    return TableLayout::Init(context, attrs);
}

ECode CTableLayout::IsShrinkAllColumns(
    /* [out] */ Boolean* shrinkAllColumns)
{
    VALIDATE_NOT_NULL(shrinkAllColumns);
    *shrinkAllColumns = TableLayout::IsShrinkAllColumns();
    return NOERROR;
}

ECode CTableLayout::SetShrinkAllColumns(
    /* [in] */ Boolean shrinkAllColumns)
{
    return TableLayout::SetShrinkAllColumns(shrinkAllColumns);
}

ECode CTableLayout::IsStretchAllColumns(
    /* [out] */ Boolean* stretchAllColumns)
{
    VALIDATE_NOT_NULL(stretchAllColumns);
    *stretchAllColumns = TableLayout::IsStretchAllColumns();
    return NOERROR;
}

ECode CTableLayout::SetStretchAllColumns(
    /* [in] */ Boolean stretchAllColumns)
{
    return TableLayout::SetStretchAllColumns(stretchAllColumns);
}

ECode CTableLayout::SetColumnCollapsed(
    /* [in] */ Int32 columnIndex,
    /* [in] */ Boolean isCollapsed)
{
    return TableLayout::SetColumnCollapsed(columnIndex, isCollapsed);
}

ECode CTableLayout::IsColumnCollapsed(
    /* [in] */ Int32 columnIndex,
    /* [out] */ Boolean* isCollapsed)
{
    VALIDATE_NOT_NULL(isCollapsed);
    *isCollapsed = TableLayout::IsColumnCollapsed(columnIndex);
    return NOERROR;
}

ECode CTableLayout::SetColumnStretchable(
    /* [in] */ Int32 columnIndex,
    /* [in] */ Boolean isStretchable)
{
    return TableLayout::SetColumnStretchable(columnIndex, isStretchable);
}

ECode CTableLayout::IsColumnStretchable(
    /* [in] */ Int32 columnIndex,
    /* [out] */ Boolean* isStretchable)
{
    VALIDATE_NOT_NULL(isStretchable);
    *isStretchable = TableLayout::IsColumnStretchable(columnIndex);
    return NOERROR;
}

ECode CTableLayout::SetColumnShrinkable(
    /* [in] */ Int32 columnIndex,
    /* [in] */ Boolean isShrinkable)
{
    return TableLayout::SetColumnShrinkable(columnIndex, isShrinkable);
}

ECode CTableLayout::IsColumnShrinkable(
    /* [in] */ Int32 columnIndex,
    /* [out] */ Boolean* isShrinkable)
{
    VALIDATE_NOT_NULL(isShrinkable);
    *isShrinkable = TableLayout::IsColumnShrinkable(columnIndex);
    return NOERROR;
}


}// namespace Widget
}// namespace Droid
}// namespace Elastos
