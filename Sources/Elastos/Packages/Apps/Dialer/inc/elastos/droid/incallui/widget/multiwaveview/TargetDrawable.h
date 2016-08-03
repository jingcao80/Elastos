
#ifndef __ELASTOS_DROID_INCALLUI_WIDGET_MULTIWAVEVIEW_TARGETDRAWABLE_H__
#define __ELASTOS_DROID_INCALLUI_WIDGET_MULTIWAVEVIEW_TARGETDRAWABLE_H__

#include "Elastos.Droid.Content.h"
#include "Elastos.Droid.Graphics.h"
#include "_Elastos.Droid.Dialer.h"
#include <elastos/core/Object.h>

using Elastos::Droid::Content::Res::IResources;
using Elastos::Droid::Graphics::ICanvas;
using Elastos::Core::Object;

namespace Elastos {
namespace Droid {
namespace InCallUI {
namespace Widget {
namespace MultiwaveView {

class TargetDrawable
    : public Object
{
public:
    /**
     * This is changed from the framework version to pass in the number of drawables in the
     * container. The framework version relies on private api's to get the count from
     * StateListDrawable.
     *
     * @param res
     * @param resId
     * @param count The number of drawables in the resource.
     */
    TargetDrawable(
        /* [in] */ IResources* res,
        /* [in] */ Int32 resId,
        /* [in] */ Int32 count);

    CARAPI_(void) SetDrawable(
        /* [in] */ IResources* res,
        /* [in] */ Int32 resId);

    CARAPI_(void) SetState(
        /* [in] */ ArrayOf<Int32>* state);

    /**
     * Returns true if this target is enabled. Typically an enabled target contains a valid
     * drawable in a valid state. Currently all targets with valid drawables are valid.
     *
     * @return
     */
    CARAPI_(Boolean) IsEnabled();

    CARAPI_(void) SetX(
        /* [in] */ Float x);

    CARAPI_(void) SetY(
        /* [in] */ Float y);

    CARAPI_(void) SetScaleX(
        /* [in] */ Float x);

    CARAPI_(void) SetScaleY(
        /* [in] */ Float y);

    CARAPI_(void) SetAlpha(
        /* [in] */ Float alpha);

    CARAPI_(Float) GetX();

    CARAPI_(Float) GetY();

    CARAPI_(void) SetPositionX(
        /* [in] */ Float x);

    CARAPI_(void) SetPositionY(
        /* [in] */ Float y);

    CARAPI_(Int32) GetWidth();

    CARAPI_(Int32) GetHeight();

    CARAPI_(void) Draw(
        /* [in] */ ICanvas* canvas);

    CARAPI_(void) SetEnabled(
        /* [in] */ Boolean enabled);

    CARAPI_(Int32) GetResourceId();

public:
    static AutoPtr<ArrayOf<Int32> > STATE_ACTIVE;
    static AutoPtr<ArrayOf<Int32> > STATE_INACTIVE;
    static AutoPtr<ArrayOf<Int32> > STATE_FOCUSED;
};

} // namespace MultiwaveView
} // namespace Widget
} // namespace InCallUI
} // namespace Droid
} // namespace Elastos

#endif // __ELASTOS_DROID_INCALLUI_WIDGET_MULTIWAVEVIEW_TARGETDRAWABLE_H__
