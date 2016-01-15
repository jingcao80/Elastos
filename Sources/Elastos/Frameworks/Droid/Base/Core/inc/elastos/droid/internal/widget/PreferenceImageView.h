
#ifndef __ELASTOS_DROID_INTERNAL_WIDGET_PREFERENCEIMAGEVIEW_H__
#define __ELASTOS_DROID_INTERNAL_WIDGET_PREFERENCEIMAGEVIEW_H__

#include "elastos/droid/ext/frameworkext.h"
#include "elastos/droid/widget/ImageView.h"

using Elastos::Droid::Content::IContext;
using Elastos::Droid::Internal::Widget::IPreferenceImageView;
using Elastos::Droid::Utility::IAttributeSet;
using Elastos::Droid::Widget::ImageView;

namespace Elastos {
namespace Droid {
namespace Internal {
namespace Widget {

class PreferenceImageView
    : public ImageView
    , public IPreferenceImageView
{
public:
    CAR_INTERFACE_DECL()

    PreferenceImageView();

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

protected:
    // @Override
    CARAPI_(void) OnMeasure(
        /* [in] */ Int32 widthMeasureSpec,
        /* [in] */ Int32 heightMeasureSpec);
};

} // namespace Widget
} // namespace Internal
} // namespace Droid
} // namespace Elastos

#endif // __ELASTOS_DROID_INTERNAL_WIDGET_PREFERENCEIMAGEVIEW_H__

