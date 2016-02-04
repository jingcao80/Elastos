
#ifndef  __ELASTOS_DROID_PACKAGES_SYSTEMUI_CDESSERTCASEVIEW_H__
#define  __ELASTOS_DROID_PACKAGES_SYSTEMUI_CDESSERTCASEVIEW_H__

#include "_Elastos_Droid_Packages_SystemUI_CDessertCaseView.h"
#include "Elastos.Droid.Os.h"
#include <elastos/droid/animation/AnimatorListenerAdapter.h>
#include <elastos/droid/os/Runnable.h>
#include <elastos/droid/widget/FrameLayout.h>

using Elastos::Droid::Animation::AnimatorListenerAdapter;
using Elastos::Droid::Animation::IAnimator;
using Elastos::Droid::Animation::IAnimatorListener;
using Elastos::Droid::Content::IContext;
using Elastos::Droid::Graphics::IBitmap;
using Elastos::Droid::Graphics::ICanvas;
using Elastos::Droid::Graphics::IPoint;
using Elastos::Droid::Os::Runnable;
using Elastos::Droid::Os::IHandler;
using Elastos::Droid::Utility::IAttributeSet;
using Elastos::Droid::Utility::ISparseArray;
using Elastos::Droid::View::IViewOnClickListener;
using Elastos::Droid::View::IView;
using Elastos::Droid::Widget::FrameLayout;
using Elastos::Droid::Widget::IImageView;
using Elastos::Utility::ISet;
using Elastos::Utility::IHashSet;

