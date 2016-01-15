#ifndef __ELASTOS_DROID_WIDGET_CSTACKVIEWLAYOUTPARAMS_H__
#define __ELASTOS_DROID_WIDGET_CSTACKVIEWLAYOUTPARAMS_H__

#include "_Elastos_Droid_Widget_CStackViewLayoutParams.h"
#include "elastos/droid/widget/StackViewLayoutParams.h"


namespace Elastos {
namespace Droid {
namespace Widget {

CarClass(CStackViewLayoutParams), public StackViewLayoutParams
{
public:
    IVIEWGROUPLP_METHODS_DECL()

    CARAPI_(PInterface) Probe(
        /* [in] */ REIID riid);

    CARAPI constructor(
        /* [in] */ IStackView* sv,
        /* [in] */ IView* view);

    CARAPI constructor(
        /* [in] */ IStackView* sv,
        /* [in] */ IContext* context,
        /* [in] */ IAttributeSet* attrs);

    CARAPI GetInvalidateRect(
        /* [out] */ IRect** rect);

    CARAPI ResetInvalidateRect();

    CARAPI SetVerticalOffset(
        /* [in] */ Int32 newVerticalOffset);

    CARAPI GetVerticalOffset(
        /* [out] */ Int32* verticalOffset);

    CARAPI SetHorizontalOffset(
        /* [in] */ Int32 newHorizontalOffset);

    CARAPI GetHorizontalOffset(
        /* [out] */ Int32* horizontalOffset);

    CARAPI SetOffsets(
        /* [in] */ Int32 newHorizontalOffset,
        /* [in] */ Int32 newVerticalOffset);
};

}// namespace Widget
}// namespace Droid
}// namespace Elastos

#endif //__ELASTOS_DROID_WIDGET_CSTACKVIEWLAYOUTPARAMS_H__
