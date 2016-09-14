
#include "elastos/droid/internal/app/PlatLogoActivity.h"
#include "elastos/droid/animation/ObjectAnimator.h"
#include "elastos/droid/content/CIntent.h"
#include "elastos/droid/content/res/CColorStateList.h"
#include "elastos/droid/graphics/CPaint.h"
#include "elastos/droid/graphics/CPath.h"
#include "elastos/droid/graphics/drawable/CGradientDrawable.h"
#include "elastos/droid/graphics/drawable/CShapeDrawable.h"
#include "elastos/droid/graphics/drawable/CRippleDrawable.h"
#include "elastos/droid/graphics/drawable/shapes/COvalShape.h"
#include "elastos/droid/provider/Settings.h"
#include "elastos/droid/view/animation/CPathInterpolator.h"
#include "elastos/droid/widget/CImageView.h"
#include "elastos/droid/widget/CFrameLayoutLayoutParams.h"
#include "elastos/droid/widget/CFrameLayout.h"
#include "Elastos.Droid.Provider.h"
#include "elastos/droid/R.h"
#include <elastos/core/Math.h>
#include <elastos/utility/logging/Logger.h>

using Elastos::Droid::Animation::IAnimator;
using Elastos::Droid::Animation::ObjectAnimator;
using Elastos::Droid::Content::CIntent;
using Elastos::Droid::Content::IContentResolver;
using Elastos::Droid::Content::Res::CColorStateList;
using Elastos::Droid::Content::Res::IColorStateList;
using Elastos::Droid::Graphics::CPaint;
using Elastos::Droid::Graphics::CPath;
using Elastos::Droid::Graphics::IOutline;
using Elastos::Droid::Graphics::Drawable::CGradientDrawable;
using Elastos::Droid::Graphics::Drawable::CShapeDrawable;
using Elastos::Droid::Graphics::Drawable::CRippleDrawable;
using Elastos::Droid::Graphics::Drawable::GradientDrawableOrientation_LEFT_RIGHT;
using Elastos::Droid::Graphics::Drawable::IGradientDrawable;
using Elastos::Droid::Graphics::Drawable::IShapeDrawable;
using Elastos::Droid::Graphics::Drawable::Shapes::COvalShape;
using Elastos::Droid::Graphics::Drawable::Shapes::IShape;
using Elastos::Droid::Provider::ISettingsSystem;
using Elastos::Droid::Provider::Settings;
using Elastos::Droid::Utility::IDisplayMetrics;
using Elastos::Droid::View::EIID_IViewOnClickListener;
using Elastos::Droid::View::EIID_IViewOnLongClickListener;
using Elastos::Droid::View::EIID_IViewOnKeyListener;
using Elastos::Droid::View::EIID_IViewOutlineProvider;
using Elastos::Droid::View::IGravity;
using Elastos::Droid::View::IViewGroup;
using Elastos::Droid::View::IViewGroupLayoutParams;
using Elastos::Droid::View::IViewOverlay;
using Elastos::Droid::View::IViewPropertyAnimator;
using Elastos::Droid::View::Animation::CPathInterpolator;
using Elastos::Droid::Widget::CFrameLayout;
using Elastos::Droid::Widget::CFrameLayoutLayoutParams;
using Elastos::Droid::Widget::CImageView;
using Elastos::Core::CSystem;
using Elastos::Core::ISystem;
using Elastos::Utility::Logging::Logger;

