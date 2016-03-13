#ifndef  __ELASTOS_DROID_LAUNCHER2_SEARCHDROPTARGETBAR_H__
#define  __ELASTOS_DROID_LAUNCHER2_SEARCHDROPTARGETBAR_H__

#include "elastos/droid/ext/frameworkext.h"

namespace Elastos {
namespace Droid {
namespace Launcher2 {

class SearchDropTargetBar
    : public FrameLayout
    , public ISearchDropTargetBar
    , public IDragControllerDragListener
{
private:
    class MyAdapter
        : public AnimatorListenerAdapter
    {
    public:
        MyAdapter(
            /* [in] */ IView* v);

        //@Override
        CARAPI OnAnimationEnd(
            /* [in] */ IAnimator* animation);

    private:
        IView* mV;
    };

public:
    CAR_INTERFACE_DECL();

    SearchDropTargetBar(
        /* [in] */ IContext* context,
        /* [in] */ IAttributeSet* attrs);

    SearchDropTargetBar(
        /* [in] */ IContext* context,
        /* [in] */ IAttributeSet* attrs,
        /* [in] */ Int32 defStyle);

    CARAPI Setup(
        /* [in] */ ILauncher* launcher,
        /* [in] */ IDragController* dragController);

    CARAPI FinishAnimations();

    /*
     * Shows and hides the search bar.
     */
    CARAPI ShowSearchBar(
        /* [in] */ Boolean animated);

    CARAPI HideSearchBar(
        /* [in] */ Boolean animated);

    /*
     * Gets various transition durations.
     */
    CARAPI GetTransitionInDuration(
        /* [out] */ Int32* result);

    CARAPI GetTransitionOutDuration(
        /* [out] */ Int32* result);

    /*
     * DragController.DragListener implementation
     */
    //@Override
    CARAPI OnDragStart(
        /* [in] */ IDragSource* source,
        /* [in] */ IInterface* info,
        /* [in] */ Int32 dragAction);

    CARAPI DeferOnDragEnd();

    //@Override
    CARAPI OnDragEnd();

    CARAPI OnSearchPackagesChanged(
        /* [in] */ Boolean searchVisible,
        /* [in] */ Boolean voiceVisible);

    CARAPI GetSearchBarBounds(
        /* [out] */ IRect** rect);

protected:
    //@Override
    CARAPI OnFinishInflate();

private:
    CARAPI_(void) PrepareStartAnimation(
        /* [in] */ IView* v);

    CARAPI_(void) SetupAnimation(
        /* [in] */ IObjectAnimator* anim,
        /* [in] */ IView* v);

private:
    static const Int32 sTransitionInDuration;
    static const Int32 sTransitionOutDuration;

    AutoPtr<IObjectAnimator> mDropTargetBarAnim;
    AutoPtr<IObjectAnimator> mQSBSearchBarAnim;
    static AutoPtr<IAccelerateInterpolator> sAccelerateInterpolator;

    Boolean mIsSearchBarHidden;
    AutoPtr<IView> mQSBSearchBar;
    AutoPtr<IView> mDropTargetBar;
    AutoPtr<ButtonDropTarget> mInfoDropTarget;
    AutoPtr<ButtonDropTarget> mDeleteDropTarget;
    Int32 mBarHeight;
    Boolean mDeferOnDragEnd;

    AutoPtr<IDrawable> mPreviousBackground;
    Boolean mEnableDropDownDropTargets;
};

} // namespace Launcher2
} // namespace Droid
} // namespace Elastos

#endif // __ELASTOS_DROID_LAUNCHER2_SEARCHDROPTARGETBAR_H__