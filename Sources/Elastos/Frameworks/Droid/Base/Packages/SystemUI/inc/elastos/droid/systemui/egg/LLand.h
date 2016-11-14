
#ifndef __ELASTOS_DROID_SYSTEMUI_EGG_LLAND_H__
#define __ELASTOS_DROID_SYSTEMUI_EGG_LLAND_H__

#include "_Elastos.Droid.SystemUI.h"
#include "Elastos.Droid.Animation.h"
#include "Elastos.Droid.Content.h"
#include "Elastos.Droid.Graphics.h"
#include "Elastos.Droid.Utility.h"
#include "Elastos.Droid.View.h"
#include "Elastos.Droid.Widget.h"
#include "Elastos.CoreLibrary.Utility.h"
#include <elastos/core/Runnable.h>
#include "elastos/droid/widget/FrameLayout.h"
#include "elastos/droid/widget/ImageView.h"
#include "elastos/droid/view/ViewOutlineProvider.h"
#include "elastos/droid/view/View.h"

using Elastos::Droid::Animation::ITimeAnimator;
using Elastos::Droid::Animation::ITimeListener;
using Elastos::Droid::Content::IContext;
using Elastos::Droid::Content::Res::IResources;
using Elastos::Droid::Graphics::ICanvas;
using Elastos::Droid::Graphics::IPaint;
using Elastos::Droid::Graphics::IOutline;
using Elastos::Droid::Graphics::IPath;
using Elastos::Droid::Graphics::IRect;
using Elastos::Core::Runnable;
using Elastos::Droid::SystemUI::Egg::IGameView;
using Elastos::Droid::Utility::IAttributeSet;
using Elastos::Droid::View::IKeyEvent;
using Elastos::Droid::View::IMotionEvent;
using Elastos::Droid::View::IView;
using Elastos::Droid::View::View;
using Elastos::Droid::View::ViewOutlineProvider;
using Elastos::Droid::Widget::FrameLayout;
using Elastos::Droid::Widget::ImageView;
using Elastos::Droid::Widget::ITextView;
using Elastos::Utility::IArrayList;

