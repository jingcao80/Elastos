
#ifndef __ELASTOS_DROID_WIDGET_IMAGESWITCHER_H__
#define __ELASTOS_DROID_WIDGET_IMAGESWITCHER_H__

#include "elastos/droid/widget/ViewSwitcher.h"

using Elastos::Droid::Content::IContext;
using Elastos::Droid::Graphics::Drawable::IDrawable;
using Elastos::Droid::Net::IUri;
using Elastos::Droid::Utility::IAttributeSet;
using Elastos::Droid::View::Accessibility::IAccessibilityEvent;
using Elastos::Droid::View::Accessibility::IAccessibilityNodeInfo;

namespace Elastos {
namespace Droid {
namespace Widget {

class ImageSwitcher
    : public ViewSwitcher
    , public IImageSwitcher
{
public:
    CAR_INTERFACE_DECL()

    CARAPI constructor(
        /* [in] */ IContext* context);

    CARAPI constructor(
        /* [in] */ IContext* context,
        /* [in] */ IAttributeSet* attrs);

    CARAPI SetImageResource(
        /* [in] */ Int32 resid);

    CARAPI SetImageURI(
        /* [in] */ IUri* uri);

    CARAPI SetImageDrawable(
        /* [in] */ IDrawable* drawable);

    CARAPI OnInitializeAccessibilityEvent(
        /* [in] */ IAccessibilityEvent* event);

    CARAPI OnInitializeAccessibilityNodeInfo(
        /* [in] */ IAccessibilityNodeInfo* info);

private:
    const static String IMAGESWITCHER_NAME;
};

} // namespace Widget
} // namespace Droid
} // namespace Elastos

#endif // __ELASTOS_DROID_WIDGET_IMAGESWITCHER_H__