namespace Elastos {
namespace Droid {
namespace Internal {
namespace App {

CAR_INTERFACE_IMPL(PlatLogoActivity::StickView::ViewOutlineProvider, Object, IViewOutlineProvider)

PlatLogoActivity::StickView::ViewOutlineProvider::ViewOutlineProvider(
    /* [in] */ StickView* host)
    : mHost(host)
{}

// @Override
ECode PlatLogoActivity::StickView::ViewOutlineProvider::GetOutline(
    /* [in] */ IView* view,
    /* [in] */ IOutline* outline)
{
    Int32 w, h;
    mHost->GetWidth(&w);
    mHost->GetHeight(&h);
    return outline->SetRect(0, h / 2, w, h);
}

ECode PlatLogoActivity::StickView::constructor(
    /* [in] */ PlatLogoActivity* host,
    /* [in] */ Int32 size)
{
    mHost = host;
    mSize = size;
    CPaint::New((IPaint**)&mPaint);
    CPath::New((IPath**)&mShadow);
    return View::constructor(host);
}

// @Override
ECode PlatLogoActivity::StickView::OnAttachedToWindow()
{
    View::OnAttachedToWindow();
    SetWillNotDraw(FALSE);
    AutoPtr<ViewOutlineProvider> provider = new ViewOutlineProvider(this);
    SetOutlineProvider(provider);
    return NOERROR;
}

// @Override
void PlatLogoActivity::StickView::OnDraw(
    /* [in] */ ICanvas* c)
{
    Int32 w, h;
    c->GetWidth(&w);
    c->GetHeight(&h);
    h /= 2;
    c->Translate(0, h);
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
    mPaint->SetColor(0xFFAAAAAA);
    mShadow->Reset();
    mShadow->MoveTo(0,0);
    mShadow->LineTo(w, 0);
    mShadow->LineTo(w, mSize/2 + 1.5f*w);
    mShadow->LineTo(0, mSize/2);
    mShadow->Close();
    c->DrawPath(mShadow, mPaint);
}

ECode PlatLogoActivity::LocalRunnable::Run()
{
    AutoPtr<IIntent> intent;
    CIntent::New(IIntent::ACTION_MAIN, (IIntent**)&intent);
    intent->SetFlags(IIntent::FLAG_ACTIVITY_NEW_TASK
            | IIntent::FLAG_ACTIVITY_CLEAR_TASK
            | IIntent::FLAG_ACTIVITY_EXCLUDE_FROM_RECENTS);
    intent->PutExtra(String("is_cm"), mHost->mIsCM);
    intent->AddCategory(String("com.android.internal.category.PLATLOGO"));
    if (mHost->StartActivity(intent) == (ECode)E_ACTIVITY_NOT_FOUND_EXCEPTION) {
        Logger::E("PlatLogoActivity", "No more eggs.");
    }
    mHost->Finish();
    return NOERROR;
}

CAR_INTERFACE_IMPL_3(PlatLogoActivity::LocalListener::LocalListener, Object,
    IViewOnClickListener, IViewOnLongClickListener, IViewOnKeyListener)

PlatLogoActivity::LocalListener::LocalListener(
    /* [in] */ PlatLogoActivity* host,
    /* [in] */ IImageView* im,
    /* [in] */ StickView* stick,
    /* [in] */ IDrawable* platlogo)
    : mHost(host)
    , mIm(im)
    , mStick(stick)
    , mPlatlogo(platlogo)
{}

ECode PlatLogoActivity::LocalListener::LocalListener::OnClick(
    /* [in] */ IView* v)
{
    AutoPtr<IView> im = IView::Probe(mIm);
    if (mHost->mTapCount == 0) {
        AutoPtr<IViewPropertyAnimator> animator;
        im->Animate((IViewPropertyAnimator**)&animator);
        animator->TranslationZ(40);
        animator->ScaleX(1);
        animator->ScaleY(1);
        animator->SetInterpolator(mHost->mInterpolator);
        animator->SetDuration(700);
        animator->SetStartDelay(500);
        animator->Start();

        AutoPtr<ArrayOf<Int32> > params = ArrayOf<Int32>::Alloc(2);
        (*params)[0] = 0;
        (*params)[1] = 255;
        AutoPtr<IAnimator> a = IAnimator::Probe(ObjectAnimator::OfInt32(mPlatlogo, String("alpha"), params));
        a->SetInterpolator(mHost->mInterpolator);
        a->SetStartDelay(1000);
        a->Start();

        animator = NULL;
        mStick->Animate((IViewPropertyAnimator**)&animator);
        animator->TranslationZ(20);
        animator->Alpha(1);
        animator->SetInterpolator(mHost->mInterpolator);
        animator->SetDuration(700);
        animator->SetStartDelay(750);
        animator->Start();

        AutoPtr<LocalListener> listener = new LocalListener(mHost, mIm, NULL, NULL);
        im->SetOnLongClickListener(listener);
    }
    else {
        im->SetBackground(mHost->MakeRipple());
    }
    mHost->mTapCount++;

    return NOERROR;
}

ECode PlatLogoActivity::LocalListener::OnLongClick(
    /* [in] */ IView* v,
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(result)
    if (mHost->mTapCount < 5) {
        *result = FALSE;
        return NOERROR;
    }

    AutoPtr<IContentResolver> cr;
    mHost->GetContentResolver((IContentResolver**)&cr);
    Int64 value;
    Settings::System::GetInt64(cr, ISettingsSystem::EGG_MODE, 0, &value);
    if (value == 0) {
        // For posterity: the moment this user unlocked the easter egg
        AutoPtr<ISystem> system;
        CSystem::AcquireSingleton((ISystem**)&system);
        Int64 t;
        system->GetCurrentTimeMillis(&t);
        Boolean res;
        Settings::System::PutInt64(cr, ISettingsSystem::EGG_MODE, t, &res);
    }
    AutoPtr<LocalRunnable> runnable = new LocalRunnable(mHost);
    Boolean res;
    IView::Probe(mIm)->Post(runnable, &res);
    *result = TRUE;
    return NOERROR;
}

ECode PlatLogoActivity::LocalListener::OnKey(
    /* [in] */ IView* v,
    /* [in] */ Int32 keyCode,
    /* [in] */ IKeyEvent* event,
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(result)
    Int32 action;
    event->GetAction(&action);
    if (keyCode != IKeyEvent::KEYCODE_BACK && action == IKeyEvent::ACTION_DOWN) {
        ++mHost->mKeyCount;
        if (mHost->mKeyCount > 2) {
            Boolean res;
            if (mHost->mTapCount > 5) {
                IView::Probe(mIm)->PerformLongClick(&res);
            }
            else {
                IView::Probe(mIm)->PerformClick(&res);
            }
        }
        *result = TRUE;
    }
    else {
        *result = FALSE;
    }

    return NOERROR;
}

const Int32 PlatLogoActivity::FLAVORS_LENGTH;
const Int32 PlatLogoActivity::FLAVORS[PlatLogoActivity::FLAVORS_LENGTH] = {
        (Int32)0xFF9C27B0, (Int32)0xFFBA68C8, // grape
        (Int32)0xFFFF9800, (Int32)0xFFFFB74D, // orange
        (Int32)0xFFF06292, (Int32)0xFFF8BBD0, // bubblegum
        (Int32)0xFFAFB42B, (Int32)0xFFCDDC39, // lime
        (Int32)0xFFFFEB3B, (Int32)0xFFFFF176, // lemon
        (Int32)0xFF795548, (Int32)0xFFA1887F, // mystery flavor
    };

CAR_INTERFACE_IMPL(PlatLogoActivity, Activity, IPlatLogoActivity)

PlatLogoActivity::PlatLogoActivity()
    : mTapCount(0)
    , mKeyCount(0)
{
    CPathInterpolator::New(0.f, 0.f, 0.5f, 1.f, (ITimeInterpolator**)&mInterpolator);
}

Int32 PlatLogoActivity::NewColorIndex()
{
    return 2 * ((Int32)(Elastos::Core::Math::Random() * FLAVORS_LENGTH / 2));
}

AutoPtr<IDrawable> PlatLogoActivity::MakeRipple()
{
    Int32 idx = NewColorIndex();
    AutoPtr<IShape> shape;
    COvalShape::New((IShape**)&shape);
    AutoPtr<IShapeDrawable> popbg;
    CShapeDrawable::New(shape, (IShapeDrawable**)&popbg);
    AutoPtr<IPaint> paint;
    popbg->GetPaint((IPaint**)&paint);
    paint->SetColor(FLAVORS[idx]);
    AutoPtr<IDrawable> ripple;
    AutoPtr<IColorStateList> csl;
    CColorStateList::ValueOf(FLAVORS[idx+1], (IColorStateList**)&csl);
    CRippleDrawable::New(csl, IDrawable::Probe(popbg), NULL, (IDrawable**)&ripple);
    return ripple;
}

ECode PlatLogoActivity::OnCreate(
    /* [in] */ IBundle* savedInstanceState)
{
    Activity::OnCreate(savedInstanceState);
    CFrameLayout::New(this, (IFrameLayout**)&mLayout);
    AutoPtr<IIntent> intent;
    GetIntent((IIntent**)&intent);
    intent->HasExtra(String("is_cm"), &mIsCM);
    SetContentView(IView::Probe(mLayout));
    return NOERROR;
}

ECode PlatLogoActivity::OnAttachedToWindow()
{
    AutoPtr<IResources> resources;
    GetResources((IResources**)&resources);
    AutoPtr<IDisplayMetrics> dm;
    resources->GetDisplayMetrics((IDisplayMetrics**)&dm);
    Float dp;
    dm->GetDensity(&dp);
    Int32 wp, hp;
    dm->GetWidthPixels(&wp);
    dm->GetHeightPixels(&hp);
    Int32 size = (Int32)(Elastos::Core::Math::Min(
        (Float)Elastos::Core::Math::Min(wp, hp), 600 * dp) - 100 * dp);

    AutoPtr<StickView> stick = new StickView();
    stick->constructor(this, size);

    AutoPtr<IViewGroupLayoutParams> lp;
    CFrameLayoutLayoutParams::New((Int32)(32 * dp), IViewGroupLayoutParams::MATCH_PARENT,
        IGravity::CENTER_HORIZONTAL, (IViewGroupLayoutParams**)&lp);
    IViewGroup::Probe(mLayout)->AddView(stick, lp);
    stick->SetAlpha(0.f);

    AutoPtr<IView> im;
    CImageView::New(this, (IView**)&im);
    AutoPtr<IImageView> imageview = IImageView::Probe(im);
    im->SetTranslationZ(20);
    im->SetScaleX(0);
    im->SetScaleY(0);
    AutoPtr<IDrawable> platlogo;
    GetDrawable(mIsCM ? R::drawable::cm_platlogo : R::drawable::platlogo,
        (IDrawable**)&platlogo);
    platlogo->SetAlpha(0);
    imageview->SetImageDrawable(platlogo);
    im->SetBackground(MakeRipple());
    im->SetClickable(TRUE);
    AutoPtr<IShape> shape;
    COvalShape::New((IShape**)&shape);
    AutoPtr<IDrawable> highlight;
    CShapeDrawable::New(shape, (IDrawable**)&highlight);
    AutoPtr<IPaint> paint;
    IShapeDrawable::Probe(highlight)->GetPaint((IPaint**)&paint);
    paint->SetColor(0x10FFFFFF);
    highlight->SetBounds(
        (Int32)(size * .15f), (Int32)(size * .15f), (Int32)(size * .6f), (Int32)(size * .6f));
    AutoPtr<IViewOverlay> overlay;
    im->GetOverlay((IViewOverlay**)&overlay);
    overlay->Add(highlight);
    AutoPtr<LocalListener> listener = new LocalListener(this, imageview, stick, platlogo);
    im->SetOnClickListener(listener);

    // Enable hardware keyboard input for TV compatibility.
    im->SetFocusable(TRUE);
    Boolean res;
    im->RequestFocus(&res);
    im->SetOnKeyListener(listener);

    lp = NULL;
    CFrameLayoutLayoutParams::New(size, size, IGravity::CENTER, (IViewGroupLayoutParams**)&lp);
    IViewGroup::Probe(mLayout)->AddView(im, lp);

    AutoPtr<IViewPropertyAnimator> animator;
    im->Animate((IViewPropertyAnimator**)&animator);
    animator->ScaleX(0.3f);
    animator->ScaleY(0.3f);
    animator->SetInterpolator(mInterpolator);
    animator->SetDuration(500);
    animator->SetStartDelay(800);
    animator->Start();
    return NOERROR;
}

} //namespace App
} //namespace Internal
} //namespace Droid
} //namespace Elastos
