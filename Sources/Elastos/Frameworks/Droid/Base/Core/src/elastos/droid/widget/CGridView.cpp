#include "elastos/droid/ext/frameworkext.h"
#include "elastos/droid/widget/CGridView.h"

using Elastos::Droid::View::EIID_View;
using Elastos::Droid::View::EIID_ViewGroup;

namespace Elastos {
namespace Droid {
namespace Widget {

IVIEW_METHODS_IMPL(CGridView, GridView)
IVIEWGROUP_METHODS_IMPL(CGridView, GridView)
IVIEWPARENT_METHODS_IMPL(CGridView, GridView)
IVIEWMANAGER_METHODS_IMPL(CGridView, GridView)
IDRAWABLECALLBACK_METHODS_IMPL(CGridView, GridView)
IKEYEVENTCALLBACK_METHODS_IMPL(CGridView, GridView)
IACCESSIBILITYEVENTSOURCE_METHODS_IMPL(CGridView, GridView)
IABSLISTVIEW_METHODS_IMPL(CGridView, GridView)
IABSLISTVIEW_INTERFACE_METHODS_IMPL(CGridView, GridView)
IADAPTERVIEW_METHODS_IMPL(CGridView, GridView)

PInterface CGridView::Probe(
        /* [in] */ REIID riid)
{
    if (riid == EIID_View) {
        return reinterpret_cast<PInterface>(this);
    } else if (riid == EIID_ViewGroup) {
        return reinterpret_cast<PInterface>(this);
    }
    return _CGridView::Probe(riid);
}

ECode CGridView::constructor(
        /* [in] */ IContext* context)
{
    return GridView::Init(context);
}

ECode CGridView::constructor(
        /* [in] */ IContext* context,
        /* [in] */ IAttributeSet* attrs)
{
    return GridView::Init(context, attrs);
}

ECode CGridView::constructor(
        /* [in] */ IContext* context,
        /* [in] */ IAttributeSet* attrs,
        /* [in] */ Int32 defStyle)
{
    return GridView::Init(context, attrs, defStyle);
}

ECode CGridView::GetGravity(
        /* [out] */ Int32* gravity)
{
    VALIDATE_NOT_NULL(gravity);
    *gravity = GridView::GetGravity();
    return NOERROR;
}

ECode CGridView::SetGravity(
        /* [in] */ Int32 gravity)
{
    return GridView::SetGravity(gravity);
}

ECode CGridView::SetHorizontalSpacing(
        /* [in] */ Int32 horizontalSpacing)
{
    return GridView::SetHorizontalSpacing(horizontalSpacing);
}

ECode CGridView::SetVerticalSpacing(
        /* [in] */ Int32 verticalSpacing)
{
    return GridView::SetVerticalSpacing(verticalSpacing);
}

ECode CGridView::SetStretchMode(
        /* [in] */ Int32 stretchMode)
{
    return GridView::SetStretchMode(stretchMode);
}

ECode CGridView::GetStretchMode(
        /* [out] */ Int32* mode)
{
    VALIDATE_NOT_NULL(mode);
    *mode = GridView::GetStretchMode();
    return NOERROR;
}

ECode CGridView::SetColumnWidth(
        /* [in] */ Int32 columnWidth)
{
    return GridView::SetColumnWidth(columnWidth);
}

ECode CGridView::SetNumColumns(
        /* [in] */ Int32 numColumns)
{
    return GridView::SetNumColumns(numColumns);
}

ECode CGridView::SmoothScrollByOffset(
        /* [in] */ Int32 offset)
{
    return GridView::SmoothScrollByOffset(offset);
}

ECode CGridView::GetHorizontalSpacing(
        /* [out] */ Int32* spacing)
{
    VALIDATE_NOT_NULL(spacing);
    *spacing = GridView::GetHorizontalSpacing();
    return NOERROR;
}

ECode CGridView::GetRequestedHorizontalSpacing(
        /* [out] */ Int32* spacing)
{
    VALIDATE_NOT_NULL(spacing);
    *spacing = GridView::GetRequestedHorizontalSpacing();
    return NOERROR;
}

ECode CGridView::GetVerticalSpacing(
        /* [out] */ Int32* spacing)
{
    VALIDATE_NOT_NULL(spacing);
    *spacing = GridView::GetVerticalSpacing();
    return NOERROR;
}

ECode CGridView::GetColumnWidth(
        /* [out] */ Int32* width)
{
    VALIDATE_NOT_NULL(width);
    *width = GridView::GetColumnWidth();
    return NOERROR;
}

ECode CGridView::GetRequestedColumnWidth(
        /* [out] */ Int32* width)
{
    VALIDATE_NOT_NULL(width);
    *width = GridView::GetRequestedColumnWidth();
    return NOERROR;
}

ECode CGridView::GetNumColumns(
        /* [out] */ Int32* columns)
{
    VALIDATE_NOT_NULL(columns);
    *columns = GridView::GetNumColumns();
    return NOERROR;
}

}// namespace Widget
}// namespace Droid
}// namespace Elastos