namespace Elastos {
namespace Droid {
namespace Packages {
namespace SystemUI {

CarClass(CDessertCaseView)
    , public FrameLayout
    , public IDessertCaseView
{
public:
    class RescalingContainer
       : public FrameLayout
    {
    public:
        RescalingContainer(
            /* [in] */ IContext* context);

        void SetView(
            /* [in] */ IDessertCaseView* v);

        void SetDarkness(
            /* [in] */ Float p);

        Float GetDarkness();

    protected:
        CARAPI OnLayout(
            /* [in] */ Boolean changed,
            /* [in] */ Int32 left,
            /* [in] */ Int32 top,
            /* [in] */ Int32 right,
            /* [in] */ Int32 bottom);

    private:
        AutoPtr<IDessertCaseView> mView;
        Float mDarkness;
    };

private:
    class Runnable_1
        : public Runnable
    {
    public:
        Runnable_1(
            /* [in] */ CDessertCaseView* owner);

        CARAPI Run();

    public:
        CDessertCaseView* mOwner;
    };

    class AnimatorListenerAdapter_1
        : public AnimatorListenerAdapter
    {
    public:
        AnimatorListenerAdapter_1(
            /* [in] */ IView* v);

        CARAPI OnAnimationStart(
            /* [in] */ IAnimator* animator);

        CARAPI OnAnimationEnd(
            /* [in] */ IAnimator* animator);

    public:
        AutoPtr<IView> mV;
    };

    class AnimatorListener
        : public Object
        , public IAnimatorListener
    {
    public:
        CAR_INTERFACE_DECL()

        AnimatorListener(
            /* [in] */ CDessertCaseView* owner,
            /* [in] */ IView* view);

        CARAPI OnAnimationStart(
            /* [in] */ IAnimator* animator);

        CARAPI OnAnimationEnd(
            /* [in] */ IAnimator* animator);

        CARAPI OnAnimationCancel(
            /* [in] */ IAnimator* animator);

        CARAPI OnAnimationRepeat(
            /* [in] */ IAnimator* animator);

    public:
        CDessertCaseView* mOwner;
        AutoPtr<IView> mView;
    };

    class OnClickListener
        : public Object
        , public IViewOnClickListener
    {
    public:
        CAR_INTERFACE_DECL()

        OnClickListener(
            /* [in] */ CDessertCaseView* owner,
            /* [in] */ IImageView* v);

        CARAPI OnClick(
            /* [in] */ IView* view);

    public:
        CDessertCaseView* mOwner;
        AutoPtr<IImageView> mV;
    };

    class Runnable_2
        : public Runnable
    {
    public:
        Runnable_2(
            /* [in] */ CDessertCaseView* owner);

        CARAPI Run();

    public:
        CDessertCaseView* mOwner;
    };

public:
    CAR_OBJECT_DECL();

    CAR_INTERFACE_DECL()

    CDessertCaseView();

    CARAPI constructor(
        /* [in] */ IContext* context);

    CARAPI constructor(
        /* [in] */ IContext* context,
        /* [in] */ IAttributeSet* attrs);

    CARAPI constructor(
        /* [in] */ IContext* context,
        /* [in] */ IAttributeSet* attrs,
        /* [in] */ Int32 defStyle);

    CARAPI Start();

    CARAPI Stop();

    Int32 Pick(
        /* [in] */ ArrayOf<Int32>* a);

    AutoPtr<IInterface> Pick(
        /* [in] */ ArrayOf<IInterface*>* a);

    AutoPtr<IInterface> Pick(
        /* [in] */ ISparseArray* sa);

    Int32 Random_color();

    CARAPI FillFreeList();

    CARAPI FillFreeList(
        /* [in] */ Int32 animationLen);

    CARAPI Place(
        /* [in] */ IView* v,
        /* [in] */ Boolean animate);

    CARAPI Place(
        /* [in] */ IView* v,
        /* [in] */ IPoint* pt,
        /* [in] */ Boolean animate);

    static Float Frand();

    static Float Frand(
        /* [in] */ Float a,
        /* [in] */ Float b);

    static Int32 Irand(
        /* [in] */ Int32 a,
        /* [in] */ Int32 b);

    void OnDraw(
        /* [in] */ ICanvas* c);

protected:
    void OnSizeChanged (
        /* [in] */ Int32 w,
        /* [in] */ Int32 h,
        /* [in] */ Int32 oldw,
        /* [in] */ Int32 oldh);

private:
    static AutoPtr<IBitmap> ConvertToAlphaMask(
        /* [in] */ IBitmap* b);

    // we don't have .withLayer() on general Animators
    AutoPtr<IAnimatorListener> MakeHardwareLayerListener(
        /* [in] */ IView* v);

    AutoPtr<ArrayOf<IPoint*> > GetOccupied(
        /* [in] */ IView* v);

    static CARAPI_(Boolean) InitStatic();

public:
    static Int32 START_DELAY;
    static Int32 DELAY;
    static Int32 DURATION;

    static Float SCALE; // natural display size will be SCALE*mCellSize

    AutoPtr<ArrayOf<Float> > mHsv;

private:
    static String TAG;
    static Boolean DEBUG;

    static Int32 TAG_POS;
    static Int32 TAG_SPAN;

    static AutoPtr<ArrayOf<Int32> > PASTRIES;

    static AutoPtr<ArrayOf<Int32> > RARE_PASTRIES;

    static AutoPtr<ArrayOf<Int32> > XRARE_PASTRIES;
    static AutoPtr<ArrayOf<Int32> > XXRARE_PASTRIES;

    static Int32 NUM_PASTRIES;

    AutoPtr<ISparseArray> mDrawables;

    static AutoPtr<ArrayOf<Float> > MASK;

    static AutoPtr<ArrayOf<Float> > ALPHA_MASK;

    static AutoPtr<ArrayOf<Float> > WHITE_MASK;

    static Float PROB_2X;
    static Float PROB_3X;
    static Float PROB_4X;
    static Boolean sInit;

    Boolean mStarted;

    Int32 mCellSize;
    Int32 mWidth, mHeight;
    Int32 mRows, mColumns;
    AutoPtr<ArrayOf<IView*> > mCells;

    AutoPtr<ISet> mFreeList;

    AutoPtr<IHandler> mHandler;

    AutoPtr<IRunnable> mJuggle;

    AutoPtr<IHashSet> mTmpSet;
};

} // namespace SystemUI
} // namespace Packages
} // namespace Droid
} // namespace Elastos

#endif // __ELASTOS_DROID_PACKAGES_SYSTEMUI_CDESSERTCASEVIEW_H__