namespace Elastos {
namespace Droid {
namespace SystemUI {
namespace Egg {

class LLand
    : public FrameLayout
    , public ILLand
{
private:
    class Params
        : public Object
    {
    public:
        Params(
            /* [in] */ IResources* res);

    public:
        Float TRANSLATION_PER_SEC;
        Int32 OBSTACLE_SPACING, OBSTACLE_PERIOD;
        Int32 BOOST_DV;
        Int32 PLAYER_HIT_SIZE;
        Int32 PLAYER_SIZE;
        Int32 OBSTACLE_WIDTH, OBSTACLE_STEM_WIDTH;
        Int32 OBSTACLE_GAP;
        Int32 OBSTACLE_MIN;
        Int32 BUILDING_WIDTH_MIN, BUILDING_WIDTH_MAX;
        Int32 BUILDING_HEIGHT_MIN;
        Int32 CLOUD_SIZE_MIN, CLOUD_SIZE_MAX;
        Int32 STAR_SIZE_MIN, STAR_SIZE_MAX;
        Int32 G;
        Int32 MAX_V;
        Float SCENERY_Z, OBSTACLE_Z, PLAYER_Z, PLAYER_Z_BOOST, HUD_Z;
    };

    class ViewOutlineProvider_Player
        : public ViewOutlineProvider
    {
    public:
        CARAPI GetOutline(
            /* [in] */ IView* view,
            /* [in] */ IOutline* outline);
    };

    class Player
        : public ImageView
        , public IGameView
    {
        friend class LLand;
    public:
        CAR_INTERFACE_DECL()

        Player(
            /* [in] */ IContext* context);

        CARAPI PrepareCheckIntersections();

        CARAPI Below(
            /* [in] */ Int32 h,
            /* [out] */ Boolean* result);

        CARAPI Step(
            /* [in] */ Int64 t_ms,
            /* [in] */ Int64 dt_ms,
            /* [in] */ Float t,
            /* [in] */ Float dt);

        CARAPI Boost();

        CARAPI Unboost();

    public:
        Float dv;

        Boolean mBoosting;

        AutoPtr<ArrayOf<Float> > corners;

    private:
        AutoPtr<ArrayOf<Float> > sHull;
    };

    class Obstacle
        : public View
        , public IGameView
        , public IObstacle
    {
        friend class LLand;
    public:
        CAR_INTERFACE_DECL()

        Obstacle(
            /* [in] */ IContext* context,
            /* [in] */ Float h);

         CARAPI Intersects(
            /* [in] */ Player* p,
            /* [out] */ Boolean* result);

        CARAPI Cleared(
            /* [in] */ Player* p,
            /* [out] */ Boolean* result);

        // @Override
        CARAPI Step(
            /* [in] */ Int64 t_ms,
            /* [in] */ Int64 dt_ms,
            /* [in] */ Float t,
            /* [in] */ Float dt);

    public:
        Float h;

        AutoPtr<IRect> hitRect;

        Int32 type;
    };

    class ViewOutlineProvider_Pop
        : public ViewOutlineProvider
    {
    public:
        ViewOutlineProvider_Pop(
            /* [in] */ LLand* host);

        CARAPI GetOutline(
            /* [in] */ IView* view,
            /* [in] */ IOutline* outline);

    private:
        LLand* mHost;
    };

    class Pop
        : public Obstacle
        , public IPop
    {
    public:
        CAR_INTERFACE_DECL()

        Pop(
            /* [in] */ IContext* context,
            /* [in] */ Float h,
            /* [in] */ LLand* host);

        CARAPI Intersects(
            /* [in] */ Player* p,
            /* [out] */ Boolean* result);

        // @Override
        CARAPI Step(
            /* [in] */ Int64 t_ms,
            /* [in] */ Int64 dt_ms,
            /* [in] */ Float t,
            /* [in] */ Float dt);

    public:
        Int32 mRotate;
        Int32 cx, cy, r;

    private:
        LLand* mHost;
    };

    class ViewOutlineProvider_Stem
        : public ViewOutlineProvider
    {
    public:
        ViewOutlineProvider_Stem(
            /* [in] */ LLand* host);

        CARAPI GetOutline(
            /* [in] */ IView* view,
            /* [in] */ IOutline* outline);

    private:
        LLand* mHost;
    };

    class Stem
        : public Obstacle
    {
    public:
        Stem(
            /* [in] */ IContext* context,
            /* [in] */ Float h,
            /* [in] */ Boolean drawShadow,
            /* [in] */ LLand* host);

        // @Override
        CARAPI OnAttachedToWindow();

        // @Override
        CARAPI_(void) OnDraw(
            /* [in] */ ICanvas* c);

    public:
        AutoPtr<IPaint> mPaint;
        AutoPtr<IPath> mShadow;
        Boolean mDrawShadow;

    private:
        LLand* mHost;
    };

    class Scenery
        : public FrameLayout
        , public IGameView
        , public IScenery
    {
        friend class LLand;
    public:
        CAR_INTERFACE_DECL()

        Scenery(
            /* [in] */ IContext* context);

        // @Override
        CARAPI Step(
            /* [in] */ Int64 t_ms,
            /* [in] */ Int64 dt_ms,
            /* [in] */ Float t,
            /* [in] */ Float dt);

    public:
        Float z;
        Float v;
        Int32 h, w;
        Int32 type;
    };

    class Building
        : public Scenery
    {
    public:
        Building(
            /* [in] */ IContext* context);
    };

    class Cloud
        : public Scenery
    {
    public:
        Cloud(
            /* [in] */ IContext* context);
    };

    class Star
        : public Scenery
    {
    public:
        Star(
            /* [in] */ IContext* context);
    };

    class LLand_Reset_TimeListener
        : public Object
        , public ITimeListener
    {
    public:
        CAR_INTERFACE_DECL()

        LLand_Reset_TimeListener(
            /* [in] */ LLand* host);

        CARAPI OnTimeUpdate(
            /* [in] */ ITimeAnimator* animation,
            /* [in] */ Int64 totalTime,
            /* [in] */ Int64 deltaTime);

    private:
        LLand* mHost;
    };

    class LLand_Stop_Runnable
        : public Runnable
    {
    public:
        LLand_Stop_Runnable(
            /* [in] */ LLand* host);

        CARAPI Run();

    private:
        LLand* mHost;
    };

    friend class LLand_Reset_TimeListener;
    friend class LLand_Stop_Runnable;
public:
    CAR_INTERFACE_DECL()

    static CARAPI_(void) L(
        /* [in] */ const String& s,
        /* [in] */ ArrayOf<IInterface* >* objects);

    LLand();

    CARAPI constructor(
        /* [in] */ IContext* context);

    CARAPI constructor(
        /* [in] */ IContext* context,
        /* [in] */ IAttributeSet* attrs);

    CARAPI constructor(
        /* [in] */ IContext* context,
        /* [in] */ IAttributeSet* attrs,
        /* [in] */ Int32 defStyle);

    // @Override
    CARAPI WillNotDraw(
        /* [out] */ Boolean* result);

    CARAPI GetGameWidth(
        /* [out] */ Int32* width);

    CARAPI GetGameHeight(
        /* [out] */ Int32* height);

    CARAPI GetGameTime(
        /* [out] */ Float* time);

    CARAPI GetLastTimeStep(
        /* [out] */ Float* ts);

    CARAPI SetScoreField(
        /* [in] */ ITextView* tv);

    CARAPI SetSplash(
        /* [in] */ IView* v);

    static CARAPI Lerp(
        /* [in] */ Float x,
        /* [in] */ Float a,
        /* [in] */ Float b,
        /* [out] */ Float* f);

    static CARAPI Rlerp(
        /* [in] */ Float v,
        /* [in] */ Float a,
        /* [in] */ Float b,
        /* [out] */ Float* f);

    static CARAPI Clamp(
        /* [in] */ Float f,
        /* [out] */ Float* res);

    static CARAPI Frand(
        /* [out] */ Float* f);

    static CARAPI Frand(
        /* [in] */ Float a,
        /* [in] */ Float b,
        /* [out] */ Float* f);

    static CARAPI Irand(
        /* [in] */ Int32 a,
        /* [in] */ Int32 b,
        /* [out] */ Int32* f);

    // @Override
    CARAPI OnTouchEvent(
        /* [in] */ IMotionEvent* ev,
        /* [out] */ Boolean* result);

    // @Override
    CARAPI OnTrackballEvent(
        /* [in] */ IMotionEvent* ev,
        /* [out] */ Boolean* result);

    // @Override
    CARAPI OnKeyDown(
        /* [in] */ Int32 keyCode,
        /* [in] */ IKeyEvent* ev,
        /* [out] */ Boolean* result);

    // @Override
    CARAPI OnKeyUp(
        /* [in] */ Int32 keyCode,
        /* [in] */ IKeyEvent* ev,
        /* [out] */ Boolean* result);

    // @Override
    CARAPI OnGenericMotionEvent(
        /* [in] */ IMotionEvent* ev,
        /* [out] */ Boolean* result);

    // @Override
    CARAPI_(void) OnDraw(
        /* [in] */ ICanvas* c);

protected:
    // @Override
    CARAPI OnSizeChanged(
        /* [in] */ Int32 w,
        /* [in] */ Int32 h,
        /* [in] */ Int32 oldw,
        /* [in] */ Int32 oldh);

private:
    CARAPI_(void) Reset();

    CARAPI_(void) SetScore(
        /* [in] */ Int32 score);

    CARAPI_(void) AddScore(
        /* [in] */ Int32 incr);

    CARAPI_(void) Start(
        /* [in] */ Boolean startPlaying);

    CARAPI_(void) Stop();

    CARAPI_(void) Step(
        /* [in] */ Int64 t_ms,
        /* [in] */ Int64 dt_ms);

    CARAPI_(void) Poke();

    CARAPI_(void) Unpoke();

public:
    const static String TAG;

    const static Boolean DEBUG;

    const static AutoPtr<ArrayOf<Int32> > POPS;
    AutoPtr<ArrayOf<Float> > hsv;
    const static AutoPtr<IRect> sTmpRect;

private:
    AutoPtr<ITimeAnimator> mAnim;

    AutoPtr<ITextView> mScoreField;
    AutoPtr<IView> mSplash;

    AutoPtr<Player> mDroid;

    AutoPtr<IArrayList> mObstaclesInPlay;

    Float t, dt;

    Int32 mScore;
    Float mLastPipeTime; // in sec
    Int32 mWidth, mHeight;
    Boolean mAnimating, mPlaying;
    Boolean mFrozen; // after death, a short backoff
    Boolean mFlipped;

    Int32 mTimeOfDay;
    const static Int32 DAY = 0;
    const static Int32 NIGHT = 1;
    const static Int32 TWILIGHT = 2;
    const static Int32 SUNSET = 3;
    static AutoPtr<ArrayOf<AutoPtr<ArrayOf<Int32> > > > SKIES;
    static AutoPtr<Params> PARAMS;
};

} // namespace Egg
} // namespace SystemUI
} // namespace Droid
} // namespace Elastos

#endif //__ELASTOS_DROID_SYSTEMUI_EGG_LLAND_H__