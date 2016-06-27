
#include "elastos/droid/systemui/egg/LLand.h"
#include "Elastos.Droid.Widget.h"
#include "Elastos.CoreLibrary.Core.h"
#include "R.h"
#include <elastos/core/Math.h>
#include <elastos/core/StringBuilder.h>
#include <elastos/utility/logging/Logger.h>

using Elastos::Droid::Animation::CTimeAnimator;
using Elastos::Droid::Animation::EIID_ITimeListener;
using Elastos::Droid::Animation::IAnimator;
using Elastos::Droid::Animation::ITimeInterpolator;
using Elastos::Droid::Graphics::CColor;
using Elastos::Droid::Graphics::CPaint;
using Elastos::Droid::Graphics::CPath;
using Elastos::Droid::Graphics::CRect;
using Elastos::Droid::Graphics::IColor;
using Elastos::Droid::Graphics::IMatrix;
using Elastos::Droid::Graphics::PaintStyle_STROKE;
using Elastos::Droid::Graphics::PorterDuffMode_SRC_ATOP;
using Elastos::Droid::Graphics::Drawable::CGradientDrawable;
using Elastos::Droid::Graphics::Drawable::GradientDrawableOrientation_BOTTOM_TOP;
using Elastos::Droid::Graphics::Drawable::GradientDrawableOrientation_LEFT_RIGHT;
using Elastos::Droid::Graphics::Drawable::IDrawable;
using Elastos::Droid::Graphics::Drawable::IGradientDrawable;
using Elastos::Droid::Utility::IDisplayMetrics;
using Elastos::Droid::View::IGravity;
using Elastos::Droid::View::IViewGroup;
using Elastos::Droid::View::IViewGroupMarginLayoutParams;
using Elastos::Droid::View::IViewOutlineProvider;
using Elastos::Droid::View::IViewPropertyAnimator;
using Elastos::Droid::View::Animation::CDecelerateInterpolator;
using Elastos::Droid::Widget::CFrameLayoutLayoutParams;
using Elastos::Droid::Widget::IFrameLayoutLayoutParams;
using Elastos::Core::CInteger32;
using Elastos::Core::CString;
using Elastos::Core::ICharSequence;
using Elastos::Core::IInteger32;
using Elastos::Core::IRunnable;
using Elastos::Core::Math;
using Elastos::Core::StringBuilder;
using Elastos::Utility::IIterable;
using Elastos::Utility::IIterator;
using Elastos::Utility::Logging::Logger;

