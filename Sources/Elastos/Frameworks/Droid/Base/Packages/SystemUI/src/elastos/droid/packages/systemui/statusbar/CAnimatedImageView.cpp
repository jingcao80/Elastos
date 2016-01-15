#include "elastos/droid/systemui/statusbar/CAnimatedImageView.h"

using Elastos::Droid::View::EIID_View;

namespace Elastos {
namespace Droid {
namespace SystemUI {
namespace StatusBar {


IVIEW_METHODS_IMPL(CAnimatedImageView, AnimatedImageView)
IDRAWABLECALLBACK_METHODS_IMPL(CAnimatedImageView, AnimatedImageView)
IKEYEVENTCALLBACK_METHODS_IMPL(CAnimatedImageView, AnimatedImageView)
IACCESSIBILITYEVENTSOURCE_METHODS_IMPL(CAnimatedImageView, AnimatedImageView)
IIMAGEVIEW_METHODS_IMPL(CAnimatedImageView, AnimatedImageView)

PInterface CAnimatedImageView::Probe(
    /* [in] */ REIID riid)
{
    if (riid == EIID_View) {
        return reinterpret_cast<PInterface>((View*)this);
    }
    return _CAnimatedImageView::Probe(riid);
}

ECode CAnimatedImageView::constructor(
    /* [in] */ IContext* context)
{
    return AnimatedImageView::Init(context);
}

ECode CAnimatedImageView::constructor(
    /* [in] */ IContext* context,
    /* [in] */ IAttributeSet* attrs)
{
    return AnimatedImageView::Init(context, attrs);
}

ECode CAnimatedImageView::SetAlpha(
    /* [in] */ Int32 alpha)
{
    return AnimatedImageView::SetAlpha(alpha);
}

}// namespace StatusBar
}// namespace SystemUI
}// namespace Droid
}// namespace Elastos
