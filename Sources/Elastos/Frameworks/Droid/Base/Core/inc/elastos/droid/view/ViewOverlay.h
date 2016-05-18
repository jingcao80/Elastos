
#ifndef __ELASTOS_DROID_VIEW_VIEWOVERLAY_H__
#define __ELASTOS_DROID_VIEW_VIEWOVERLAY_H__

#include "Elastos.Droid.View.h"
#include "elastos/droid/ext/frameworkext.h"
#include <elastos/core/Object.h>

using Elastos::Droid::Content::IContext;
using Elastos::Droid::Graphics::Drawable::IDrawable;
using Elastos::Core::Object;

namespace Elastos {
namespace Droid {
namespace View {

/**
 * An overlay is an extra layer that sits on top of a View (the "host view")
 * which is drawn after all other content in that view (including children,
 * if the view is a ViewGroup). Interaction with the overlay layer is done
 * by adding and removing drawables.
 *
 * <p>An overlay requested from a ViewGroup is of type {@link ViewGroupOverlay},
 * which also supports adding and removing views.</p>
 *
 * @see View#getOverlay() View.getOverlay()
 * @see ViewGroup#getOverlay() ViewGroup.getOverlay()
 * @see ViewGroupOverlay
 */
class ViewOverlay
    : public Object
    , public IViewOverlay
{
public:
    CAR_INTERFACE_DECL()

    CARAPI constructor(
        /* [in] */ IContext* context,
        /* [in] */ IView* hostView);

    /**
     * Used internally by View and ViewGroup to handle drawing and invalidation
     * of the overlay
     * @return
     */
    CARAPI GetOverlayView(
        /* [out] */ IViewGroup** group);

    /**
     * Adds a Drawable to the overlay. The bounds of the drawable should be relative to
     * the host view. Any drawable added to the overlay should be removed when it is no longer
     * needed or no longer visible.
     *
     * @param drawable The Drawable to be added to the overlay. This drawable will be
     * drawn when the view redraws its overlay.
     * @see #remove(Drawable)
     */
    CARAPI Add(
        /* [in] */ IDrawable* drawable);

    /**
     * Removes the specified Drawable from the overlay.
     *
     * @param drawable The Drawable to be removed from the overlay.
     * @see #add(Drawable)
     */
    CARAPI Remove(
        /* [in] */ IDrawable* drawable);

    /**
     * Removes all content from the overlay.
     */
    CARAPI Clear();

    CARAPI IsEmpty(
        /* [out] */ Boolean* empyt);

protected:
    /**
     * The actual container for the drawables (and views, if it's a ViewGroupOverlay).
     * All of the management and rendering details for the overlay are handled in
     * OverlayViewGroup.
     */
    AutoPtr<IOverlayViewGroup> mOverlayViewGroup;
};

} // namespace View
} // namepsace Droid
} // namespace Elastos

#endif // __ELASTOS_DROID_VIEW_VIEWOVERLAY_H__
