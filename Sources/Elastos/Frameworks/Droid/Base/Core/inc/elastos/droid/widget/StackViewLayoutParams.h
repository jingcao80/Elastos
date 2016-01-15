#ifndef __ELASTOS_DROID_WIDGET_STACKVIEWLAYOUTPARAMS_H__
#define __ELASTOS_DROID_WIDGET_STACKVIEWLAYOUTPARAMS_H__

#include "elastos/droid/ext/frameworkext.h"
#include "elastos/droid/view/ViewGroup.h"

using Elastos::Droid::View::IView;
using Elastos::Droid::View::ViewGroup;
using Elastos::Droid::Graphics::IRect;
using Elastos::Droid::Graphics::IRectF;

namespace Elastos {
namespace Droid {
namespace Widget {

class StackViewLayoutParams : public ViewGroup::LayoutParams
{
public:
    StackViewLayoutParams(
        /* [in] */ IStackView* sv,
        /* [in] */ IView* view);

    StackViewLayoutParams(
        /* [in] */ IStackView* sv,
        /* [in] */ IContext* context,
        /* [in] */ IAttributeSet* attrs);

    CARAPI SetVerticalOffset(
        /* [in] */ Int32 newVerticalOffset);

    CARAPI_(Int32) GetVerticalOffset();

    CARAPI SetHorizontalOffset(
        /* [in] */ Int32 newHorizontalOffset);

    CARAPI_(Int32) GetHorizontalOffset();

    CARAPI SetOffsets(
        /* [in] */ Int32 newHorizontalOffset,
        /* [in] */ Int32 newVerticalOffset);

    CARAPI ResetInvalidateRect();

protected:
    StackViewLayoutParams();

    CARAPI Init(
        /* [in] */ IStackView* sv,
        /* [in] */ IView* view);

    CARAPI Init(
        /* [in] */ IStackView* sv,
        /* [in] */ IContext* context,
        /* [in] */ IAttributeSet* attrs);

    CARAPI_(void) InvalidateGlobalRegion(
        /* [in] */ IView* v,
        /* [in] */ IRect* r);

    CARAPI_(AutoPtr<IRect>) GetInvalidateRect();

public:
    Int32 mHorizontalOffset;
    Int32 mVerticalOffset;
    AutoPtr<IView> mView;

private:
    AutoPtr<IStackView> mStackView;
    AutoPtr<IRect> mParentRect;
    AutoPtr<IRect> mInvalidateRect;
    AutoPtr<IRectF> mInvalidateRectf;
    AutoPtr<IRect> mGlobalInvalidateRect;
};

} // namespace Widget
} // namespace Droid
} // namespace Elastos

#endif //__ELASTOS_DROID_WIDGET_STACKVIEWLAYOUTPARAMS_H__
