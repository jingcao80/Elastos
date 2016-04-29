
#ifndef  __ELASTOS_DROID_SYSTEMUI_RECENTS_VIEWS_FIXEDSIZEIMAGEVIEW_H__
#define  __ELASTOS_DROID_SYSTEMUI_RECENTS_VIEWS_FIXEDSIZEIMAGEVIEW_H__

#include "_Elastos.Droid.SystemUI.h"
#include <elastos/droid/widget/ImageView.h>

using Elastos::Droid::Widget::ImageView;

namespace Elastos {
namespace Droid {
namespace SystemUI {
namespace Recents {
namespace Views {

/**
 * This is an optimized ImageView that does not trigger a requestLayout() or invalidate() when
 * setting the image to Null.
 */
class FixedSizeImageView
    : public ImageView
    , public IFixedSizeImageView
{
public:
    FixedSizeImageView();

    CAR_INTERFACE_DECL()

    CARAPI constructor(
        /* [in] */ IContext* context);

    CARAPI constructor(
        /* [in] */ IContext* context,
        /* [in] */ IAttributeSet* attrs);

    CARAPI constructor(
        /* [in] */ IContext* context,
        /* [in] */ IAttributeSet* attrs,
        /* [in] */ Int32 defStyleAttr);

    CARAPI constructor(
        /* [in] */ IContext* context,
        /* [in] */ IAttributeSet* attrs,
        /* [in] */ Int32 defStyleAttr,
        /* [in] */ Int32 defStyleRes);

    // @Override
    CARAPI RequestLayout();

    // @Override
    CARAPI Invalidate();

    // @Override
    CARAPI SetImageDrawable(
        /* [in] */ IDrawable* drawable);

public:
    Boolean mAllowRelayout;
    Boolean mAllowInvalidate;
};

} // namespace Views
} // namespace Recents
} // namespace SystemUI
} // namespace Droid
} // namespace Elastos

#endif // __ELASTOS_DROID_SYSTEMUI_RECENTS_VIEWS_FIXEDSIZEIMAGEVIEW_H__
