
#ifndef __ELASTOS_DROID_INTERNAL_WIDGET_BACKGROUNDFALLBACK_H__
#define __ELASTOS_DROID_INTERNAL_WIDGET_BACKGROUNDFALLBACK_H__

#include <Elastos.Droid.Graphics.h>
#include <Elastos.Droid.Internal.h>
#include "elastos/droid/ext/frameworkext.h"
#include <elastos/core/Object.h>

using Elastos::Droid::Graphics::Drawable::IDrawable;
using Elastos::Droid::Graphics::ICanvas;
using Elastos::Droid::View::IViewGroup;
using Elastos::Droid::View::IView;

namespace Elastos {
namespace Droid {
namespace Internal {
namespace Widget {

/**
 * Helper class for drawing a fallback background in framework decor layouts.
 * Useful for when an app has not set a window background but we're asked to draw
 * an uncovered area.
 */
class BackgroundFallback
    : public Object
    , public IBackgroundFallback
{
public:
    CAR_INTERFACE_DECL();

    BackgroundFallback();

    ~BackgroundFallback();

    CARAPI constructor();

    CARAPI SetDrawable(
        /* [in] */ IDrawable* d);

    CARAPI HasFallback(
        /* [out] */ Boolean* hasFallback);

    CARAPI Draw(
        /* [in] */ IViewGroup* root,
        /* [in] */ ICanvas* c,
        /* [in] */ IView* content);

private:
    AutoPtr<IDrawable> mBackgroundFallback;
};

}// namespace Widget
}// namespace Internal
}// namespace Droid
}// namespace Elastos

#endif //__ELASTOS_DROID_INTERNAL_WIDGET_BACKGROUNDFALLBACK_H__
