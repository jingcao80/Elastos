
#ifndef __ELASTOS_DROID_VIEW_VIEWOUTLINEPROVIDER_H__
#define __ELASTOS_DROID_VIEW_VIEWOUTLINEPROVIDER_H__

#include "Elastos.Droid.View.h"
#include <elastos/droid/ext/frameworkext.h>
#include <elastos/core/Object.h>

using Elastos::Droid::Graphics::IOutline;
using Elastos::Core::Object;

namespace Elastos {
namespace Droid {
namespace View {

/**
 * Interface by which a View builds its {@link Outline}, used for shadow casting and clipping.
 */
class ECO_PUBLIC ViewOutlineProvider
    : public Object
    , public IViewOutlineProvider
{
public:
    CAR_INTERFACE_DECL();

    /**
     * Called to get the provider to populate the Outline.
     *
     * This method will be called by a View when its owned Drawables are invalidated, when the
     * View's size changes, or if {@link View#invalidateOutline()} is called
     * explicitly.
     *
     * The input outline is empty and has an alpha of <code>1.0f</code>.
     *
     * @param view The view building the outline.
     * @param outline The empty outline to be populated.
     */
    CARAPI GetOutline(
        /* [in] */ IView* view,
        /* [in] */ IOutline* outline) = 0;

private:
    static CARAPI_(Boolean) InitStatic();

public:
    /**
     * Default outline provider for Views, which queries the Outline from the View's background,
     * or generates a 0 alpha, rectangular Outline the size of the View if a background
     * isn't present.
     *
     * @see Drawable#getOutline(Outline)
     */
    static AutoPtr<IViewOutlineProvider> BACKGROUND;

    /**
     * Maintains the outline of the View to match its rectangular bounds,
     * at <code>1.0f</code> alpha.
     *
     * This can be used to enable Views that are opaque but lacking a background cast a shadow.
     */
    static AutoPtr<IViewOutlineProvider> BOUNDS;

    /**
     * Maintains the outline of the View to match its rectangular padded bounds,
     * at <code>1.0f</code> alpha.
     *
     * This can be used to enable Views that are opaque but lacking a background cast a shadow.
     */
    static AutoPtr<IViewOutlineProvider> PADDED_BOUNDS;

private:
    ECO_LOCAL static Boolean sInit;
};

class ECO_PUBLIC BackgroundViewOutlineProvider
    : public ViewOutlineProvider
{
public:
    CARAPI GetOutline(
        /* [in] */ IView* view,
        /* [in] */ IOutline* outline);
};

class ECO_PUBLIC BoundsViewOutlineProvider
    : public ViewOutlineProvider
{
public:
    CARAPI GetOutline(
        /* [in] */ IView* view,
        /* [in] */ IOutline* outline);
};

class ECO_PUBLIC PaddedViewOutlineProvider
    : public ViewOutlineProvider
{
public:
    CARAPI GetOutline(
        /* [in] */ IView* view,
        /* [in] */ IOutline* outline);
};

} // namespace View
} // namepsace Droid
} // namespace Elastos

#endif // __ELASTOS_DROID_VIEW_VIEWOUTLINEPROVIDER_H__
