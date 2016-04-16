
#include "elastos/droid/launcher2/CCling.h"
#include "Elastos.Droid.Service.h"
#include <elastos/core/Math.h>
#include "R.h"

using Elastos::Droid::Graphics::IPaint;
using Elastos::Droid::Graphics::CPaint;
using Elastos::Droid::Graphics::CCanvas;
using Elastos::Droid::Graphics::IXfermode;
using Elastos::Droid::Graphics::CPorterDuffXfermode;
using Elastos::Droid::Graphics::IPorterDuffXfermode;
using Elastos::Droid::Graphics::PorterDuffMode_MULTIPLY;
using Elastos::Droid::Graphics::BitmapConfig_ARGB_8888;
using Elastos::Droid::Graphics::CBitmapHelper;
using Elastos::Droid::Graphics::IBitmapHelper;
using Elastos::Droid::View::IDisplay;
using Elastos::Droid::View::IFocusFinder;
using Elastos::Droid::View::CFocusFinderHelper;
using Elastos::Droid::View::IFocusFinderHelper;
using Elastos::Droid::View::IWindowManager;
using Elastos::Droid::Utility::CDisplayMetrics;
using Elastos::Droid::Utility::IDisplayMetrics;

namespace Elastos {
namespace Droid {
namespace Launcher2 {

String CCling::WORKSPACE_PORTRAIT("workspace_portrait");
String CCling::WORKSPACE_LANDSCAPE("workspace_landscape");
String CCling::WORKSPACE_LARGE("workspace_large");
String CCling::WORKSPACE_CUSTOM("workspace_custom");

String CCling::ALLAPPS_PORTRAIT("all_apps_portrait");
String CCling::ALLAPPS_LANDSCAPE("all_apps_landscape");
String CCling::ALLAPPS_LARGE("all_apps_large");

String CCling::FOLDER_PORTRAIT("folder_portrait");
String CCling::FOLDER_LANDSCAPE("folder_landscape");
String CCling::FOLDER_LARGE("folder_large");

CAR_INTERFACE_IMPL(CCling, FrameLayout, ICling);

CAR_OBJECT_IMPL(CCling);

CCling::CCling()
    : mIsInitialized(FALSE)
    , mDrawIdentifier(NULL)
    , mPunchThroughGraphicCenterRadius(0)
    , mAppIconSize(0)
    , mButtonBarHeight(0)
    , mRevealRadius(0.0f)
{
}

ECode CCling::constructor(
    /* [in] */ IContext* context)
{
    return constructor(context, NULL, 0);
}

ECode CCling::constructor(
    /* [in] */ IContext* context,
    /* [in] */ IAttributeSet* attrs)
{
    return constructor(context, attrs, 0);
}

ECode CCling::constructor(
    /* [in] */ IContext* context,
    /* [in] */ IAttributeSet* attrs,
    /* [in] */ Int32 defStyle)
{
    FrameLayout::constructor(context, attrs, defStyle);

    AutoPtr<ArrayOf<Int32> > attrIds = ArrayOf<Int32>::Alloc(
            const_cast<Int32 *>(Elastos::Droid::Launcher2::R::styleable::Cling),
            ArraySize(Elastos::Droid::Launcher2::R::styleable::Cling));
    AutoPtr<ITypedArray> a;
    context->ObtainStyledAttributes(attrs, attrIds, defStyle, 0, (ITypedArray**)&a);
    a->GetString(Elastos::Droid::Launcher2::R::styleable::Cling_drawIdentifier, &mDrawIdentifier);
    a->Recycle();

    return SetClickable(TRUE);
}

ECode CCling::CCling_Init(
    /* [in] */ ILauncher* l,
    /* [in] */ ArrayOf<Int32>* positionData)
{
    if (!mIsInitialized) {
        mLauncher = l;
        mPositionData = positionData;

        AutoPtr<IContext> context;
        GetContext((IContext**)&context);
        AutoPtr<IResources> r;
        context->GetResources((IResources**)&r);

        r->GetDrawable(Elastos::Droid::Launcher2::R::drawable::cling, (IDrawable**)&mPunchThroughGraphic);
        r->GetDimensionPixelSize(Elastos::Droid::Launcher2::R::dimen::clingPunchThroughGraphicCenterRadius,
                &mPunchThroughGraphicCenterRadius);
        r->GetDimensionPixelSize(Elastos::Droid::Launcher2::R::dimen::app_icon_size,
                &mAppIconSize);

        Int32 size;
        r->GetDimensionPixelSize(Elastos::Droid::Launcher2::R::dimen::reveal_radius, &size);
        mRevealRadius = size * 1.0f;
        r->GetDimensionPixelSize(Elastos::Droid::Launcher2::R::dimen::button_bar_height, &mButtonBarHeight);

        CPaint::New((IPaint**)&mErasePaint);
        AutoPtr<IPorterDuffXfermode> mode;
        CPorterDuffXfermode::New(PorterDuffMode_MULTIPLY, (IPorterDuffXfermode**)&mode);
        mErasePaint->SetXfermode(IXfermode::Probe(mode));
        mErasePaint->SetColor(0xFFFFFF);
        mErasePaint->SetAlpha(0);

        mIsInitialized = TRUE;
    }
    return NOERROR;
}

ECode CCling::Cleanup()
{
    mBackground = NULL;
    mPunchThroughGraphic = NULL;
    mHandTouchGraphic = NULL;
    mIsInitialized = FALSE;
    return NOERROR;
}

ECode CCling::GetDrawIdentifier(
    /* [out] */ String* str)
{
    VALIDATE_NOT_NULL(str);

    *str = mDrawIdentifier;
    return NOERROR;
}

ECode CCling::FocusSearch(
    /* [in] */ Int32 direction,
    /* [out] */ IView** res)
{
    VALIDATE_NOT_NULL(res);

    return FocusSearch(this, direction, res);
}

ECode CCling::FocusSearch(
    /* [in] */ IView* focused,
    /* [in] */ Int32 direction,
    /* [out] */ IView** res)
{
    VALIDATE_NOT_NULL(res);

    AutoPtr<IFocusFinderHelper> helper;
    CFocusFinderHelper::AcquireSingleton((IFocusFinderHelper**)&helper);
    AutoPtr<IFocusFinder> finder;
    helper->GetInstance((IFocusFinder**)&finder);
    return finder->FindNextFocus(this, focused, direction,res);
}

ECode CCling::OnHoverEvent(
    /* [in] */ IMotionEvent* event,
    /* [out] */ Boolean* res)
{
    VALIDATE_NOT_NULL(res);

    *res = (mDrawIdentifier.Equals(WORKSPACE_PORTRAIT)
            || mDrawIdentifier.Equals(WORKSPACE_LANDSCAPE)
            || mDrawIdentifier.Equals(WORKSPACE_LARGE)
            || mDrawIdentifier.Equals(ALLAPPS_PORTRAIT)
            || mDrawIdentifier.Equals(ALLAPPS_LANDSCAPE)
            || mDrawIdentifier.Equals(ALLAPPS_LARGE)
            || mDrawIdentifier.Equals(WORKSPACE_CUSTOM));
    return NOERROR;

}

ECode CCling::OnTouchEvent(
    /* [in] */ IMotionEvent* event,
    /* [out] */ Boolean* res)
{
    VALIDATE_NOT_NULL(res);

    if (mDrawIdentifier.Equals(WORKSPACE_PORTRAIT) ||
            mDrawIdentifier.Equals(WORKSPACE_LANDSCAPE) ||
            mDrawIdentifier.Equals(WORKSPACE_LARGE) ||
            mDrawIdentifier.Equals(ALLAPPS_PORTRAIT) ||
            mDrawIdentifier.Equals(ALLAPPS_LANDSCAPE) ||
            mDrawIdentifier.Equals(ALLAPPS_LARGE)) {
        AutoPtr<ArrayOf<Int32> > positions = GetPunchThroughPositions();
        for (Int32 i = 0; i < positions->GetLength(); i += 2) {
            Float x;
            event->GetX(&x);
            Float y;
            event->GetY(&y);
            Double diff = Elastos::Core::Math::Sqrt(Elastos::Core::Math::Pow(x - (*positions)[i], 2) +
                    Elastos::Core::Math::Pow(y - (*positions)[i + 1], 2));
            if (diff < mRevealRadius) {
                *res = FALSE;
                return NOERROR;
            }
        }
    }
    else if (mDrawIdentifier.Equals(FOLDER_PORTRAIT) ||
            mDrawIdentifier.Equals(FOLDER_LANDSCAPE) ||
            mDrawIdentifier.Equals(FOLDER_LARGE)) {
        AutoPtr<IWorkspace> workspace;
        assert(0 && "need class Launcher");
        //mLauncher->GetWorkspace((IWorkspace**)&workspace);
        AutoPtr<IFolder> f;
        assert(0 && "need class workspace");
        //workspace->GetOpenFolder((IFolder**)&f);
        // if (f != NULL) {
        //     AutoPtr<IRect> r;
        //     CRect::New((IRect**)&r);
        //     f->GetHitRect(r);
        //     Float x;
        //     event->GetX(&x);
        //     Float y;
        //     event->GetY(&y);
        //     Boolean tmp;
        //     r->Contains((Int32)x, (Int32)y, &tmp)
        //     if (tmp) {
        //         *res = FALSE;
        //         return NOERROR;
        //     }
        // }
    }
    *res = TRUE;
    return NOERROR;
}

ECode CCling::DispatchDraw(
    /* [in] */ ICanvas* canvas)
{
    if (mIsInitialized) {
        AutoPtr<IDisplayMetrics> metrics;
        CDisplayMetrics::New((IDisplayMetrics**)&metrics);

        AutoPtr<IWindowManager> manager;
        assert(0 && "need class Launcher");
        //mLauncher->GetWindowManager((IWindowManager**)&manager);
        AutoPtr<IDisplay> display;
        manager->GetDefaultDisplay((IDisplay**)&display);
        display->GetMetrics(metrics);

        // Initialize the draw buffer (to allow punching through)
        Int32 width;
        GetMeasuredWidth(&width);
        Int32 height;
        GetMeasuredHeight(&height);

        AutoPtr<IBitmapHelper> helper;
        CBitmapHelper::AcquireSingleton((IBitmapHelper**)&helper);
        AutoPtr<IBitmap> b;
        helper->CreateBitmap(width, height, BitmapConfig_ARGB_8888, (IBitmap**)&b);
        AutoPtr<ICanvas> c;
        CCanvas::New(b, (ICanvas**)&c);

        // Draw the background
        if (mBackground == NULL) {
            if (mDrawIdentifier.Equals(WORKSPACE_PORTRAIT) ||
                    mDrawIdentifier.Equals(WORKSPACE_LANDSCAPE) ||
                    mDrawIdentifier.Equals(WORKSPACE_LARGE)) {
                AutoPtr<IResources> resources;
                GetResources((IResources**)&resources);
                resources->GetDrawable(Elastos::Droid::Launcher2::R::drawable::bg_cling1,
                        (IDrawable**)&mBackground);
            }
            else if (mDrawIdentifier.Equals(ALLAPPS_PORTRAIT) ||
                    mDrawIdentifier.Equals(ALLAPPS_LANDSCAPE) ||
                    mDrawIdentifier.Equals(ALLAPPS_LARGE)) {
                AutoPtr<IResources> resources;
                GetResources((IResources**)&resources);
                resources->GetDrawable(Elastos::Droid::Launcher2::R::drawable::bg_cling2,
                        (IDrawable**)&mBackground);
            }
            else if (mDrawIdentifier.Equals(FOLDER_PORTRAIT) ||
                    mDrawIdentifier.Equals(FOLDER_LANDSCAPE)) {
                AutoPtr<IResources> resources;
                GetResources((IResources**)&resources);
                resources->GetDrawable(Elastos::Droid::Launcher2::R::drawable::bg_cling3,
                        (IDrawable**)&mBackground);
            }
            else if (mDrawIdentifier.Equals(FOLDER_LARGE)) {
                AutoPtr<IResources> resources;
                GetResources((IResources**)&resources);
                resources->GetDrawable(Elastos::Droid::Launcher2::R::drawable::bg_cling4,
                        (IDrawable**)&mBackground);
            }
            else if (mDrawIdentifier.Equals(WORKSPACE_CUSTOM)) {
                AutoPtr<IResources> resources;
                GetResources((IResources**)&resources);
                resources->GetDrawable(Elastos::Droid::Launcher2::R::drawable::bg_cling5,
                        (IDrawable**)&mBackground);
            }
        }

        if (mBackground != NULL) {
            Int32 width;
            GetMeasuredWidth(&width);
            Int32 height;
            GetMeasuredHeight(&height);
            mBackground->SetBounds(0, 0, width, height);
            mBackground->Draw(c);
        }
        else {
            c->DrawColor(0x99000000);
        }

        Int32 cx = -1;
        Int32 cy = -1;
        Float scale = mRevealRadius / mPunchThroughGraphicCenterRadius;
        Int32 intrinsicWidth;
        mPunchThroughGraphic->GetIntrinsicWidth(&intrinsicWidth);
        Int32 dw = (Int32)(scale * intrinsicWidth);
        Int32 IntrinsicHeight;
        mPunchThroughGraphic->GetIntrinsicHeight(&IntrinsicHeight);
        Int32 dh = (Int32)(scale * IntrinsicHeight);

        // Determine where to draw the punch through graphic
        AutoPtr<ArrayOf<Int32> > positions = GetPunchThroughPositions();
        for (Int32 i = 0; i < positions->GetLength(); i += 2) {
            cx = (*positions)[i];
            cy = (*positions)[i + 1];
            if (cx > -1 && cy > -1) {
                c->DrawCircle(cx, cy, mRevealRadius, mErasePaint);
                mPunchThroughGraphic->SetBounds(cx - dw/2, cy - dh/2, cx + dw/2, cy + dh/2);
                mPunchThroughGraphic->Draw(c);
            }
        }

        // Draw the hand graphic in All Apps
        if (mDrawIdentifier.Equals(ALLAPPS_PORTRAIT) ||
                mDrawIdentifier.Equals(ALLAPPS_LANDSCAPE) ||
                mDrawIdentifier.Equals(ALLAPPS_LARGE)) {
            if (mHandTouchGraphic == NULL) {
                AutoPtr<IResources> resources;
                GetResources((IResources**)&resources);
                resources->GetDrawable(Elastos::Droid::Launcher2::R::drawable::hand,
                        (IDrawable**)&mHandTouchGraphic);
            }
            Int32 offset = mAppIconSize / 4;
            Int32 width;
            mHandTouchGraphic->GetIntrinsicWidth(&width);
            Int32 height;
            mHandTouchGraphic->GetIntrinsicHeight(&height);
            mHandTouchGraphic->SetBounds(cx + offset, cy + offset,
                    cx + width + offset,
                    cy + height + offset);
            mHandTouchGraphic->Draw(c);
        }

        canvas->DrawBitmap(b, 0.0, 0.0, NULL);
        c->SetBitmap(NULL);
        b = NULL;
    }

    // Draw the rest of the cling
    return FrameLayout::DispatchDraw(canvas);
}

AutoPtr<ArrayOf<Int32> > CCling::GetPunchThroughPositions()
{
    if (mDrawIdentifier.Equals(WORKSPACE_PORTRAIT)) {
        AutoPtr<ArrayOf<Int32> > array = ArrayOf<Int32>::Alloc(2);
        Int32 width;
        GetMeasuredWidth(&width);
        (*array)[0] = width / 2;
        Int32 height;
        GetMeasuredHeight(&height);
        (*array)[1] = height - (mButtonBarHeight / 2);
        return array;
    }
    else if (mDrawIdentifier.Equals(WORKSPACE_LANDSCAPE)) {
         AutoPtr<ArrayOf<Int32> > array = ArrayOf<Int32>::Alloc(2);
        Int32 width;
        GetMeasuredWidth(&width);
        (*array)[0] = width - (mButtonBarHeight / 2);
        Int32 height;
        GetMeasuredHeight(&height);
        (*array)[1] = height / 2;
        return array;
    }
    else if (mDrawIdentifier.Equals(WORKSPACE_LARGE)) {
        Float scale;
        assert(0 && "need class LauncherApplication");
        //LauncherApplication::GetScreenDensity(&scale);
        const Int32 cornerXOffset = (Int32) (scale * 15);
        const Int32 cornerYOffset = (Int32) (scale * 10);
        AutoPtr<ArrayOf<Int32> > array = ArrayOf<Int32>::Alloc(2);
        Int32 width;
        GetMeasuredWidth(&width);
        (*array)[0] = width - cornerXOffset;
        (*array)[1] = cornerYOffset;
        return array;
    }
    else if (mDrawIdentifier.Equals(ALLAPPS_PORTRAIT) ||
           mDrawIdentifier.Equals(ALLAPPS_LANDSCAPE) ||
           mDrawIdentifier.Equals(ALLAPPS_LARGE)) {
        return mPositionData;
    }
    AutoPtr<ArrayOf<Int32> > array = ArrayOf<Int32>::Alloc(2);
    (*array)[0] = -1;
    (*array)[1] = -1;
    return array;
}

} // namespace Launcher2
} // namespace Droid
} // namespace Elastos