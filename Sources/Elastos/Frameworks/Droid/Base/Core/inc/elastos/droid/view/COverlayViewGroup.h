
#ifndef __ELASTOS_DROID_VIEW_COVERLAYVIEWGROUP_H__
#define __ELASTOS_DROID_VIEW_COVERLAYVIEWGROUP_H__

#include "_Elastos_Droid_View_COverlayViewGroup.h"
#include "elastos/droid/view/ViewGroup.h"
#include <elastos/core/Object.h>

using Elastos::Core::Object;
using Elastos::Utility::IArrayList;

namespace Elastos {
namespace Droid {
namespace View {

/**
 * OverlayViewGroup is a container that View and ViewGroup use to host
 * drawables and views added to their overlays  ({@link ViewOverlay} and
 * {@link ViewGroupOverlay}, respectively). Drawables are added to the overlay
 * via the add/remove methods in ViewOverlay, Views are added/removed via
 * ViewGroupOverlay. These drawable and view objects are
 * drawn whenever the view itself is drawn; first the view draws its own
 * content (and children, if it is a ViewGroup), then it draws its overlay
 * (if it has one).
 *
 * <p>Besides managing and drawing the list of drawables, this class serves
 * two purposes:
 * (1) it noops layout calls because children are absolutely positioned and
 * (2) it forwards all invalidation calls to its host view. The invalidation
 * redirect is necessary because the overlay is not a child of the host view
 * and invalidation cannot therefore follow the normal path up through the
 * parent hierarchy.</p>
 *
 * @see View#getOverlay()
 * @see ViewGroup#getOverlay()
 */
CarClass(COverlayViewGroup)
    , public ViewGroup
    , public IOverlayViewGroup
{
public:
    CAR_INTERFACE_DECL()

    CAR_OBJECT_DECL()

    CARAPI constructor(
        /* [in] */ IContext* context,
        /* [in] */ IView* hostView);

    CARAPI Add(
        /* [in] */ IDrawable* drawable);

    CARAPI Remove(
        /* [in] */ IDrawable* drawable);

    CARAPI Add(
        /* [in] */ IView* child);

    CARAPI Remove(
        /* [in] */ IView* view);

    CARAPI Clear();

    CARAPI IsEmpty(
        /* [out] */ Boolean* empty);

    // @Override
    CARAPI InvalidateDrawable(
        /* [in] */ IDrawable* drawable);

    /*
     The following invalidation overrides exist for the purpose of redirecting invalidation to
     the host view. The overlay is not parented to the host view (since a View cannot be a
     parent), so the invalidation cannot proceed through the normal parent hierarchy.
     There is a built-in assumption that the overlay exactly covers the host view, therefore
     the invalidation rectangles received do not need to be adjusted when forwarded to
     the host view.
     */
    // @Override
    CARAPI Invalidate(
        /* [in] */ IRect* dirty);

    // @Override
    CARAPI Invalidate(
        /* [in] */ Int32 l,
        /* [in] */ Int32 t,
        /* [in] */ Int32 r,
        /* [in] */ Int32 b);

    // @Override
    CARAPI Invalidate();

    /**
     * @hide
     */
    // @Override
    CARAPI DamageChild(
        /* [in] */ IView* child,
        /* [in] */ IRect* dirty);

    // @Override
    virtual CARAPI InvalidateChildInParent(
        /* [in] */ ArrayOf<Int32>* location,
        /* [in] */ IRect* dirty,
        /* [out] */ IViewParent** parent);

protected:
    // @Override
    CARAPI_(Boolean) VerifyDrawable(
        /* [in] */ IDrawable* who);

    // @Override
    CARAPI DispatchDraw(
        /* [in] */ ICanvas* canvas);

    // @Override
    CARAPI OnLayout(
        /* [in] */ Boolean changed,
        /* [in] */ Int32 l,
        /* [in] */ Int32 t,
        /* [in] */ Int32 r,
        /* [in] */ Int32 b);

    // @Override
    CARAPI_(void) Invalidate(
        /* [in] */ Boolean invalidateCache);

    // @Override
    CARAPI_(void) InvalidateViewProperty(
        /* [in] */ Boolean invalidateParent,
        /* [in] */ Boolean forceRedraw);

    // @Override
    CARAPI_(void) InvalidateParentCaches();

    // @Override
    CARAPI_(void) InvalidateParentIfNeeded();

    /**
     * @hide
     */
    // @Override
    CARAPI_(AutoPtr<IViewParent>) DamageChildInParent(
        /* [in] */ Int32 left,
        /* [in] */ Int32 top,
        /* [in] */ IRect* dirty);

private:
    /**
     * The View for which this is an overlay. Invalidations of the overlay are redirected to
     * this host view.
     */
    IView* mHostView;   // weak-ref

    /**
     * The set of drawables to draw when the overlay is rendered.
     */
    AutoPtr<IArrayList> mDrawables;
};

} // namespace View
} // namepsace Droid
} // namespace Elastos

#endif // __ELASTOS_DROID_VIEW_COVERLAYVIEWGROUP_H__
