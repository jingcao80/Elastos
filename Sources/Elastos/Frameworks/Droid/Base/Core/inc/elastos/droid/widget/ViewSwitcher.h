
#ifndef __ELASTOS_DROID_WIDGET_VIEWSWITCHER_H__
#define __ELASTOS_DROID_WIDGET_VIEWSWITCHER_H__

#include "elastos/droid/widget/ViewAnimator.h"

/**
 * {@link ViewAnimator} that switches between two views, and has a factory
 * from which these views are created.  You can either use the factory to
 * create the views, or add them yourself.  A ViewSwitcher can only have two
 * child views, of which only one is shown at a time.
 */
namespace Elastos {
namespace Droid {
namespace Widget {

class ViewSwitcher
    : public ViewAnimator
    , public IViewSwitcher
{
public:
    CAR_INTERFACE_DECL();

    /**
     * Creates a new empty ViewSwitcher.
     *
     * @param context the application's environment
     */
    CARAPI constructor(
        /* [in] */ IContext* context);

    /**
     * Creates a new empty ViewSwitcher for the given context and with the
     * specified set attributes.
     *
     * @param context the application environment
     * @param attrs a collection of attributes
     */
    CARAPI constructor(
        /* [in] */ IContext* context,
        /* [in] */ IAttributeSet* attrs);

    /**
     * {@inheritDoc}
     *
     * @throws IllegalStateException if this switcher already contains two children
     */
    using ViewAnimator::AddView;

    CARAPI AddView(
        /* [in] */ IView* child,
        /* [in] */ Int32 index,
        /* [in] */ IViewGroupLayoutParams* params);

    CARAPI OnInitializeAccessibilityEvent(
        /* [in] */ IAccessibilityEvent* event);

    CARAPI OnInitializeAccessibilityNodeInfo(
        /* [in] */ IAccessibilityNodeInfo* info);

    /**
     * Returns the next view to be displayed.
     *
     * @return the view that will be displayed after the next views flip.
     */
    CARAPI GetNextView(
        /* [out] */ IView** v);

    /**
     * Sets the factory used to create the two views between which the
     * ViewSwitcher will flip. Instead of using a factory, you can call
     * {@link #addView(android.view.View, int, android.view.ViewGroup.LayoutParams)}
     * twice.
     *
     * @param factory the view factory used to generate the switcher's content
     */
    CARAPI SetFactory(
        /* [in] */ IViewFactory* factory);

    /**
     * Reset the ViewSwitcher to hide all of the existing views and to make it
     * think that the first time animation has not yet played.
     */
    CARAPI Reset();

private:
    CARAPI ObtainView(
        /* [out] */ IView** v);

protected:
    /**
     * The factory used to create the two children.
     */
    AutoPtr<IViewFactory> mFactory;
};

}// namespace Elastos
}// namespace Droid
}// namespace Widget

#endif // __ELASTOS_DROID_WIDGET_VIEWSWITCHER_H__
