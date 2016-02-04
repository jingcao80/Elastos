
#include "elastos/droid/ext/frameworkext.h"
#include "Elastos.Droid.Graphics.h"
#include "Elastos.Droid.View.h"
#include "elastos/droid/content/res/CCompatibilityInfo.h"
//#include "elastos/droid/content/pm/CApplicationInfo.h"
//#include "elastos/droid/graphics/CPoint.h"
//#include "elastos/droid/graphics/CPointF.h"
//#include "elastos/droid/graphics/CRect.h"
//#include "elastos/droid/graphics/CRegion.h"
#include "elastos/droid/utility/CDisplayMetrics.h"
//#include "elastos/droid/hardware/display/DisplayManagerGlobal.h"

//using Elastos::Droid::Content::Pm::CApplicationInfo;
using Elastos::Droid::View::IDisplay;
//using Elastos::Droid::Hardware::Display::DisplayManagerGlobal;
using Elastos::Droid::Graphics::IPoint;
//using Elastos::Droid::Graphics::CPoint;
//using Elastos::Droid::Graphics::CPointF;
//using Elastos::Droid::Graphics::CRect;
//using Elastos::Droid::Graphics::CRegion;
using Elastos::Droid::Utility::CDisplayMetrics;


namespace Elastos {
namespace Droid {
namespace Content {
namespace Res {

CAR_INTERFACE_IMPL(CCompatibilityInfo::Translator, Object, ICompatibilityInfoTranslator)

CCompatibilityInfo::Translator::Translator(
    /* [in] */ Float applicationScale,
    /* [in] */ Float applicationInvertedScale)
    : mApplicationScale(applicationScale)
    , mApplicationInvertedScale(applicationInvertedScale)
{}

CCompatibilityInfo::Translator::Translator()
    : mApplicationScale(0)
    , mApplicationInvertedScale(0)
{}

ECode CCompatibilityInfo::Translator::TranslateRectInScreenToAppWinFrame(
    /* [in] */ IRect* rect)
{
    return rect->Scale(mApplicationInvertedScale);
}

ECode CCompatibilityInfo::Translator::TranslateRegionInWindowToScreen(
    /* [in] */ IRegion* transparentRegion)
{
    return transparentRegion->Scale(mApplicationScale);
}

/**
* Apply translation to the canvas that is necessary to draw the content.
*/
ECode CCompatibilityInfo::Translator::TranslateCanvas(
    /* [in] */ ICanvas* canvas)
{
    if (mApplicationScale == 1.5f) {
        /*  When we scale for compatibility, we can put our stretched
        bitmaps and ninepatches on exacty 1/2 pixel boundaries,
        which can give us inconsistent drawing due to imperfect
        Float precision in the graphics engine's inverse matrix.

        As a work-around, we translate by a tiny amount to avoid
        landing on exact pixel centers and boundaries, giving us
        the slop we need to draw consistently.

        This constant is meant to resolve to 1/255 after it is
        scaled by 1.5 (mApplicationScale). Note, this is just a guess
        as to what is small enough not to create its own artifacts,
        and big enough to avoid the precision problems. Feel free
        to experiment with smaller values as you choose.
        */
        Float tinyOffset = 2.0f / (3 * 255);
        canvas->Translate(tinyOffset, tinyOffset);
    }

    return canvas->Scale(mApplicationScale, mApplicationScale);
}

ECode CCompatibilityInfo::Translator::TranslateEventInScreenToAppWindow(
    /* [in] */ IMotionEvent* event)
{
    return event->Scale(mApplicationInvertedScale);
}

/**
* Translate the window's layout parameter, from application's view to
* Screen's view.
*/
ECode CCompatibilityInfo::Translator::TranslateWindowLayout(
    /* [in] */ IWindowManagerLayoutParams* params)
{
    return params->Scale(mApplicationScale);
}

/**
* Translate a Rect in application's window to screen.
*/
ECode CCompatibilityInfo::Translator::TranslateRectInAppWindowToScreen(
    /* [in] */ IRect* rect)
{
    return rect->Scale(mApplicationScale);
}

ECode CCompatibilityInfo::Translator::TranslateRectInScreenToAppWindow(
    /* [in] */ IRect* rect)
{
    return rect->Scale(mApplicationInvertedScale);
}

ECode CCompatibilityInfo::Translator::TranslatePointInScreenToAppWindow(
    /* [in] */ IPointF* point)
{
    assert(0 && "TODO");
    Float scale = mApplicationInvertedScale;
    if (scale != 1.0f) {
        // AutoPtr<CPointF> cls = (CPointF*)point;
        // cls->mX = cls->mX * scale;
        // cls->mY = cls->mY * scale;
    }

    return NOERROR;
}

ECode CCompatibilityInfo::Translator::TranslateLayoutParamsInAppWindowToScreen(
   /* [in] */ IWindowManagerLayoutParams* params)
{
    return params->Scale(mApplicationScale);
}

ECode CCompatibilityInfo::Translator::GetTranslatedContentInsets(
    /* [in] */ IRect* contentInsets,
    /* [out] */ IRect** rect)
{
    VALIDATE_NOT_NULL(rect);

    if (mContentInsetsBuffer == NULL) {
        // FAIL_RETURN(CRect::New((IRect**)&mContentInsetsBuffer));
    }

    mContentInsetsBuffer->Set(contentInsets);
    TranslateRectInAppWindowToScreen(mContentInsetsBuffer);
    *rect = mContentInsetsBuffer;
    REFCOUNT_ADD(*rect);

    return NOERROR;
}

ECode CCompatibilityInfo::Translator::GetTranslatedVisibleInsets(
    /* [in] */ IRect* visibleInsets,
    /* [out] */ IRect** rect)
{
    VALIDATE_NOT_NULL(rect);

    if (mVisibleInsetsBuffer == NULL) {
        // FAIL_RETURN(CRect::New((IRect**)&mVisibleInsetsBuffer));
    }

    mVisibleInsetsBuffer->Set(visibleInsets);
    TranslateRectInAppWindowToScreen(mVisibleInsetsBuffer);
    *rect = mVisibleInsetsBuffer;
    REFCOUNT_ADD(*rect);

    return NOERROR;
}

ECode CCompatibilityInfo::Translator::GetApplicationScale(
    /* [out] */ Float* appScale)
{
    VALIDATE_NOT_NULL(appScale);
    *appScale = mApplicationScale;
    return NOERROR;
}

ECode CCompatibilityInfo::Translator::GetApplicationInvertedScale(
    /* [out] */ Float* appInvertedScale)
{
    VALIDATE_NOT_NULL(appInvertedScale);
    *appInvertedScale = mApplicationInvertedScale;
    return NOERROR;
}

ECode CCompatibilityInfo::Translator::GetTranslatedTouchableArea(
    /* [in] */ IRegion* touchableArea,
    /* [out] */ IRegion** region)
{
    VALIDATE_NOT_NULL(region);

    if (mTouchableAreaBuffer == NULL) {
        // FAIL_RETURN(CRegion::New((IRegion**)&mTouchableAreaBuffer));
    }
    Boolean result;
    mTouchableAreaBuffer->Set(touchableArea, &result);
    mTouchableAreaBuffer->Scale(mApplicationScale);
    *region = mTouchableAreaBuffer;
    REFCOUNT_ADD(*region);
    return NOERROR;
}


static AutoPtr<ICompatibilityInfo> InitDefaultCompatibilityInfo()
{
    AutoPtr<ICompatibilityInfo> info;
    ASSERT_SUCCEEDED(CCompatibilityInfo::New((ICompatibilityInfo**)&info));
    return info;
}

INIT_PROI_3 const AutoPtr<ICompatibilityInfo> CCompatibilityInfo::DEFAULT_COMPATIBILITY_INFO
        = InitDefaultCompatibilityInfo();

const Int32 CCompatibilityInfo::SCALING_REQUIRED = 1;
const Int32 CCompatibilityInfo::ALWAYS_NEEDS_COMPAT = 2;
const Int32 CCompatibilityInfo::NEVER_NEEDS_COMPAT = 4;
const Int32 CCompatibilityInfo::NEEDS_SCREEN_COMPAT = 8;

CAR_INTERFACE_IMPL(CCompatibilityInfo, Object, ICompatibilityInfo)

CAR_OBJECT_IMPL(CCompatibilityInfo)

CCompatibilityInfo::CCompatibilityInfo()
    : mApplicationDensity(0)
    , mApplicationScale(0)
    , mApplicationInvertedScale(0)
    , mCompatibilityFlags(0)
{}

ECode CCompatibilityInfo::IsScalingRequired(
    /* [out] */ Boolean* isRequire)
{
    VALIDATE_NOT_NULL(isRequire);
    *isRequire = (mCompatibilityFlags & SCALING_REQUIRED) != 0;
    return NOERROR;
}

ECode CCompatibilityInfo::SupportsScreen(
    /* [out] */ Boolean* isSupport)
{
    VALIDATE_NOT_NULL(isSupport);
    *isSupport = (mCompatibilityFlags & NEEDS_SCREEN_COMPAT) == 0;
    return NOERROR;
}

ECode CCompatibilityInfo::NeverSupportsScreen(
    /* [out] */ Boolean* value)
{
    VALIDATE_NOT_NULL(value);
    *value = (mCompatibilityFlags & ALWAYS_NEEDS_COMPAT) != 0;
    return NOERROR;
}

ECode CCompatibilityInfo::AlwaysSupportsScreen(
    /* [out] */ Boolean* value)
{
    VALIDATE_NOT_NULL(value);
    *value = (mCompatibilityFlags & NEVER_NEEDS_COMPAT) != 0;
    return NOERROR;
}

ECode CCompatibilityInfo::GetTranslator(
    /* [out] */ ICompatibilityInfoTranslator** translator)
{
    VALIDATE_NOT_NULL(translator);

    Boolean isRequire;
    IsScalingRequired(&isRequire);
    if (isRequire) {
        *translator = (ICompatibilityInfoTranslator*)new Translator();
        REFCOUNT_ADD(*translator);
    }
    else {
        *translator = NULL;
    }
    return NOERROR;
}

ECode CCompatibilityInfo::ApplyToDisplayMetrics(
    /* [in] */ IDisplayMetrics* inoutDm)
{
    Boolean isSupport;
    SupportsScreen(&isSupport);
    if (!isSupport) {
        // This is a larger screen device and the app is not
        // compatible with large screens, so diddle it.
        ComputeCompatibleScaling(inoutDm, inoutDm);
    }
    else {
        Int32 noncompatWidthPixels;
        inoutDm->GetNoncompatWidthPixels(&noncompatWidthPixels);
        inoutDm->SetWidthPixels(noncompatWidthPixels);

        Int32 noncompatHeightPixels;
        inoutDm->GetNoncompatHeightPixels(&noncompatHeightPixels);
        inoutDm->SetHeightPixels(noncompatHeightPixels);
    }

    Boolean isScalingReq = FALSE;
    IsScalingRequired(&isScalingReq);
    if (isScalingReq) {
        Float invertedRatio = mApplicationInvertedScale;
        Float noncompatDensity;
        inoutDm->GetNoncompatDensity(&noncompatDensity);
        inoutDm->SetDensity(noncompatDensity * invertedRatio);

        Int32 noncompatDensityDpi = 0;
        inoutDm->GetNoncompatDensityDpi(&noncompatDensityDpi);
        inoutDm->SetDensityDpi((Int32)((noncompatDensityDpi * invertedRatio) + .5f));

        Float noncompatScaledDensity;
        inoutDm->GetNoncompatScaledDensity(&noncompatScaledDensity);
        inoutDm->SetScaledDensity(noncompatScaledDensity * invertedRatio);

        Float noncompatXdpi;
        inoutDm->GetNoncompatXdpi(&noncompatXdpi);
        inoutDm->SetXdpi(noncompatXdpi * invertedRatio);

        Float noncompatYdpi;
        inoutDm->GetNoncompatYdpi(&noncompatYdpi);
        inoutDm->SetYdpi(noncompatYdpi * invertedRatio);

        Int32 widthPixels;
        inoutDm->GetWidthPixels(&widthPixels);
        inoutDm->SetWidthPixels((Int32)(widthPixels * invertedRatio + 0.5f));

        Int32 heightPixels;
        inoutDm->GetHeightPixels(&heightPixels);
        inoutDm->SetHeightPixels((Int32)(heightPixels * invertedRatio + 0.5f));
    }

    return NOERROR;
}

ECode CCompatibilityInfo::ApplyToConfiguration(
    /* [in] */ Int32 displayDensity,
    /* [in] */ IConfiguration* inoutConfig)
{
    Boolean isSupport;
    SupportsScreen(&isSupport);
    if (!isSupport) {
        // This is a larger screen device and the app is not
        // compatible with large screens, so we are forcing it to
        // run as if the screen is normal size.
        Int32 screenLayout;
        inoutConfig->GetScreenLayout(&screenLayout);
        inoutConfig->SetScreenLayout(
                (screenLayout & ~IConfiguration::SCREENLAYOUT_SIZE_MASK)
                | IConfiguration::SCREENLAYOUT_SIZE_NORMAL);

        Int32 compatScreenWidthDp;
        inoutConfig->GetCompatScreenWidthDp(&compatScreenWidthDp);
        inoutConfig->SetScreenWidthDp(compatScreenWidthDp);

        Int32 compatScreenHeightDp;
        inoutConfig->GetCompatScreenHeightDp(&compatScreenHeightDp);
        inoutConfig->SetScreenHeightDp(compatScreenHeightDp);

        Int32 compatSmallestScreenWidthDp;
        inoutConfig->GetCompatSmallestScreenWidthDp(&compatSmallestScreenWidthDp);
        inoutConfig->SetSmallestScreenWidthDp(compatSmallestScreenWidthDp);
    }

    inoutConfig->SetDensityDpi(displayDensity);

    Boolean isScalingReq = FALSE;
    IsScalingRequired(&isScalingReq);
    if (isScalingReq) {
        Float invertedRatio = mApplicationInvertedScale;
        Int32 densityDpi;
        inoutConfig->GetDensityDpi(&densityDpi);
        inoutConfig->SetDensityDpi((Int32)((densityDpi * invertedRatio) + .5f));
    }

    return NOERROR;
}

Float CCompatibilityInfo::ComputeCompatibleScaling(
    /* [in] */ IDisplayMetrics* dm,
    /* [in] */ IDisplayMetrics* outDm)
{
    Int32 width, height;
    dm->GetNoncompatWidthPixels(&width);
    dm->GetNoncompatHeightPixels(&height);
    Int32 shortSize, longSize;
    if (width < height) {
        shortSize = width;
        longSize = height;
    }
    else {
        shortSize = height;
        longSize = width;
    }
    Float density;
    dm->GetDensity(&density);
    Int32 newShortSize = (Int32)(DEFAULT_NORMAL_SHORT_DIMENSION * density + 0.5f);
    Float aspect = ((Float)longSize) / shortSize;
    if (aspect > MAXIMUM_ASPECT_RATIO) {
        aspect = MAXIMUM_ASPECT_RATIO;
    }
    Int32 newLongSize = (Int32)(newShortSize * aspect + 0.5f);
    Int32 newWidth, newHeight;
    if (width < height) {
        newWidth = newShortSize;
        newHeight = newLongSize;
    }
    else {
        newWidth = newLongSize;
        newHeight = newShortSize;
    }

    Float sw = width / (Float)newWidth;
    Float sh = height / (Float)newHeight;
    Float scale = sw < sh ? sw : sh;
    if (scale < 1) {
        scale = 1;
    }

    if (outDm != NULL) {
        outDm->SetWidthPixels(newWidth);
        outDm->SetHeightPixels(newHeight);
    }

    // //tellen add 2012/12/07 for apk compatibility
    // AutoPtr<IDisplay> display;
    // DisplayManagerGlobal::GetInstance()->GetRealDisplay(
    //     IDisplay::DEFAULT_DISPLAY, (IDisplay**)&display);
    // AutoPtr<IPoint> p;
    // ASSERT_SUCCEEDED(CPoint::New((IPoint**)&p));
    // display->GetRealSize(p);
    // //1024x768 resolution screen need reduce scale ratio in order to support special apks
    // // begin from this
    // Int32 x, y;
    // p->GetX(&x);
    // p->GetY(&y);
    // if( ((1024 == x) && (768 == y)) || ((1024 == y) && (768 == x)) ){
    //     return 2.0f;
    // }

    return scale;
}

ECode CCompatibilityInfo::Equals(
    /* [in] */ IInterface* obj,
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(result);
    *result = FALSE;

    if (TO_IINTERFACE(this) == obj) {
        *result = TRUE;
        return NOERROR;
    }

    AutoPtr<ICompatibilityInfo> info = ICompatibilityInfo::Probe(obj);

    if (info == NULL){
        *result = FALSE;
        return NOERROR;
    }

    AutoPtr<CCompatibilityInfo> oc = (CCompatibilityInfo*)info.Get();
    if (mCompatibilityFlags != oc->mCompatibilityFlags) {
        *result = FALSE;
        return NOERROR;
    }
    if (mApplicationDensity != oc->mApplicationDensity) {
        *result = FALSE;
        return NOERROR;
    }
    if (mApplicationScale != oc->mApplicationScale) {
        *result = FALSE;
        return NOERROR;
    }
    if (mApplicationInvertedScale != oc->mApplicationInvertedScale) {
        *result = FALSE;
        return NOERROR;
    }

    *result = TRUE;
    return NOERROR;
}

ECode CCompatibilityInfo::ReadFromParcel(
    /* [in] */ IParcel* source)
{
    source->ReadInt32(&mCompatibilityFlags);
    source->ReadInt32(&mApplicationDensity);
    source->ReadFloat(&mApplicationScale);
    source->ReadFloat(&mApplicationInvertedScale);
    return NOERROR;
}

ECode CCompatibilityInfo::WriteToParcel(
    /* [in] */ IParcel* dest)
{
    dest->WriteInt32(mCompatibilityFlags);
    dest->WriteInt32(mApplicationDensity);
    dest->WriteFloat(mApplicationScale);
    dest->WriteFloat(mApplicationInvertedScale);
    return NOERROR;
}

ECode CCompatibilityInfo::GetApplicationDensity(
    /* [out] */ Int32* applicationDensity)
{
    VALIDATE_NOT_NULL(applicationDensity);
    *applicationDensity = mApplicationDensity;
    return NOERROR;
}

ECode CCompatibilityInfo::SetApplicationDensity(
    /* [in] */ Int32 applicationDensity)
{
    mApplicationDensity = applicationDensity;
    return NOERROR;
}

ECode CCompatibilityInfo::GetApplicationScale(
    /* [out] */ Float* applicationScale)
{
    VALIDATE_NOT_NULL(applicationScale);
    *applicationScale = mApplicationScale;
    return NOERROR;
}

ECode CCompatibilityInfo::SetApplicationScale(
    /* [in] */ Float applicationScale)
{
    mApplicationScale = applicationScale;
    return NOERROR;
}

ECode CCompatibilityInfo::GetApplicationInvertedScale(
    /* [out] */ Float* applicationInvertedScale)
{
    VALIDATE_NOT_NULL(applicationInvertedScale);
    *applicationInvertedScale = mApplicationInvertedScale;
    return NOERROR;
}

ECode CCompatibilityInfo::SetApplicationInvertedScale(
    /* [in] */ Float applicationInvertedScale)
{
    mApplicationInvertedScale = applicationInvertedScale;
    return NOERROR;
}

ECode CCompatibilityInfo::constructor(
    /* [in] */ IApplicationInfo* appInfo,
    /* [in] */ Int32 screenLayout,
    /* [in] */ Int32 sw,
    /* [in] */ Boolean forceCompat)
{
    assert(0 && "TODO");
    // Int32 compatFlags = 0;

    // AutoPtr<CApplicationInfo> appInfoCls = (CApplicationInfo*)appInfo;

    // if (appInfoCls->mRequiresSmallestWidthDp != 0 || appInfoCls->mCompatibleWidthLimitDp != 0
    //         || appInfoCls->mLargestWidthLimitDp != 0) {
    //     // New style screen requirements spec.
    //     Int32 required = appInfoCls->mRequiresSmallestWidthDp != 0
    //             ? appInfoCls->mRequiresSmallestWidthDp
    //             : appInfoCls->mCompatibleWidthLimitDp;
    //     if (required == 0) {
    //         required = appInfoCls->mLargestWidthLimitDp;
    //     }
    //     Int32 compat = appInfoCls->mCompatibleWidthLimitDp != 0
    //             ? appInfoCls->mCompatibleWidthLimitDp : required;
    //     if (compat < required) {
    //         compat = required;
    //     }
    //     Int32 largest = appInfoCls->mLargestWidthLimitDp;

    //     if (required > DEFAULT_NORMAL_SHORT_DIMENSION) {
    //         // For now -- if they require a size larger than the only
    //         // size we can do in compatibility mode, then don't ever
    //         // allow the app to go in to compat mode.  Trying to run
    //         // it at a smaller size it can handle will make it far more
    //         // broken than running at a larger size than it wants or
    //         // thinks it can handle.
    //         compatFlags |= NEVER_NEEDS_COMPAT;
    //     }
    //     else if (largest != 0 && sw > largest) {
    //         // If the screen size is larger than the largest size the
    //         // app thinks it can work with, then always force it in to
    //         // compatibility mode.
    //         compatFlags |= NEEDS_SCREEN_COMPAT | ALWAYS_NEEDS_COMPAT;
    //     }
    //     else if (compat >= sw) {
    //         // The screen size is something the app says it was designed
    //         // for, so never do compatibility mode.
    //         compatFlags |= NEVER_NEEDS_COMPAT;
    //     }
    //     else if (forceCompat) {
    //         // The app may work better with or without compatibility mode.
    //         // Let the user decide.
    //         compatFlags |= NEEDS_SCREEN_COMPAT;
    //     }

    //     // Modern apps always support densities.
    //     mApplicationDensity = CDisplayMetrics::DENSITY_DEVICE;
    //     mApplicationScale = 1.0f;
    //     mApplicationInvertedScale = 1.0f;

    // }
    // else {
    //     /**
    //      * Has the application said that its UI is expandable?  Based on the
    //      * <supports-screen> android:expandible in the manifest.
    //      */
    //     Int32 EXPANDABLE = 2;

    //     /**
    //      * Has the application said that its UI supports large screens?  Based on the
    //      * <supports-screen> android:largeScreens in the manifest.
    //      */
    //     Int32 LARGE_SCREENS = 8;

    //     /**
    //      * Has the application said that its UI supports xlarge screens?  Based on the
    //      * <supports-screen> android:xlargeScreens in the manifest.
    //      */
    //     Int32 XLARGE_SCREENS = 32;

    //     Int32 sizeInfo = 0;

    //     // We can't rely on the application always setting
    //     // FLAG_RESIZEABLE_FOR_SCREENS so will compute it based on various input.
    //     Boolean anyResizeable = FALSE;

    //     if ((appInfoCls->mFlags & IApplicationInfo::FLAG_SUPPORTS_LARGE_SCREENS) != 0) {
    //         sizeInfo |= LARGE_SCREENS;
    //         anyResizeable = TRUE;
    //         if (!forceCompat) {
    //             // If we aren't forcing the app into compatibility mode, then
    //             // assume if it supports large screens that we should allow it
    //             // to use the full space of an xlarge screen as well.
    //             sizeInfo |= XLARGE_SCREENS | EXPANDABLE;
    //         }
    //     }

    //     if ((appInfoCls->mFlags & IApplicationInfo::FLAG_SUPPORTS_XLARGE_SCREENS) != 0) {
    //         anyResizeable = TRUE;
    //         if (!forceCompat) {
    //             sizeInfo |= XLARGE_SCREENS | EXPANDABLE;
    //         }
    //     }

    //     if ((appInfoCls->mFlags & IApplicationInfo::FLAG_RESIZEABLE_FOR_SCREENS) != 0) {
    //         anyResizeable = TRUE;
    //         sizeInfo |= EXPANDABLE;
    //     }

    //     if (forceCompat) {
    //         // If we are forcing compatibility mode, then ignore an app that
    //         // just says it is resizable for screens.  We'll only have it fill
    //         // the screen if it explicitly says it supports the screen size we
    //         // are running in.
    //         sizeInfo &= ~EXPANDABLE;
    //     }

    //     compatFlags |= NEEDS_SCREEN_COMPAT;
    //     switch (screenLayout & IConfiguration::SCREENLAYOUT_SIZE_MASK) {
    //         case IConfiguration::SCREENLAYOUT_SIZE_XLARGE:
    //             if ((sizeInfo & XLARGE_SCREENS) != 0) {
    //                 compatFlags &= ~NEEDS_SCREEN_COMPAT;
    //             }
    //             if ((appInfoCls->mFlags & IApplicationInfo::FLAG_SUPPORTS_XLARGE_SCREENS) != 0) {
    //                 compatFlags |= NEVER_NEEDS_COMPAT;
    //             }
    //             break;
    //         case IConfiguration::SCREENLAYOUT_SIZE_LARGE:
    //             if ((sizeInfo & LARGE_SCREENS) != 0) {
    //                 compatFlags &= ~NEEDS_SCREEN_COMPAT;
    //             }
    //             if ((appInfoCls->mFlags & IApplicationInfo::FLAG_SUPPORTS_LARGE_SCREENS) != 0) {
    //                 compatFlags |= NEVER_NEEDS_COMPAT;
    //             }
    //             break;
    //     }

    //     if ((screenLayout & IConfiguration::SCREENLAYOUT_COMPAT_NEEDED) != 0) {
    //         if ((sizeInfo&EXPANDABLE) != 0) {
    //             compatFlags &= ~NEEDS_SCREEN_COMPAT;
    //         }
    //         else if (!anyResizeable) {
    //             compatFlags |= ALWAYS_NEEDS_COMPAT;
    //         }
    //     }
    //     else {
    //         compatFlags &= ~NEEDS_SCREEN_COMPAT;
    //         compatFlags |= NEVER_NEEDS_COMPAT;
    //     }

    //     if ((appInfoCls->mFlags & IApplicationInfo::FLAG_SUPPORTS_SCREEN_DENSITIES) != 0) {
    //         mApplicationDensity = CDisplayMetrics::DENSITY_DEVICE;
    //         mApplicationScale = 1.0f;
    //         mApplicationInvertedScale = 1.0f;
    //     }
    //     else {
    //         mApplicationDensity = IDisplayMetrics::DENSITY_DEFAULT;
    //         mApplicationScale = CDisplayMetrics::DENSITY_DEVICE
    //                 / (Float) IDisplayMetrics::DENSITY_DEFAULT;
    //         mApplicationInvertedScale = 1.0f / mApplicationScale;
    //         compatFlags |= SCALING_REQUIRED;
    //     }
    // }

    // mCompatibilityFlags = compatFlags;

    return NOERROR;
}

ECode CCompatibilityInfo::constructor(
    /* [in] */ Int32 compFlags,
    /* [in] */ Int32 dens,
    /* [in] */ Float scale,
    /* [in] */ Float invertedScale)
{
    mCompatibilityFlags = compFlags;
    mApplicationDensity = dens;
    mApplicationScale = scale;
    mApplicationInvertedScale = invertedScale;
    return NOERROR;
}

ECode CCompatibilityInfo::constructor()
{
    return constructor(NEVER_NEEDS_COMPAT, CDisplayMetrics::DENSITY_DEVICE, 1.0f, 1.0f);
}

} // namespace Res
} // namespace Content
} // namespace Droid
} // namespace Elastos
