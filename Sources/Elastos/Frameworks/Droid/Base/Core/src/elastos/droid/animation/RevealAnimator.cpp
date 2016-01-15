
#include "elastos/droid/animation/RevealAnimator.h"

namespace Elastos {
namespace Droid {
namespace Animation {

RevealAnimator::RevealAnimator(
    /* [in] */ IView* clipView,
    /* [in] */ Int32 x,
    /* [in] */ Int32 y,
    /* [in] */ Float startRadius,
    /* [in] */ Float endRadius)
    : mClipView(clipView)
{
    RenderNodeAnimator::constructor(x, y, startRadius, endRadius);
    SetTarget(mClipView);
}

void RevealAnimator::OnFinished()
{
    mClipView->SetRevealClip(FALSE, 0, 0, 0);
    RenderNodeAnimator::OnFinished();
}

}   //namespace Animation
}   //namespace Droid
}   //namespace Elastos