namespace Elastos {
namespace Droid {
namespace SystemUI {
namespace Egg {

//=====================
// LLand::Params
//=====================

LLand::Params::Params(
    /* [in] */ IResources* res)
{
    res->GetDimension(R::dimen::translation_per_sec, &TRANSLATION_PER_SEC);
    res->GetDimensionPixelSize(R::dimen::obstacle_spacing, &OBSTACLE_SPACING);
    OBSTACLE_PERIOD = (Int32) (OBSTACLE_SPACING / TRANSLATION_PER_SEC);
    res->GetDimensionPixelSize(R::dimen::boost_dv, &BOOST_DV);
    res->GetDimensionPixelSize(R::dimen::player_hit_size, &PLAYER_HIT_SIZE);
    res->GetDimensionPixelSize(R::dimen::player_size, &PLAYER_SIZE);
    res->GetDimensionPixelSize(R::dimen::obstacle_width, &OBSTACLE_WIDTH);
    res->GetDimensionPixelSize(R::dimen::obstacle_stem_width, &OBSTACLE_STEM_WIDTH);
    res->GetDimensionPixelSize(R::dimen::obstacle_gap, &OBSTACLE_GAP);
    res->GetDimensionPixelSize(R::dimen::obstacle_height_min, &OBSTACLE_MIN);
    res->GetDimensionPixelSize(R::dimen::building_height_min, &BUILDING_HEIGHT_MIN);
    res->GetDimensionPixelSize(R::dimen::building_width_min, &BUILDING_WIDTH_MIN);
    res->GetDimensionPixelSize(R::dimen::building_width_max, &BUILDING_WIDTH_MAX);
    res->GetDimensionPixelSize(R::dimen::cloud_size_min, &CLOUD_SIZE_MIN);
    res->GetDimensionPixelSize(R::dimen::cloud_size_max, &CLOUD_SIZE_MAX);
    res->GetDimensionPixelSize(R::dimen::star_size_min, &STAR_SIZE_MIN);
    res->GetDimensionPixelSize(R::dimen::star_size_max, &STAR_SIZE_MAX);

    res->GetDimensionPixelSize(R::dimen::G, &G);
    res->GetDimensionPixelSize(R::dimen::max_v, &MAX_V);

    Int32 i1;
    res->GetDimensionPixelSize(R::dimen::scenery_z, &i1);
    SCENERY_Z = i1;
    res->GetDimensionPixelSize(R::dimen::obstacle_z, &i1);
    OBSTACLE_Z = i1;
    res->GetDimensionPixelSize(R::dimen::player_z, &i1);
    PLAYER_Z = i1;
    res->GetDimensionPixelSize(R::dimen::player_z_boost, &i1);
    PLAYER_Z_BOOST = i1;
    res->GetDimensionPixelSize(R::dimen::hud_z, &i1);
    HUD_Z = i1;
}

//=====================
// LLand::Player
//=====================

static AutoPtr<ArrayOf<Float> > InitSHull()
{
    AutoPtr<ArrayOf<Float> > shull = ArrayOf<Float>::Alloc(16);
    (*shull)[0] = 0.3f;
    (*shull)[1] = 0.0f;
    (*shull)[2] = 0.7f;
    (*shull)[3] = 0.0f;
    (*shull)[4] = 0.92f;
    (*shull)[5] = 0.33f;
    (*shull)[6] = 0.92f;
    (*shull)[7] = 0.75f;
    (*shull)[8] = 0.6f;
    (*shull)[9] = 1.0f;
    (*shull)[10] = 0.4f;
    (*shull)[11] = 1.0f;
    (*shull)[12] = 0.08f;
    (*shull)[13] = 0.75f;
    (*shull)[14] = 0.08f;
    (*shull)[15] = 0.33f;
    return shull;
}

CAR_INTERFACE_IMPL(LLand::Player, ImageView, IGameView)

LLand::Player::Player(
    /* [in] */ IContext* context)
{
    ImageView::constructor(context);

    SetBackgroundResource(R::drawable::android);
    AutoPtr<IDrawable> drawable;
    GetBackground((IDrawable**)&drawable);
    drawable->SetTintMode(PorterDuffMode_SRC_ATOP);
    drawable->SetTint(0xFF00FF00);
    AutoPtr<ViewOutlineProvider_Player> provider = new ViewOutlineProvider_Player();
    SetOutlineProvider((IViewOutlineProvider*)provider);
    sHull = InitSHull();
    corners = ArrayOf<Float>::Alloc(sHull->GetLength());
}

ECode LLand::Player::PrepareCheckIntersections()
{
    const Int32 inset = (LLand::PARAMS->PLAYER_SIZE - LLand::PARAMS->PLAYER_HIT_SIZE)/2;
    const Int32 scale = LLand::PARAMS->PLAYER_HIT_SIZE;
    const Int32 N = sHull->GetLength() / 2;
    for (Int32 i = 0; i < N; i++) {
        (*corners)[i*2]   = scale * (*sHull)[i * 2]   + inset;
        (*corners)[i*2+1] = scale * (*sHull)[i * 2 + 1] + inset;
    }
    AutoPtr<IMatrix> m;
    GetMatrix((IMatrix**)&m);
    m->MapPoints(corners);
    return NOERROR;
}

ECode LLand::Player::Below(
    /* [in] */ Int32 h,
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(result)
    const Int32 N = corners->GetLength() / 2;
    for (Int32 i = 0; i < N; i++) {
        const Int32 y = (Int32)(*corners)[i*2+1];
        if (y >= h) {
            *result = TRUE;
            return NOERROR;
        }
    }
    *result = FALSE;
    return NOERROR;
}

ECode LLand::Player::Step(
    /* [in] */ Int64 t_ms,
    /* [in] */ Int64 dt_ms,
    /* [in] */ Float t,
    /* [in] */ Float dt)
{
    Int32 visibility;
    GetVisibility(&visibility);
    if (visibility != IView::VISIBLE) return E_NULL_POINTER_EXCEPTION; // not playing yet

    if (mBoosting) {
        dv = -LLand::PARAMS->BOOST_DV;
    } else {
        dv += LLand::PARAMS->G;
    }
    if (dv < -LLand::PARAMS->MAX_V) dv = -LLand::PARAMS->MAX_V;
    else if (dv > LLand::PARAMS->MAX_V) dv = LLand::PARAMS->MAX_V;

    Float ty;
    GetTranslationY(&ty);
    const Float y = ty + dv * dt;
    SetTranslationY(y < 0 ? 0 : y);
    Float f, c, l;
    LLand::Rlerp(dv, LLand::PARAMS->MAX_V, -1 * LLand::PARAMS->MAX_V, &f);
    LLand::Clamp(f, &c);
    LLand::Lerp(c, 90, -90, &l);
    SetRotation(90 + l);

    PrepareCheckIntersections();
    return NOERROR;
}

ECode LLand::Player::Boost()
{
    mBoosting = TRUE;
    dv = -PARAMS->BOOST_DV;

    AutoPtr<IViewPropertyAnimator> animator;
    Animate((IViewPropertyAnimator**)&animator);
    animator->Cancel();
    animator->ScaleX(1.25f);
    animator->ScaleY(1.25f);
    animator->TranslationZ(LLand::PARAMS->PLAYER_Z_BOOST);
    animator->SetDuration(100);
    SetScaleX(1.25f);
    SetScaleY(1.25f);
    return NOERROR;
}

ECode LLand::Player::Unboost()
{
    mBoosting = FALSE;

    AutoPtr<IViewPropertyAnimator> animator;
    Animate((IViewPropertyAnimator**)&animator);
    animator->Cancel();
    animator->ScaleX(1.0f);
    animator->ScaleY(1.0f);
    animator->TranslationZ(PARAMS->PLAYER_Z);
    animator->SetDuration(200);
    return NOERROR;
}

//=====================
// LLand::ViewOutlineProvider_Player
//=====================

ECode LLand::ViewOutlineProvider_Player::GetOutline(
    /* [in] */ IView* view,
    /* [in] */ IOutline* outline)
{
    Int32 w;
    view->GetWidth(&w);
    Int32 h;
    view->GetHeight(&h);
    Int32 ix = (Int32) (w * 0.3f);
    Int32 iy = (Int32) (h * 0.2f);
    outline->SetRect(ix, iy, w - ix, h - iy);
    return NOERROR;
}

//=====================
// LLand::Obstacle
//=====================

CAR_INTERFACE_IMPL_2(LLand::Obstacle, View, IGameView, IObstacle)

LLand::Obstacle::Obstacle(
    /* [in] */ IContext* context,
    /* [in] */ Float h)
{
    View::constructor(context);;
    SetBackgroundColor(0xFFFF0000);
    this->h = h;
    CRect::New((IRect**)&hitRect);
    type = 0;
}

ECode LLand::Obstacle::Intersects(
    /* [in] */ Player* p,
    /* [out] */ Boolean* result)
 {
    const Int32 N = p->corners->GetLength() / 2;
    for (Int32 i = 0; i < N; i++) {
        const Int32 x = (Int32) (*(p->corners))[i*2];
        const Int32 y = (Int32) (*(p->corners))[i*2+1];
        Boolean con;
        hitRect->Contains(x, y, &con);
        if (con) {
            *result = TRUE;
            return NOERROR;
        }
    }
    *result = FALSE;
    return NOERROR;
}

ECode LLand::Obstacle::Cleared(
    /* [in] */ Player* p,
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(result)
    const Int32 N = p->corners->GetLength()/2;
    for (Int32 i = 0; i < N; i++) {
        const Int32 x = (Int32) (*(p->corners))[i*2];
        Int32 right;
        hitRect->GetRight(&right);
        if (right >= x) {
            *result = FALSE;
            return NOERROR;
        }
    }
    *result = TRUE;
    return NOERROR;
}

ECode LLand::Obstacle::Step(
    /* [in] */ Int64 t_ms,
    /* [in] */ Int64 dt_ms,
    /* [in] */ Float t,
    /* [in] */ Float dt)
{
    Float x;
    GetTranslationX(&x);
    SetTranslationX(x - PARAMS->TRANSLATION_PER_SEC * dt);
    GetHitRect(hitRect);
    return NOERROR;
}

//=====================
// LLand::ViewOutlineProvider_Pop
//=====================

LLand::ViewOutlineProvider_Pop::ViewOutlineProvider_Pop(
    /* [in] */ LLand* host)
    : mHost(host)
{}

ECode LLand::ViewOutlineProvider_Pop::GetOutline(
    /* [in] */ IView* view,
    /* [in] */ IOutline* outline)
{
    Int32 width, height;
    mHost->GetWidth(&width);
    mHost->GetHeight(&height);
    const Int32 pad = (Int32) (width * 0.02f);
    outline->SetOval(pad, pad, width - pad, height - pad);
    return NOERROR;
}

//=====================
// LLand::Pop
//=====================

CAR_INTERFACE_IMPL(LLand::Pop, Obstacle, IPop)

LLand::Pop::Pop(
    /* [in] */ IContext* context,
    /* [in] */ Float h,
    /* [in] */ LLand* host)
    : Obstacle(context, h)
    , mHost(host)
{
    Int32 i;
    LLand::Irand(0, LLand::POPS->GetLength() / 2, &i);
    Int32 idx = 2 * i;
    SetBackgroundResource((*(LLand::POPS))[idx]);
    Float f;
    LLand::Frand(&f);
    SetScaleX(f < 0.5f ? -1 : 1);
    mRotate = (*(LLand::POPS))[idx+1] == 0 ? 0 : (f < 0.5f ? -1 : 1);
    AutoPtr<ViewOutlineProvider_Pop> provider = new ViewOutlineProvider_Pop(mHost);
    SetOutlineProvider((IViewOutlineProvider*)provider);
}

ECode LLand::Pop::Intersects(
    /* [in] */ Player* p,
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(result)
    const Int32 N = (p->corners)->GetLength() / 2;
    for (Int32 i = 0; i < N; i++) {
        const Int32 x = (Int32) (*(p->corners))[i * 2];
        const Int32 y = (Int32) (*(p->corners))[i * 2 + 1];
        if (Elastos::Core::Math::Hypot(x - cx, y - cy) <= r) {
            *result = TRUE;
            return NOERROR;
        }
    }
    *result = FALSE;
    return NOERROR;
}

ECode LLand::Pop::Step(
    /* [in] */ Int64 t_ms,
    /* [in] */ Int64 dt_ms,
    /* [in] */ Float t,
    /* [in] */ Float dt)
{
    Obstacle::Step(t_ms, dt_ms, t, dt);
    if (mRotate != 0) {
        Float rotation;
        GetRotation(&rotation);
        SetRotation(rotation + dt * 45 * mRotate);
    }

    Int32 left, right, top, bottom;
    hitRect->GetLeft(&left);
    hitRect->GetRight(&right);
    hitRect->GetTop(&top);
    hitRect->GetBottom(&bottom);
    cx = (left + right)/2;
    cy = (top + bottom)/2;
    Int32 width;
    GetWidth(&width);
    r = width / 2;
    return NOERROR;
}

//=====================
// LLand::ViewOutlineProvider_Stem
//=====================

LLand::ViewOutlineProvider_Stem::ViewOutlineProvider_Stem(
    /* [in] */ LLand* host)
    : mHost(host)
{}

ECode LLand::ViewOutlineProvider_Stem::GetOutline(
    /* [in] */ IView* view,
    /* [in] */ IOutline* outline)
{
    Int32 width, height;
    mHost->GetWidth(&width);
    mHost->GetHeight(&height);
    outline->SetRect(0, 0, width, height);
    return NOERROR;
}

//=====================
// LLand::Stem
//=====================

LLand::Stem::Stem(
    /* [in] */ IContext* context,
    /* [in] */ Float h,
    /* [in] */ Boolean drawShadow,
    /* [in] */ LLand* host)
    : Obstacle(context, h)
    , mHost(host)
{
    mDrawShadow = drawShadow;
    CPaint::New((IPaint**)&mPaint);
    mPaint->SetColor(0xFFAAAAAA);
    SetBackground(NULL);
    CPath::New((IPath**)&mShadow);
    type = 1;
}

ECode LLand::Stem::OnAttachedToWindow()
{
    View::OnAttachedToWindow();
    SetWillNotDraw(FALSE);
    AutoPtr<ViewOutlineProvider_Stem> provider = new ViewOutlineProvider_Stem(mHost);
    SetOutlineProvider((IViewOutlineProvider*)provider);
    return NOERROR;
}

void LLand::Stem::OnDraw(
    /* [in] */ ICanvas* c)
{
    Int32 width, height;
    c->GetWidth(&width);
    c->GetHeight(&height);
    const Int32 w = width;
    const Int32 h = height;
    AutoPtr<IGradientDrawable> g;
    CGradientDrawable::New((IGradientDrawable**)&g);
    g->SetOrientation(GradientDrawableOrientation_LEFT_RIGHT);
    g->SetGradientCenter(w * 0.75f, 0);
    AutoPtr<ArrayOf<Int32> > colors = ArrayOf<Int32>::Alloc(2);
    (*colors)[0] = 0xFFFFFFFF;
    (*colors)[1] = 0xFFAAAAAA;
    g->SetColors(colors);
    IDrawable::Probe(g)->SetBounds(0, 0, w, h);
    IDrawable::Probe(g)->Draw(c);
    if (!mDrawShadow) return;
    mShadow->Reset();
    mShadow->MoveTo(0,0);
    mShadow->LineTo(w, 0);
    mShadow->LineTo(w, LLand::PARAMS->OBSTACLE_WIDTH / 2 + w * 1.5f);
    mShadow->LineTo(0, LLand::PARAMS->OBSTACLE_WIDTH / 2);
    mShadow->Close();
    c->DrawPath(mShadow, mPaint);
}

//=====================
// LLand::Scenery
//=====================

CAR_INTERFACE_IMPL_2(LLand::Scenery, FrameLayout, IGameView, IScenery)

LLand::Scenery::Scenery(
    /* [in] */ IContext* context)
{
    FrameLayout::constructor(context);
    type = 0;
}

ECode LLand::Scenery::Step(
    /* [in] */ Int64 t_ms,
    /* [in] */ Int64 dt_ms,
    /* [in] */ Float t,
    /* [in] */ Float dt)
{
    Float x;
    GetTranslationX(&x);
    SetTranslationX(x - LLand::PARAMS->TRANSLATION_PER_SEC * dt * v);
    return NOERROR;
}

//=====================
// LLand::Building
//=====================

LLand::Building::Building(
    /* [in] */ IContext* context)
    : Scenery(context)
{
    LLand::Irand(LLand::PARAMS->BUILDING_WIDTH_MIN, LLand::PARAMS->BUILDING_WIDTH_MAX, &(Scenery::w));
    Scenery::h = 0; // will be setup later, along with z
    SetTranslationZ(LLand::PARAMS->SCENERY_Z);
    type = 1;
}

//=====================
// LLand::Cloud
//=====================

LLand::Cloud::Cloud(
    /* [in] */ IContext* context)
    : Scenery(context)
{
    Float f;
    LLand::Frand(&f);
    SetBackgroundResource(f < 0.01f ? R::drawable::cloud_off : R::drawable::cloud);
    AutoPtr<IDrawable> drawable;
    GetBackground((IDrawable**)&drawable);
    drawable->SetAlpha(0x40);
    LLand::Irand(LLand::PARAMS->CLOUD_SIZE_MIN, LLand::PARAMS->CLOUD_SIZE_MAX, &(Scenery::h));
    Scenery::w = Scenery::h;
    Scenery::z = 0;
    LLand::Frand(0.15f, 0.5f, &(Scenery::v));
}

//=====================
// LLand::Star
//=====================

LLand::Star::Star(
    /* [in] */ IContext* context)
    : Scenery(context)
{
    SetBackgroundResource(R::drawable::star);
    LLand::Irand(LLand::PARAMS->STAR_SIZE_MIN, LLand::PARAMS->STAR_SIZE_MAX, &(Scenery::h));
    Scenery::w = Scenery::h;
    Scenery::v = Scenery::z = 0;
    type = 2;
}

//=====================
// LLand::LLand_Reset_TimeListener
//=====================

CAR_INTERFACE_IMPL(LLand::LLand_Reset_TimeListener, Object, ITimeListener)

LLand::LLand_Reset_TimeListener::LLand_Reset_TimeListener(
    /* [in] */ LLand* host)
    : mHost(host)
{}

ECode LLand::LLand_Reset_TimeListener::OnTimeUpdate(
    /* [in] */ ITimeAnimator* animation,
    /* [in] */ Int64 totalTime,
    /* [in] */ Int64 deltaTime)
{
    mHost->Step(mHost->t, mHost->dt);
    return NOERROR;
}

//=====================
// LLand::LLand_Stop_Runnable
//=====================

LLand::LLand_Stop_Runnable::LLand_Stop_Runnable(
    /* [in] */ LLand* host)
    : mHost(host)
{}

ECode LLand::LLand_Stop_Runnable::Run()
{
    mHost->mFrozen = FALSE;
    return NOERROR;
}

//=====================
// LLand
//=====================

static AutoPtr<ArrayOf<Int32> > InitPOPS()
{
    AutoPtr<ArrayOf<Int32> > pops = ArrayOf<Int32>::Alloc(14);
    (*pops)[0] = R::drawable::pop_belt;
    (*pops)[1] = 0;
    (*pops)[2] = R::drawable::pop_droid;
    (*pops)[3] = 0;
    (*pops)[4] = R::drawable::pop_pizza;
    (*pops)[5] = 1;
    (*pops)[6] = R::drawable::pop_stripes;
    (*pops)[7] = 0;
    (*pops)[8] = R::drawable::pop_swirl;
    (*pops)[9] = 1;
    (*pops)[10] = R::drawable::pop_vortex;
    (*pops)[11] = 1;
    (*pops)[12] = R::drawable::pop_vortex2;
    (*pops)[13] = 1;
    return pops;
}

static AutoPtr<ArrayOf<AutoPtr<ArrayOf<Int32> > > > InitSKIES()
{
    AutoPtr<ArrayOf<AutoPtr<ArrayOf<Int32> > > > skies = ArrayOf<AutoPtr<ArrayOf<Int32> > >::Alloc(4);
    for (Int32 i = 0; i < skies->GetLength(); i++) {
        (*skies)[i] = ArrayOf<Int32>::Alloc(2);
    }
    (*(*skies)[0])[0] = 0xFFc0c0FF;
    (*(*skies)[0])[1] = 0xFFa0a0FF; // DAY
    (*(*skies)[1])[0] = 0xFF000010;
    (*(*skies)[1])[1] = 0xFF000000; // NIGHT
    (*(*skies)[2])[0] = 0xFF000040;
    (*(*skies)[2])[1] = 0xFF000010; // TWILIGHT
    (*(*skies)[3])[0] = 0xFFa08020;
    (*(*skies)[3])[1] = 0xFF204080; // SUNSET
    return skies;
}

const String LLand::TAG("LLand");

const Boolean LLand::DEBUG = Logger::IsLoggable(TAG, Logger::___DEBUG);

const AutoPtr<ArrayOf<Int32> > LLand::POPS = InitPOPS();

const Int32 LLand::DAY;
const Int32 LLand::NIGHT;
const Int32 LLand::TWILIGHT;
const Int32 LLand::SUNSET;
AutoPtr<ArrayOf<AutoPtr<ArrayOf<Int32> > > > LLand::SKIES = InitSKIES();
AutoPtr<LLand::Params> LLand::PARAMS;

CAR_INTERFACE_IMPL(LLand, FrameLayout, ILLand)

void LLand::L(
    /* [in] */ const String& s,
    /* [in] */ ArrayOf<IInterface* >* objects)
{
    if (DEBUG) {
        // Logger::D(TAG, String.format(s, objects));
    }
}

LLand::LLand()
{
    hsv = ArrayOf<Float>::Alloc(3);
    (*hsv)[0] = 0;
    (*hsv)[1] = 0;
    (*hsv)[2] = 0;
}

ECode LLand::constructor(
    /* [in] */ IContext* context)
{
    return constructor(context, NULL);
}

ECode LLand::constructor(
    /* [in] */ IContext* context,
    /* [in] */ IAttributeSet* attrs)
{
    return constructor(context, attrs, 0);
}

ECode LLand::constructor(
    /* [in] */ IContext* context,
    /* [in] */ IAttributeSet* attrs,
    /* [in] */ Int32 defStyle)
{
    FrameLayout::constructor(context, attrs, defStyle);
    SetFocusable(TRUE);
    AutoPtr<IResources> resources;
    GetResources((IResources**)&resources);
    PARAMS = new Params(resources);
    Irand(0, SKIES->GetLength(), &mTimeOfDay);
    return NOERROR;
}

ECode LLand::WillNotDraw(
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(result)
    *result = !DEBUG;
    return NOERROR;
}

ECode LLand::GetGameWidth(
    /* [out] */ Int32* width)
{
    VALIDATE_NOT_NULL(width)
    *width = mWidth;
    return NOERROR;
}

ECode LLand::GetGameHeight(
    /* [out] */ Int32* height)
{
    VALIDATE_NOT_NULL(height)
    *height = mHeight;
    return NOERROR;
}

ECode LLand::GetGameTime(
    /* [out] */ Float* time)
{
    VALIDATE_NOT_NULL(time)
    *time = t;
    return NOERROR;
}

ECode LLand::GetLastTimeStep(
    /* [out] */ Float* ts)
{
    VALIDATE_NOT_NULL(ts)
    *ts = dt;
    return NOERROR;
}

ECode LLand::SetScoreField(
    /* [in] */ ITextView* tv)
{
    mScoreField = tv;
    if (tv != NULL) {
        IView::Probe(tv)->SetTranslationZ(PARAMS->HUD_Z);
        if (!(mAnimating && mPlaying)) {
            IView::Probe(tv)->SetTranslationY(-500);
        }
    }
    return NOERROR;
}

ECode LLand::SetSplash(
    /* [in] */ IView* v)
{
    mSplash = v;
    return NOERROR;
}

void LLand::OnSizeChanged(
    /* [in] */ Int32 w,
    /* [in] */ Int32 h,
    /* [in] */ Int32 oldw,
    /* [in] */ Int32 oldh)
{
    Stop();
    Reset();
    if (AUTOSTART) {
        Start(FALSE);
    }
}

void LLand::Reset()
{
    L(String("reset"), NULL);
    AutoPtr<IGradientDrawable> drawable;
    CGradientDrawable::New(
            GradientDrawableOrientation_BOTTOM_TOP,
            (*SKIES)[mTimeOfDay],
            (IGradientDrawable**)&drawable
    );
    const AutoPtr<IDrawable> sky = IDrawable::Probe(drawable);
    sky->SetDither(TRUE);
    SetBackground(sky);

    Float f;
    Frand(&f);
    mFlipped = f > 0.5f;
    SetScaleX(mFlipped ? -1 : 1);

    SetScore(0);

    Int32 i;
    GetChildCount(&i);
    while (i-- > 0) {
        AutoPtr<IView> child;
        GetChildAt(i, (IView**)&child);
        const AutoPtr<IView> v = child;
        if (IGameView::Probe(v) != NULL) {
            RemoveViewAt(i);
        }
    }

    mObstaclesInPlay->Clear();

    GetWidth(&mWidth);
    GetHeight(&mHeight);

    Boolean showingSun = (mTimeOfDay == DAY || mTimeOfDay == SUNSET) && f > 0.25;
    if (showingSun) {
        AutoPtr<IContext> ctx;
        GetContext((IContext**)&ctx);
        AutoPtr<Star> sun = new Star(ctx);
        sun->SetBackgroundResource(R::drawable::sun);
        AutoPtr<IResources> res;
        GetResources((IResources**)&res);
        Int32 size;
        res->GetDimensionPixelSize(R::dimen::sun_size, &size);
        const Int32 w = size;
        Float f2;
        Frand(w, mWidth - w, &f2);
        sun->SetTranslationX(f2);
        if (mTimeOfDay == DAY) {
            Float f3;
            Frand(w, (mHeight * 0.66f), &f3);
            sun->SetTranslationY(f3);
            AutoPtr<IDrawable> drawable;
            sun->GetBackground((IDrawable**)&drawable);
            drawable->SetTint(0);
        }
        else {
            Float f4;
            Frand(mHeight * 0.66f, mHeight - w, &f4);
            sun->SetTranslationY(f4);
            AutoPtr<IDrawable> drawable;
            sun->GetBackground((IDrawable**)&drawable);
            drawable->SetTintMode(PorterDuffMode_SRC_ATOP);
            drawable->SetTint(0xC0FF8000);

        }
        AutoPtr<IFrameLayoutLayoutParams> lp;
        CFrameLayoutLayoutParams::New(w, w, (IFrameLayoutLayoutParams**)&lp);
        AddView(sun, IViewGroupLayoutParams::Probe(lp));
    }
    if (!showingSun) {
        const Boolean dark = mTimeOfDay == NIGHT || mTimeOfDay == TWILIGHT;
        const Float ff = f;
        if ((dark && ff < 0.75f) || ff < 0.5f) {
            AutoPtr<IContext> ctx;
            GetContext((IContext**)&ctx);
            const AutoPtr<Star> moon = new Star(ctx);
            moon->SetBackgroundResource(R::drawable::moon);
            AutoPtr<IDrawable> drawable;
            moon->GetBackground((IDrawable**)&drawable);
            drawable->SetAlpha(dark ? 255 : 128);
            moon->SetScaleX(f > 0.5 ? -1 : 1);
            Float scalex;
            moon->GetScaleX(&scalex);
            Float f5;
            Frand(5, 30, &f5);
            moon->SetRotation(scalex * f5);
            AutoPtr<IResources> res;
            GetResources((IResources**)&res);
            Int32 size;
            res->GetDimensionPixelSize(R::dimen::sun_size, &size);
            const Int32 w = size;
            Float f6,f7;
            Frand(w, mWidth - w, &f6);
            Frand(w, mHeight - w, &f7);
            moon->SetTranslationX(f6);
            moon->SetTranslationY(f7);
            AutoPtr<IFrameLayoutLayoutParams> lp;
            CFrameLayoutLayoutParams::New(w, w, (IFrameLayoutLayoutParams**)&lp);
            AddView(moon, IViewGroupLayoutParams::Probe(lp));
        }
    }

    const Int32 mh = mHeight / 6;
    const Boolean cloudless = f < 0.25;
    const Int32 N = 20;
    for (i = 0; i < N; i++) {
        const Float r1 = f;
        AutoPtr<Scenery> s;
        AutoPtr<IContext> ctx;
        GetContext((IContext**)&ctx);
        if (HAVE_STARS && r1 < 0.3 && mTimeOfDay != DAY) {
            s = new Star(ctx);
        }
        else if (r1 < 0.6 && !cloudless) {
            s = new Cloud(ctx);
        }
        else {
            s = new Building(ctx);

            s->z = (Float)i/N;
            s->SetTranslationZ(PARAMS->SCENERY_Z * (1 + s->z));
            s->v = 0.85f * s->z; // buildings move proportional to their distance
            (*hsv)[0] = 175;
            (*hsv)[1] = 0.25f;
            (*hsv)[2] = 1 * s->z;
            AutoPtr<IColor> color;
            CColor::AcquireSingleton((IColor**)&color);
            Int32 cl;
            color->HSVToColor(hsv, &cl);
            s->SetBackgroundColor(cl);
            Irand(PARAMS->BUILDING_HEIGHT_MIN, mh, &(s->h));
        }
        const AutoPtr<IFrameLayoutLayoutParams> lp;
        CFrameLayoutLayoutParams::New(s->w, s->h, (IFrameLayoutLayoutParams**)&lp);

        if (s->type == 1) {
            lp->SetGravity(IGravity::BOTTOM);
        }
        else {
            lp->SetGravity(IGravity::TOP);
            Float r1;
            Frand(&r1);
            const Float r = r1;
            if (s->type == 2) {
                IViewGroupMarginLayoutParams::Probe(lp)->SetTopMargin((Int32) (r * r * mHeight));
            } else {
                IViewGroupMarginLayoutParams::Probe(lp)->SetTopMargin((Int32) (1 - r*r * mHeight/2) + mHeight/2);
            }
        }
        AddView(s, IViewGroupLayoutParams::Probe(lp));
        Int32 lpwidth;
        IViewGroupLayoutParams::Probe(lp)->GetWidth(&lpwidth);
        Float f8;
        Frand(-lpwidth, mWidth + lpwidth, &f8);
        s->SetTranslationX(f8);
    }

    AutoPtr<IContext> ctx;
    GetContext((IContext**)&ctx);
    mDroid = new Player(ctx);
    mDroid->SetX(mWidth / 2);
    mDroid->SetY(mHeight / 2);
    const AutoPtr<IFrameLayoutLayoutParams> lp;
    CFrameLayoutLayoutParams::New(PARAMS->PLAYER_SIZE, PARAMS->PLAYER_SIZE,
        (IFrameLayoutLayoutParams**)&lp);
    AddView((IView*)mDroid, IViewGroupLayoutParams::Probe(lp));

    CTimeAnimator::New((ITimeAnimator**)&mAnim);
    AutoPtr<LLand_Reset_TimeListener> tl = new LLand_Reset_TimeListener(this);
    mAnim->SetTimeListener((ITimeListener*)tl.Get());
}

void LLand::SetScore(
    /* [in] */ Int32 score)
{
    mScore = score;
    if (mScoreField != NULL) mScoreField->SetText(score);
}

void LLand::AddScore(
    /* [in] */ Int32 incr)
{
    SetScore(mScore + incr);
}

void LLand::Start(
    /* [in] */ Boolean startPlaying)
{
    AutoPtr<ICharSequence> cs;
    CString::New(startPlaying ? String("true") : String("false"), (ICharSequence**)&cs);
    AutoPtr<ArrayOf<IInterface* > > objects= ArrayOf<IInterface* >::Alloc(1);
    objects->Set(0, cs);
    L(String("start(startPlaying=%s)"), objects);
    if (startPlaying) {
        mPlaying = TRUE;

        t = 0;
        // there's a sucker born every OBSTACLE_PERIOD
        Float gt;
        GetGameTime(&gt);
        mLastPipeTime = gt - PARAMS->OBSTACLE_PERIOD;

        Float alpha;
        mSplash->GetAlpha(&alpha);
        if (mSplash != NULL && alpha > 0.0f) {
            mSplash->SetTranslationZ(PARAMS->HUD_Z);
            AutoPtr<IViewPropertyAnimator> animator;
            mSplash->Animate((IViewPropertyAnimator**)&animator);
            animator->Alpha(0);
            animator->TranslationZ(0);
            animator->SetDuration(400);

            animator = NULL;
            IView::Probe(mScoreField)->Animate((IViewPropertyAnimator**)&animator);
            animator->TranslationY(0);
            AutoPtr<ITimeInterpolator> ti;
            CDecelerateInterpolator::New((ITimeInterpolator**)&ti);
            animator->SetInterpolator(ti);
            animator->SetDuration(1500);
        }

        mScoreField->SetTextColor(0xFFAAAAAA);
        IView::Probe(mScoreField)->SetBackgroundResource(R::drawable::scorecard);
        mDroid->SetVisibility(IView::VISIBLE);
        mDroid->SetX(mWidth / 2);
        mDroid->SetY(mHeight / 2);
    }
    else {
        mDroid->SetVisibility(IView::GONE);
    }
    if (!mAnimating) {
        IAnimator::Probe(mAnim)->Start();
        mAnimating = TRUE;
    }
}

void LLand::Stop()
{
    if (mAnimating) {
        IAnimator::Probe(mAnim)->Cancel();
        mAnim = NULL;
        mAnimating = FALSE;
        mScoreField->SetTextColor(0xFFFFFFFF);
        IView::Probe(mScoreField)->SetBackgroundResource(R::drawable::scorecard_gameover);
        Irand(0, SKIES->GetLength(), &mTimeOfDay); // for next reset
        mFrozen = TRUE;
        AutoPtr<LLand_Stop_Runnable> action = new LLand_Stop_Runnable(this);
        Boolean result;
        PostDelayed((IRunnable*)action.Get(), 250, &result);
    }
}

ECode LLand::Lerp(
    /* [in] */ Float x,
    /* [in] */ Float a,
    /* [in] */ Float b,
    /* [out] */ Float* f)
{
    VALIDATE_NOT_NULL(f)
    *f = (b - a) * x + a;
    return NOERROR;
}

ECode LLand::Rlerp(
    /* [in] */ Float v,
    /* [in] */ Float a,
    /* [in] */ Float b,
    /* [out] */ Float* f)
{
    VALIDATE_NOT_NULL(f)
    *f = (v - a) / (b - a);
    return NOERROR;
}

ECode LLand::Clamp(
    /* [in] */ Float f,
    /* [out] */ Float* res)
{
    VALIDATE_NOT_NULL(res)
    *res = f < 0.0f ? 0.0f : f > 1.0f ? 1.0f : f;
    return NOERROR;
}

ECode LLand::Frand(
    /* [out] */ Float* f)
{
    VALIDATE_NOT_NULL(f)
    *f = (Float) Elastos::Core::Math::Random();
    return NOERROR;
}

ECode LLand::Frand(
    /* [in] */ Float a,
    /* [in] */ Float b,
    /* [out] */ Float* f)
{
    VALIDATE_NOT_NULL(f)
    Frand(f);
    Lerp(*f, a, b, f);
    return NOERROR;
}

ECode LLand::Irand(
    /* [in] */ Int32 a,
    /* [in] */ Int32 b,
    /* [out] */ Int32* f)
{
    VALIDATE_NOT_NULL(f)
    Float ff;
    Frand(&ff);
    Lerp(ff, (Float) a, (Float) b, &ff);
    *f = (Int32)ff;
    return NOERROR;
}

void LLand::Step(
    /* [in] */ Int64 t_ms,
    /* [in] */ Int64 dt_ms)
{
    t = t_ms / 1000.0f; // seconds
    dt = dt_ms / 1000.0f;

    if (DEBUG) {
        t *= DEBUG_SPEED_MULTIPLIER;
        dt *= DEBUG_SPEED_MULTIPLIER;
    }

    // 1. Move all objects and update bounds
    Int32 count;
    GetChildCount(&count);
    const Int32 N = count;
    Int32 i = 0;
    for (; i < N; i++) {
        AutoPtr<IView> v;
        GetChildAt(i, (IView**)&v);
        if (IGameView::Probe(v) != NULL) {
            IGameView::Probe(v)->Step(t_ms, dt_ms, t, dt);
        }
    }

    // 2. Check for altitude
    Boolean isBelow;
    mDroid->Below(mHeight, &isBelow);
    if (mPlaying && isBelow) {
        if (DEBUG_IDDQD) {
            Poke();
        }
        else {
            L(String("player hit the floor"), NULL);
            Stop();
        }
    }

    // 3. Check for obstacles
    Boolean passedBarrier = FALSE;
    Int32 j;
    mObstaclesInPlay->GetSize(&j);
    for (; j-- > 0;) {
        AutoPtr<IInterface> obj;
        mObstaclesInPlay->Get(j, (IInterface**)&obj);
        AutoPtr<Obstacle> ob = (Obstacle*) (IObject::Probe(obj));
        Boolean isIntersects, isCleared;
        ob->Intersects(mDroid, &isIntersects);
        ob->Cleared(mDroid, &isCleared);
        if (mPlaying && isIntersects && !DEBUG_IDDQD) {
            L(String("player hit an obstacle"), NULL);
            Stop();
        }
        else if (isCleared) {
            if (ob->type == 1) passedBarrier = TRUE;
            mObstaclesInPlay->Remove(j);
        }
    }

    if (mPlaying && passedBarrier) {
        AddScore(1);
    }

    // 4. Handle edge of screen
    // Walk backwards to make sure removal is safe
    while (i-- > 0) {
        AutoPtr<IView> v;
        GetChildAt(i, (IView**)&v);
        if (IObstacle::Probe(v) != NULL) {
            Float x;
            v->GetTranslationX(&x);
            Int32 width;
            v->GetWidth(&width);
            if (x + width < 0) {
                RemoveViewAt(i);
            }
        }
        else if (IScenery::Probe(v) != NULL) {
            const AutoPtr<Scenery> s = (Scenery*)v.Get();
            Float x;
            v->GetTranslationX(&x);
            if (x + s->w < 0) {
                Int32 width;
                GetWidth(&width);
                v->SetTranslationX(width);
            }
        }
    }

    // 3. Time for more obstacles!
    if (mPlaying && (t - mLastPipeTime) > PARAMS->OBSTACLE_PERIOD) {
        mLastPipeTime = t;
        const Int32 obstacley = (Int32) (Elastos::Core::Math::Random()
                * (mHeight - 2*PARAMS->OBSTACLE_MIN - PARAMS->OBSTACLE_GAP)) + PARAMS->OBSTACLE_MIN;

        const Int32 inset = (PARAMS->OBSTACLE_WIDTH - PARAMS->OBSTACLE_STEM_WIDTH) / 2;
        const Int32 yinset = PARAMS->OBSTACLE_WIDTH / 2;

        Int32 d11;
        Irand(0,250, &d11);
        const Int32 d1 = d11;
        AutoPtr<IContext> ctx;
        GetContext((IContext**)&ctx);
        AutoPtr<Stem> stem = new Stem(ctx, obstacley - yinset, FALSE, this);
        AutoPtr<Obstacle> s1 = stem;


        AutoPtr<IFrameLayoutLayoutParams> lp;
        CFrameLayoutLayoutParams::New(
            PARAMS->OBSTACLE_STEM_WIDTH,
            (Int32) (s1->h),
            IGravity::TOP | IGravity::LEFT,
            (IFrameLayoutLayoutParams**)&lp);
        AddView(s1, IViewGroupLayoutParams::Probe(lp));

        s1->SetTranslationX(mWidth + inset);
        s1->SetTranslationY(-s1->h - yinset);
        s1->SetTranslationZ(PARAMS->OBSTACLE_Z * 0.75f);
        AutoPtr<IViewPropertyAnimator> animator;
        s1->Animate((IViewPropertyAnimator**)&animator);
        animator->TranslationY(0);
        animator->SetStartDelay(d1);
        animator->SetDuration(250);
        mObstaclesInPlay->Add(s1->Probe(EIID_IInterface));

        AutoPtr<Pop> pop = new Pop(ctx, PARAMS->OBSTACLE_WIDTH, this);
        AutoPtr<Obstacle> p1 = pop;
        AutoPtr<IFrameLayoutLayoutParams> lp2;
        CFrameLayoutLayoutParams::New(
            PARAMS->OBSTACLE_WIDTH,
            PARAMS->OBSTACLE_WIDTH,
            IGravity::TOP | IGravity::LEFT,
            (IFrameLayoutLayoutParams**)&lp2);
        AddView(p1, IViewGroupLayoutParams::Probe(lp2));

        p1->SetTranslationX(mWidth);
        p1->SetTranslationY(-PARAMS->OBSTACLE_WIDTH);
        p1->SetTranslationZ(PARAMS->OBSTACLE_Z);
        p1->SetScaleX(0.25f);
        p1->SetScaleY(0.25f);
        animator = NULL;
        p1->Animate((IViewPropertyAnimator**)&animator);
        animator->TranslationY(s1->h-inset);
        animator->ScaleX(1.0f);
        animator->ScaleY(1.0f);
        animator->SetStartDelay(d1);
        animator->SetDuration(250);
        mObstaclesInPlay->Add(p1->Probe(EIID_IInterface));

        Int32 d22;
        Irand(0,250, &d22);
        const Int32 d2 = d22;
        AutoPtr<Stem> stem2 =
            new Stem(ctx, mHeight - obstacley - PARAMS->OBSTACLE_GAP - yinset, TRUE, this);
        const AutoPtr<Obstacle> s2 = stem2;
        AutoPtr<IFrameLayoutLayoutParams> lp3;
        CFrameLayoutLayoutParams::New(
            PARAMS->OBSTACLE_STEM_WIDTH,
            (Int32) s2->h,
            IGravity::TOP | IGravity::LEFT,
            (IFrameLayoutLayoutParams**)&lp3);
        AddView(s2, IViewGroupLayoutParams::Probe(lp3));

        s2->SetTranslationX(mWidth+inset);
        s2->SetTranslationY(mHeight+yinset);
        s2->SetTranslationZ(PARAMS->OBSTACLE_Z * 0.75f);
        animator = NULL;
        s2->Animate((IViewPropertyAnimator**)&animator);
        animator->TranslationY(mHeight - s2->h);
        animator->SetStartDelay(d2);
        animator->SetDuration(400);
        mObstaclesInPlay->Add(s2->Probe(EIID_IInterface));

        AutoPtr<Pop> pop2 = new Pop(ctx, PARAMS->OBSTACLE_WIDTH, this);
        const AutoPtr<Obstacle> p2 = pop2;

        AutoPtr<IFrameLayoutLayoutParams> lp4;
        CFrameLayoutLayoutParams::New(
            PARAMS->OBSTACLE_WIDTH,
            PARAMS->OBSTACLE_WIDTH,
            IGravity::TOP | IGravity::LEFT,
            (IFrameLayoutLayoutParams**)&lp4);
        AddView(p2, IViewGroupLayoutParams::Probe(lp4));

        p2->SetTranslationX(mWidth);
        p2->SetTranslationY(mHeight);
        p2->SetTranslationZ(PARAMS->OBSTACLE_Z);
        p2->SetScaleX(0.25f);
        p2->SetScaleY(0.25f);
        animator = NULL;
        p2->Animate((IViewPropertyAnimator**)&animator);
        animator->TranslationY(mHeight- s2->h - yinset);
        animator->ScaleX(1.0f);
        animator->ScaleY(1.0f);
        animator->SetStartDelay(d2);
        animator->SetDuration(400);
        mObstaclesInPlay->Add(p2->Probe(EIID_IInterface));
    }

    if (ILLand::DEBUG_DRAW) Invalidate();
}

ECode LLand::OnTouchEvent(
    /* [in] */ IMotionEvent* ev,
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(result)
    AutoPtr<ArrayOf<IInterface*> > objects = ArrayOf<IInterface*>::Alloc(1);
    objects->Set(0, ev);
    if (DEBUG) L(String("touch: %s"), objects);
    Int32 action;
    ev->GetAction(&action);
    switch (action) {
        case IMotionEvent::ACTION_DOWN:
            Poke();
            *result = TRUE;
            return NOERROR;
        case IMotionEvent::ACTION_UP:
            Unpoke();
            *result = TRUE;
            return NOERROR;
    }
    *result = FALSE;
    return NOERROR;
}

ECode LLand::OnTrackballEvent(
    /* [in] */ IMotionEvent* ev,
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(result)
    AutoPtr<ArrayOf<IInterface*> > objects = ArrayOf<IInterface*>::Alloc(1);
    objects->Set(0, ev);
    if (DEBUG) L(String("trackball: %s"), objects);
    Int32 action;
    ev->GetAction(&action);
    switch (action) {
        case IMotionEvent::ACTION_DOWN:
            Poke();
            *result = TRUE;
            return NOERROR;
        case IMotionEvent::ACTION_UP:
            Unpoke();
            *result = TRUE;
            return NOERROR;
    }
    *result = FALSE;
    return NOERROR;
}

ECode LLand::OnKeyDown(
    /* [in] */ Int32 keyCode,
    /* [in] */ IKeyEvent* ev,
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(result)
    AutoPtr<IInteger32> val;
    CInteger32::New(keyCode, (IInteger32**)&val);
    AutoPtr<ArrayOf<IInterface*> > objects = ArrayOf<IInterface*>::Alloc(1);
    objects->Set(0, val);
    if (DEBUG) L(String("keyDown: %d"), objects);
    switch (keyCode) {
        case IKeyEvent::KEYCODE_DPAD_CENTER:
        case IKeyEvent::KEYCODE_DPAD_UP:
        case IKeyEvent::KEYCODE_SPACE:
        case IKeyEvent::KEYCODE_ENTER:
        case IKeyEvent::KEYCODE_BUTTON_A:
            Poke();
            *result = TRUE;
            return NOERROR;
    }
    *result = FALSE;
    return NOERROR;
}

ECode LLand::OnKeyUp(
    /* [in] */ Int32 keyCode,
    /* [in] */ IKeyEvent* ev,
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(result)
    AutoPtr<IInteger32> val;
    CInteger32::New(keyCode, (IInteger32**)&val);
    AutoPtr<ArrayOf<IInterface*> > objects = ArrayOf<IInterface*>::Alloc(1);
    objects->Set(0, val);
    if (DEBUG) L(String("keyUp: %d"), objects);
    switch (keyCode) {
        case IKeyEvent::KEYCODE_DPAD_CENTER:
        case IKeyEvent::KEYCODE_DPAD_UP:
        case IKeyEvent::KEYCODE_SPACE:
        case IKeyEvent::KEYCODE_ENTER:
        case IKeyEvent::KEYCODE_BUTTON_A:
            Unpoke();
            *result = TRUE;
            return NOERROR;
    }
    *result = FALSE;
    return NOERROR;
}

ECode LLand::OnGenericMotionEvent(
    /* [in] */ IMotionEvent* ev,
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(result)
    AutoPtr<ArrayOf<IInterface*> > objects = ArrayOf<IInterface*>::Alloc(1);
    objects->Set(0, ev);
    if (DEBUG) L(String("generic: %s"), objects);
    *result = FALSE;
    return NOERROR;
}

void LLand::Poke()
{
    L(String("poke"), NULL);
    if (mFrozen) return;
    if (!mAnimating) {
        Reset();
        Start(TRUE);
    }
    else if (!mPlaying) {
        Start(TRUE);
    }
    mDroid->Boost();
    if (DEBUG) {
        mDroid->dv *= DEBUG_SPEED_MULTIPLIER;
        AutoPtr<IViewPropertyAnimator> animator;
        mDroid->Animate((IViewPropertyAnimator**)&animator);
        animator->SetDuration((Int64) (200/DEBUG_SPEED_MULTIPLIER));
    }
}

void LLand::Unpoke()
{
    L(String("unboost"), NULL);
    if (mFrozen) return;
    if (!mAnimating) return;
    mDroid->Unboost();
}

void LLand::OnDraw(
    /* [in] */ ICanvas* c)
{
    // super.onDraw(c);

    if (!ILLand::DEBUG_DRAW) return;

    AutoPtr<IPaint> pt;
    CPaint::New((IPaint**)&pt);
    pt->SetColor(0xFFFFFFFF);
    const Int32 L = mDroid->corners->GetLength();
    const Int32 N = L/2;
    for (Int32 i=0; i < N; i++) {
        const Int32 x = (Int32) (*(mDroid->corners))[i * 2];
        const Int32 y = (Int32) (*(mDroid->corners))[i * 2 + 1];
        c->DrawCircle(x, y, 4, pt);
        c->DrawLine(x, y,
                (*(mDroid->corners))[(i*2+2)%L],
                (*(mDroid->corners))[(i*2+3)%L],
                pt);
    }

    pt->SetStyle(PaintStyle_STROKE);
    AutoPtr<IResources> resources;
    GetResources((IResources**)&resources);
    AutoPtr<IDisplayMetrics> metrics;
    resources->GetDisplayMetrics((IDisplayMetrics**)&metrics);
    Float density;
    metrics->GetDensity(&density);
    pt->SetStrokeWidth(density);

    Int32 count;
    GetChildCount(&count);
    const Int32 M = count;
    pt->SetColor(0x8000FF00);
    for (Int32 i = 0; i < M; i++) {
        AutoPtr<IView> v;
        GetChildAt(i, (IView**)&v);
        if (v == mDroid) continue;
        if (IGameView::Probe(v) == NULL) continue;
        if (IPop::Probe(v) != NULL) {
            const AutoPtr<Pop> p = (Pop*) v.Get();
            c->DrawCircle(p->cx, p->cy, p->r, pt);
        }
        else {
            AutoPtr<IRect> r;
            CRect::New((IRect**)&r);
            v->GetHitRect(r);
            c->DrawRect(r, pt);
        }
    }

    pt->SetColor(IColor::BLACK);
    StringBuilder sb("obstacles: ");

    AutoPtr<IIterator> it;
    IIterable::Probe(mObstaclesInPlay)->GetIterator((IIterator**)&it);
    Boolean hasNext;
    while(it->HasNext(&hasNext), hasNext) {
        AutoPtr<IInterface> obj;
        it->GetNext((IInterface**)&obj);
        AutoPtr<Obstacle> ob = (Obstacle*)(IObject::Probe(obj));
        String str;
        ob->hitRect->ToShortString(&str);
        sb += str;
        sb += " ";
    }
    pt->SetTextSize(20.0f);
    c->DrawText(sb.ToString(), 20, 100, pt);
}

} // namespace Egg
} // namespace SystemUI
} // namespace Droid
} // namespace Elastos