#include "elastos/droid/ext/frameworkext.h"
#include "elastos/droid/widget/internal/CActionBarViewHomeView.h"

using Elastos::Droid::View::EIID_View;
using Elastos::Droid::View::EIID_ViewGroup;
using Elastos::Droid::Widget::Internal::EIID_ActionBarViewHomeView;

namespace Elastos {
namespace Droid {
namespace Widget {
namespace Internal {

IVIEW_METHODS_IMPL(CActionBarViewHomeView, ActionBarViewHomeView)
IVIEWGROUP_METHODS_IMPL(CActionBarViewHomeView, ActionBarViewHomeView)
IVIEWPARENT_METHODS_IMPL(CActionBarViewHomeView, ActionBarViewHomeView)
IVIEWMANAGER_METHODS_IMPL(CActionBarViewHomeView, ActionBarViewHomeView)
IFRAMELAYOUT_METHODS_IMPL(CActionBarViewHomeView, ActionBarViewHomeView)
IDRAWABLECALLBACK_METHODS_IMPL(CActionBarViewHomeView, ActionBarViewHomeView)
IKEYEVENTCALLBACK_METHODS_IMPL(CActionBarViewHomeView, ActionBarViewHomeView)
IACCESSIBILITYEVENTSOURCE_METHODS_IMPL(CActionBarViewHomeView, ActionBarViewHomeView)

PInterface CActionBarViewHomeView::Probe(
    /* [in] */ REIID riid)
{
    if (riid == EIID_View) {
        return reinterpret_cast<PInterface>((View*)this);
    } else if (riid == EIID_ViewGroup) {
        return reinterpret_cast<PInterface>((ViewGroup*)this);
    } else if (riid == EIID_ActionBarViewHomeView) {
        return reinterpret_cast<PInterface>((ActionBarViewHomeView*)this);
    }
    return _CActionBarViewHomeView::Probe(riid);
}

ECode CActionBarViewHomeView::constructor(
    /* [in] */ IContext* context)
{
    return ActionBarViewHomeView::Init(context);
}

ECode CActionBarViewHomeView::constructor(
    /* [in] */ IContext* context,
    /* [in] */ IAttributeSet* attrs)
{
    return ActionBarViewHomeView::Init(context, attrs);
}

ECode CActionBarViewHomeView::SetUp(
    /* [in] */ Boolean isUp)
{
    return ActionBarViewHomeView::SetUp(isUp);
}

ECode CActionBarViewHomeView::SetIcon(
    /* [in] */ IDrawable* d)
{
    return ActionBarViewHomeView::SetIcon(d);
}

ECode CActionBarViewHomeView::GetStartOffset(
    /* [out] */ Int32* offset)
{
    VALIDATE_NOT_NULL(offset);
    *offset = ActionBarViewHomeView::GetStartOffset();
    return NOERROR;
}

}// namespace Internal
}// namespace Widget
}// namespace Droid
}// namespace Elastos
