#ifndef  __ELASTOS_DROID_LAUNCHER2_CSEARCHDROPTARGETBAR_H__
#define  __ELASTOS_DROID_LAUNCHER2_CSEARCHDROPTARGETBAR_H__

#include "_Elastos_Droid_Launcher2_CSearchDropTargetBar.h"
#include "elastos/droid/ext/frameworkext.h"
#include "elastos/droid/animation/AnimatorListenerAdapter.h"
#include "elastos/droid/widget/FrameLayout.h"
#include "Elastos.Droid.Animation.h"
#include "Elastos.Droid.Content.h"
#include "Elastos.Droid.Graphics.h"
#include "Elastos.Droid.Utility.h"
#include "Elastos.Droid.View.h"

using Elastos::Droid::Animation::IAnimator;
using Elastos::Droid::Animation::IObjectAnimator;
using Elastos::Droid::Animation::AnimatorListenerAdapter;
using Elastos::Droid::Content::IContext;
using Elastos::Droid::Graphics::IRect;
using Elastos::Droid::Graphics::Drawable::IDrawable;
using Elastos::Droid::Utility::IAttributeSet;
using Elastos::Droid::View::IView;
using Elastos::Droid::View::Animation::IAccelerateInterpolator;
using Elastos::Droid::Widget::FrameLayout;

namespace Elastos {
namespace Droid {
namespace Launcher2 {

/*
 * Ths bar will manage the transition between the QSB search bar and the delete drop
 * targets so that each of the individual IconDropTargets don't have to.
 */
CarClass(CSearchDropTargetBar)
    , public FrameLayout
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
        AutoPtr<IView> mV;
    };

public:
    CAR_INTERFACE_DECL();

    CAR_OBJECT_DECL();

    CSearchDropTargetBar();

    CARAPI constructor(
        /* [in] */ IContext* context,
        /* [in] */ IAttributeSet* attrs);

    CARAPI constructor(
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
    AutoPtr<IButtonDropTarget> mInfoDropTarget;
    AutoPtr<IButtonDropTarget> mDeleteDropTarget;
    Int32 mBarHeight;
    Boolean mDeferOnDragEnd;

    AutoPtr<IDrawable> mPreviousBackground;
    Boolean mEnableDropDownDropTargets;
};

} // namespace Launcher2
} // namespace Droid
} // namespace Elastos

#endif // __ELASTOS_DROID_LAUNCHER2_CSEARCHDROPTARGETBAR_H__