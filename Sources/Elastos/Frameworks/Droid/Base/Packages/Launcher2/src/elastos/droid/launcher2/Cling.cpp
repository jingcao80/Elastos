
#include "elastos/droid/launcher2/CheckLongPressHelper.h"
#include "Elastos.Droid.Service.h"
#include "R.h"

namespace Elastos {
namespace Droid {
namespace Launcher2 {

const String Cling::WORKSPACE_CLING_DISMISSED_KEY("cling.workspace.dismissed");
const String Cling::ALLAPPS_CLING_DISMISSED_KEY("cling.allapps.dismissed");
const String Cling::FOLDER_CLING_DISMISSED_KEY("cling.folder.dismissed");

String Cling::WORKSPACE_PORTRAIT("workspace_portrait");
String Cling::WORKSPACE_LANDSCAPE("workspace_landscape");
String Cling::WORKSPACE_LARGE("workspace_large");
String Cling::WORKSPACE_CUSTOM("workspace_custom");

String Cling::ALLAPPS_PORTRAIT("all_apps_portrait");
String Cling::ALLAPPS_LANDSCAPE("all_apps_landscape");
String Cling::ALLAPPS_LARGE("all_apps_large");

String Cling::FOLDER_PORTRAIT("folder_portrait");
String Cling::FOLDER_LANDSCAPE("folder_landscape");
String Cling::FOLDER_LARGE("folder_large");

Cling::Cling(
    /* [in] */ IContext* context)
    : mIsInitialized(FALSE)
    , mDrawIdentifier(NULL)
    , mPunchThroughGraphicCenterRadius(0)
    , mAppIconSize(0)
    , mButtonBarHeight(0)
    , mRevealRadius(0.0f)
{
    Cling(context, NULL, 0);
}

Cling::Cling(
    /* [in] */ IContext* context,
    /* [in] */ IAttributeSet* attrs)
    : mIsInitialized(FALSE)
    , mDrawIdentifier(NULL)
    , mPunchThroughGraphicCenterRadius(0)
    , mAppIconSize(0)
    , mButtonBarHeight(0)
    , mRevealRadius(0.0f)
{
    Cling(context, attrs, 0);
}

Cling::Cling(
    /* [in] */ IContext* context,
    /* [in] */ IAttributeSet* attrs,
    /* [in] */ Int32 defStyle)
    : mIsInitialized(FALSE)
    , mDrawIdentifier(NULL)
    , mPunchThroughGraphicCenterRadius(0)
    , mAppIconSize(0)
    , mButtonBarHeight(0)
    , mRevealRadius(0.0f)
{
    FrameLayout(context, attrs, defStyle);

    AutoPtr<ITypedArray> a;
    context->ObtainStyledAttributes(attrs, Elastos::Droid::Launcher2::R::styleable::Cling,
            defStyle, 0, (ITypedArray**)&a);
    a->GetString(Elastos::Droid::Launcher2::R::styleable::Cling_drawIdentifier, mDrawIdentifier);
    a->Recycle();

    SetClickable(TRUE);
}

void Cling::Init(
    /* [in] */ ILauncher* l,
    /* [in] */ ArrayOf<Int32>* positionData)
{
    if (!mIsInitialized) {
        mLauncher = l;
        mPositionData = positionData;

        AutoPtr<IContext> context;
        GetContext((IContext**)&context);
        AutoPtr<IResources> r;
        context->GetResources((IResources**)&r););

        r->GetDrawable(Elastos::Droid::Launcher2::R::drawable::cling, (IDrawable**)&mPunchThroughGraphic);
        r->GetDimensionPixelSize(Elastos::Droid::Launcher2::R::dimen::clingPunchThroughGraphicCenterRadius,
                &mPunchThroughGraphicCenterRadius);
        r->GetDimensionPixelSize(Elastos::Droid::Launcher2::R::dimen::app_icon_size,
                &mAppIconSize);

        Int32 size;
        r->GetDimensionPixelSize(Elastos::Droid::Launcher2::R::dimen::reveal_radius, &size);
        mRevealRadius = size * 1f;
        r->GetDimensionPixelSize(Elastos::Droid::Launcher2::R::dimen::button_bar_height, &mButtonBarHeight);

        CPaint::New((Paint**)&mErasePaint);
        AutoPtr<IPorterDuffXfermode> mode;
        CPorterDuffXfermode::New(PorterDuffMode::MULTIPLY, (IPorterDuffXfermode**)&mode);
        mErasePaint->SetXfermode(mode);
        mErasePaint->SetColor(0xFFFFFF);
        mErasePaint->SetAlpha(0);

        mIsInitialized = TRUE;
    }
}

void Cling::Cleanup()
{
    mBackground = NULL;
    mPunchThroughGraphic = NULL;
    mHandTouchGraphic = NULL;
    mIsInitialized = FALSE;
}

String Cling::GetDrawIdentifier()
{
    return mDrawIdentifier;
}

ECode Cling::FocusSearch(
    /* [in] */ Int32 direction,
    /* [out] */ IView** res)
{
    VALIDATE_NOT_NULL(res);

    return FocusSearch(this, direction, res);
}

ECode Cling::FocusSearch(
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

ECode Cling::OnHoverEvent(
    /* [in] */ IMotionEvent* event,
    /* [out] */ Boolean* res)
{
    VALIDATE_NOT_NULL(res);

    Boolean tmp1, tmp2, tmp3, tmp4, tmp5, tmp6, tmp7;
    mDrawIdentifier.Equals(WORKSPACE_PORTRAIT, &tmp1);
    mDrawIdentifier.Equals(WORKSPACE_LANDSCAPE, &tmp2);
    mDrawIdentifier.Equals(WORKSPACE_LARGE, &tmp3);
    mDrawIdentifier.Equals(ALLAPPS_PORTRAIT, &tmp4);
    mDrawIdentifier.Equals(ALLAPPS_LANDSCAPE, &tmp5);
    mDrawIdentifier.Equals(ALLAPPS_LARGE, &tmp6);
    mDrawIdentifier.Equals(WORKSPACE_CUSTOM, &tmp7);
    *res = tmp1 || tmp2 || tmp3 || tmp4 || tmp5 || tmp6 || tmp7;
    return NOERROR;

}

ECode Cling::OnTouchEvent(
    /* [in] */ IMotionEvent* event,
    /* [out] */ Boolean* res)
{
    VALIDATE_NOT_NULL(res);

    Boolean tmp1, tmp2, tmp3, tmp4, tmp5, tmp6;
    mDrawIdentifier.Equals(WORKSPACE_PORTRAIT, &tmp1);
    mDrawIdentifier.Equals(WORKSPACE_LANDSCAPE, &tmp2);
    mDrawIdentifier.Equals(WORKSPACE_LARGE, &tmp3);
    mDrawIdentifier.Equals(ALLAPPS_PORTRAIT, &tmp4);
    mDrawIdentifier.Equals(ALLAPPS_LANDSCAPE, &tmp5);
    mDrawIdentifier.Equals(ALLAPPS_LARGE, &tmp6);
    if (tmp1 || tmp2 || tmp3 || tmp4 || tmp5 || tmp6) {
        AutoPtr<ArrayOf<Int32> > positions = GetPunchThroughPositions();
        for (Int32 i = 0; i < positions->GetLength(); i += 2) {
            Float x;
            event->GetX(&x);
            Float y;
            event->GetY(&y);
            Double diff = Math::Sqrt(Math::Pow(x - (*positions)[i], 2) +
                    Math::Pow(y - (*positions)[i + 1], 2));
            if (diff < mRevealRadius) {
                *res = FALSE;
                return NOERROR;
            }
        }
    }
    else {
        Boolean tmp7, tmp8, tmp9;
        mDrawIdentifier.Equals(FOLDER_PORTRAIT, &tmp7);
        mDrawIdentifier.Equals(FOLDER_LANDSCAPE, &tmp8);
        mDrawIdentifier.Equals(FOLDER_LARGE, &tmp9);
        if (tmp7 || tmp8 || tmp9) {
            AutoPtr<IWorkspace> workspace;
            mLauncher->GetWorkspace((IWorkspace**)&workspace);
            AutoPtr<IFolder> f;
            workspace->GetOpenFolder((IFolder**)&f);
            if (f != NULL) {
                AutoPtr<IRect> r;
                CRect::New((IRect**)&r);
                f->GetHitRect(r);
                Float x;
                event->GetX(&x);
                Float y;
                event->GetY(&y);
                Boolean tmp;
                r->Contains((Int32)x, (Int32)y, &tmp)
                if (tmp) {
                    *res = FALSE;
                    return NOERROR;
                }
            }
        }
    }
    *res = TRUE;
    return NOERROR;
}

ECode Cling::DispatchDraw(
    /* [in] */ ICanvas* canvas)
{
    if (mIsInitialized) {
        AutoPtr<IDisplayMetrics> metrics;
        CDisplayMetrics::New((IDisplayMetrics**)&metrics);

        AutoPtr<IWindowManager> manager;
        mLauncher->GetWindowManager((IWindowManager**)&manager);
        AutoPtr<IDisplay> display;
        manager->GetDefaultDisplay((IDisplay**)&display);
        display->GetMetrics(metrics);

        // Initialize the draw buffer (to allow punching through)
        Int32 width;
        GetMeasuredWidth(&width);
        Int32 height;
        GetMeasuredHeight(&height);
        AutoPtr<IBitmap> b;
        Bitmap::CreateBitmap(width, height, Bitmap::Config::ARGB_8888, (IBitmap**)&b);
        AutoPtr<ICanvas> c;
        CCanvas::New(b, (ICanvas**)&c);

        // Draw the background
        if (mBackground == NULL) {
            Boolean tmp1, tmp2, tmp3;
            mDrawIdentifier.Equals(WORKSPACE_PORTRAIT, &tmp1);
            mDrawIdentifier.Equals(WORKSPACE_LANDSCAPE, &tmp2);
            mDrawIdentifier.Equals(WORKSPACE_LARGE, &tmp3);
            if (tmp1 || tmp2 || tmp3) {
                AutoPtr<IResources> resources;
                getResources((IResources**)&resources);
                resources->GetDrawable(Elastos::Droid::Launcher2::R::drawable::bg_cling1,
                        (IDrawable**)&mBackground);
            }
            else {
                Boolean tmp4, tmp5, tmp6;
                mDrawIdentifier.equals(ALLAPPS_PORTRAIT, &tmp4);
                mDrawIdentifier.equals(ALLAPPS_LANDSCAPE, &tmp5);
                mDrawIdentifier.equals(ALLAPPS_LARGE, &tmp6);
                if (tmp4 || tmp5 || tmp6) {
                    AutoPtr<IResources> resources;
                    GetResources((IResources**)&resources);
                    resources->GetDrawable(Elastos::Droid::Launcher2::R::drawable::bg_cling2,
                            (IDrawable**)&mBackground);
                }
                else {
                    Boolean tmp7, tmp8;
                    mDrawIdentifier.Equals(FOLDER_PORTRAIT, &tmp7);
                    mDrawIdentifier.Equals(FOLDER_LANDSCAPE, &tmp8);
                    if (tmp7 || tmp8) {
                        AutoPtr<IResources> resources;
                        GetResources((IResources**)&resources);
                        resources->GetDrawable(Elastos::Droid::Launcher2::R::drawable::bg_cling3,
                                (IDrawable**)&mBackground);
                    }
                    else {
                        Boolean tmp9;
                        mDrawIdentifier.Equals(FOLDER_LARGE, &tmp9);
                        if (tmp9) {
                            AutoPtr<IResources> resources;
                            GetResources((IResources**)&resources);
                            resources->GetDrawable(Elastos::Droid::Launcher2::R::drawable::bg_cling4,
                                    (IDrawable**)&mBackground);
                        }
                        else {
                            Boolean tmp10;
                            mDrawIdentifier.Equals(WORKSPACE_CUSTOM, &tmp10);
                            if (tmp10) {
                                AutoPtr<IResources> resources;
                                GetResources((IResources**)&resources);
                                resources->GetDrawable(Elastos::Droid::Launcher2::R::drawable::bg_cling4,
                                        (IDrawable**)&mBackground);
                            }
                        }
                    }
                }
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
        Int32 width;
        mPunchThroughGraphic->GetIntrinsicWidth(&width);
        Int32 dw = (Int32)(scale * width);
        Int32 height;
        mPunchThroughGraphic->GetIntrinsicHeight(&height);
        Int32 dh = (Int32)(scale * height);

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
        Boolean tmp1, tmp2, tmp3;
        mDrawIdentifier.Equals(ALLAPPS_PORTRAIT, &tmp1);
        mDrawIdentifier.Equals(ALLAPPS_LANDSCAPE, &tmp2);
        mDrawIdentifier.Equals(ALLAPPS_LARGE, &tmp3);
        if (tmp1 || tmp2 || tmp3) {
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

        canvas->DrawBitmap(b, 0, 0, NULL);
        c->SetBitmap(NULL);
        b = NULL;
    }

    // Draw the rest of the cling
    return FrameLayout::DispatchDraw(canvas);
}

AutoPtr<ArrayOf<Int32> > Cling::GetPunchThroughPositions()
{
    Boolean res;
    mDrawIdentifier.Equals(WORKSPACE_PORTRAIT, &res);
    if (res) {
        AutoPtr<ArrayOf<Int32> > array = ArrayOf<Int32>::Alloc(2);
        Int32 width;
        GetMeasuredWidth(&width);
        (*array)[0] = width / 2;
        Int32 height;
        GetMeasuredHeight(&height);
        (*array)[1] = height - (mButtonBarHeight / 2);
        return array;
    }
    else {
        Boolean tmp;
        mDrawIdentifier.Equals(WORKSPACE_LANDSCAPE, &tmp);
        if (tmp) {
            AutoPtr<ArrayOf<Int32> > array = ArrayOf<Int32>::Alloc(2);
            Int32 width;
            GetMeasuredWidth(&width);
            (*array)[0] = width - (mButtonBarHeight / 2);
            Int32 height;
            GetMeasuredHeight(&height);
            (*array)[1] = height / 2;
            return array;
        }
        else {
            Boolean tmp;
            mDrawIdentifier.Equals(WORKSPACE_LARGE, &tmp);
            if (tmp) {
                Float scale;
                LauncherApplication::GetScreenDensity(&scale);
                const Int32 cornerXOffset = (Int32) (scale * 15);
                const Int32 cornerYOffset = (Int32) (scale * 10);
                AutoPtr<ArrayOf<Int32> > array = ArrayOf<Int32>::Alloc(2);
                Int32 width;
                GetMeasuredWidth(&width);
                (*array)[0] = width - cornerXOffset;
                (*array)[1] = cornerYOffset;
                return array;
            }
            else {
                Boolean tmp1, tmp2, tmp3;
                mDrawIdentifier.Equals(ALLAPPS_PORTRAIT, &tmp1);
                mDrawIdentifier.Equals(ALLAPPS_LANDSCAPE, &tmp2);
                mDrawIdentifier.Equals(ALLAPPS_LARGE, &tmp3);
                if (tmp1 || tmp2 || tmp3) {
                    return mPositionData;
                }
            }
        }
    }
    AutoPtr<ArrayOf<Int32> > array = ArrayOf<Int32>::Alloc(2);
    (*array)[0] = -1;
    (*array)[1] = -1;
    return array;
}

} // namespace Launcher2
} // namespace Droid
} // namespace Elastos